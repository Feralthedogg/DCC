#include "support/interaction_server_smoke_support.h"

#if !LLAM_PLATFORM_POSIX
int main(void) {
    return 0;
}
#else

#include <openssl/evp.h>

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>

static int interaction_validate_runtime_route_contract(
    dcc_interaction_server_t *server,
    interaction_seen_t *seen
) {
    dcc_interaction_route_t health_route = {
        .size = sizeof(health_route),
        .method = "GET",
        .path = "/healthz",
        .flags = DCC_INTERACTION_ROUTE_F_UNSIGNED,
        .callback = interaction_health_callback,
        .user_data = seen,
    };
    if (dcc_interaction_server_add_route(server, &health_route) != DCC_ERR_STATE ||
        dcc_interaction_server_add_health_route(server, "/latez") != DCC_ERR_STATE ||
        dcc_interaction_server_clear_routes(server) != DCC_ERR_STATE) {
        fprintf(stderr, "interaction route runtime mutation contract failed\n");
        return 0;
    }
    return 1;
}

static int interaction_validate_after_stop(
    dcc_interaction_server_t *server,
    interaction_wait_state_t *wait_state,
    pthread_t wait_thread
) {
    dcc_status_t st = dcc_interaction_server_stop(server);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_interaction_server_stop failed: %s\n", dcc_status_string(st));
        (void)pthread_join(wait_thread, NULL);
        return 0;
    }

    dcc_interaction_server_state_t server_state = {
        .size = sizeof(server_state),
    };
    if (dcc_interaction_server_get_state(server, &server_state) != DCC_OK ||
        server_state.stopping != 1U ||
        server_state.listening != 0U ||
        server_state.draining != 0U ||
        server_state.active_requests != 0U) {
        fprintf(
            stderr,
            "interaction stopping state mismatch: started=%u stopping=%u listening=%u draining=%u active=%llu\n",
            (unsigned)server_state.started,
            (unsigned)server_state.stopping,
            (unsigned)server_state.listening,
            (unsigned)server_state.draining,
            (unsigned long long)server_state.active_requests
        );
        (void)pthread_join(wait_thread, NULL);
        return 0;
    }

    if (pthread_join(wait_thread, NULL) != 0 || wait_state->status != DCC_OK) {
        fprintf(stderr, "interaction server wait failed: %s\n", dcc_status_string(wait_state->status));
        return 0;
    }
    if (dcc_interaction_server_get_state(server, &server_state) != DCC_OK ||
        server_state.started != 0U ||
        server_state.stopping != 1U ||
        server_state.listening != 0U ||
        server_state.active_requests != 0U ||
        server_state.completed_requests != 19U) {
        fprintf(
            stderr,
            "interaction stopped state mismatch: started=%u stopping=%u listening=%u active=%llu completed=%llu\n",
            (unsigned)server_state.started,
            (unsigned)server_state.stopping,
            (unsigned)server_state.listening,
            (unsigned long long)server_state.active_requests,
            (unsigned long long)server_state.completed_requests
        );
        return 0;
    }
    return 1;
}

int main(void) {
    static const unsigned char seed[32] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
    };
    EVP_PKEY *key = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, NULL, seed, sizeof(seed));
    if (key == NULL) {
        fprintf(stderr, "failed to create Ed25519 key\n");
        return 1;
    }

    unsigned char public_key[32];
    size_t public_key_len = sizeof(public_key);
    char public_key_hex[65];
    if (EVP_PKEY_get_raw_public_key(key, public_key, &public_key_len) != 1 ||
        public_key_len != sizeof(public_key)) {
        fprintf(stderr, "failed to read Ed25519 public key\n");
        EVP_PKEY_free(key);
        return 1;
    }
    interaction_hex_encode(public_key, sizeof(public_key), public_key_hex);

    interaction_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    atomic_init(&seen.slow_entered, 0U);
    atomic_init(&seen.slow_release, 0U);

    dcc_interaction_server_options_t options;
    memset(&options, 0, sizeof(options));
    options.size = sizeof(options);
    options.public_key_hex = public_key_hex;
    options.address = "127.0.0.1";
    options.port = 0;
    options.max_header_size = 1024U;
    options.max_body_size = 128U;
    options.callback = interaction_callback;
    options.user_data = &seen;

    dcc_interaction_server_t *server = NULL;
    dcc_status_t st = dcc_interaction_server_create(&options, &server);
    if (st != DCC_OK || server == NULL) {
        fprintf(stderr, "dcc_interaction_server_create failed: %s\n", dcc_status_string(st));
        EVP_PKEY_free(key);
        return 1;
    }

    int ok = 0;
    int wait_started = 0;
    interaction_wait_state_t wait_state;
    pthread_t wait_thread;

    if (!interaction_configure_smoke_routes(server, &seen)) {
        goto cleanup;
    }
    st = dcc_interaction_server_start(server);
    if (st != DCC_OK) {
        fprintf(
            stderr,
            "dcc_interaction_server_start failed: %s (%s)\n",
            dcc_status_string(st),
            dcc_interaction_server_last_error(server)
        );
        goto cleanup;
    }
    if (!interaction_validate_runtime_route_contract(server, &seen)) {
        goto cleanup_stop;
    }

    uint16_t port = dcc_interaction_server_port(server);
    if (port == 0U) {
        fprintf(stderr, "interaction server did not bind a port\n");
        goto cleanup_stop;
    }
    if (!interaction_validate_start_state(server, port)) {
        goto cleanup_stop;
    }

    wait_state.server = server;
    wait_state.status = DCC_ERR_RUNTIME;
    if (pthread_create(&wait_thread, NULL, interaction_wait_thread, &wait_state) != 0) {
        fprintf(stderr, "failed to start wait thread\n");
        goto cleanup_stop;
    }
    wait_started = 1;

    if (!interaction_run_request_smoke(server, key, public_key_hex, port, &seen) ||
        !interaction_run_health_and_drain_smoke(server, port, &seen) ||
        !interaction_validate_after_stop(server, &wait_state, wait_thread)) {
        wait_started = 0;
        goto cleanup_stop;
    }
    wait_started = 0;
    ok = 1;

cleanup_stop:
    if (!ok) {
        (void)dcc_interaction_server_stop(server);
        if (wait_started) {
            (void)pthread_join(wait_thread, NULL);
        }
    }
cleanup:
    dcc_interaction_server_destroy(server);
    EVP_PKEY_free(key);
    return ok ? 0 : 1;
}

#endif
