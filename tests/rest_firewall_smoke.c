#include <dcc/dcc.h>

#if !defined(_WIN32)
#include "support/http_smoke_server.h"
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>

static int check_standalone_firewall(void) {
    dcc_rest_firewall_options_t opts;
    dcc_rest_firewall_options_init(&opts);
    opts.invalid_request_soft_limit = 1U;
    opts.invalid_request_hard_limit = 2U;
    opts.soft_limit_delay_ms = 7U;

    dcc_rest_firewall_t firewall;
    if (dcc_rest_firewall_init(&firewall, &opts) != DCC_OK) {
        fprintf(stderr, "firewall init failed\n");
        return 0;
    }

    dcc_rest_firewall_request_t request = {
        .size = sizeof(request),
        .method = "POST",
        .path = "/channels/1/messages",
        .now_ms = 1000U,
        .critical = 0U,
    };
    dcc_rest_firewall_result_t result;
    if (dcc_rest_firewall_check(&firewall, &request, &result) != DCC_OK ||
        result.decision != DCC_REST_FIREWALL_ALLOW ||
        dcc_rest_firewall_record_response(&firewall, 401U, 1001U) != DCC_OK) {
        fprintf(stderr, "firewall first request failed\n");
        dcc_rest_firewall_deinit(&firewall);
        return 0;
    }

    request.now_ms = 1002U;
    if (dcc_rest_firewall_check(&firewall, &request, &result) != DCC_OK ||
        result.decision != DCC_REST_FIREWALL_DELAY ||
        result.reason != DCC_REST_FIREWALL_REASON_SOFT_INVALID_REQUEST_LIMIT ||
        result.delay_ms != 7U ||
        dcc_rest_firewall_record_response(&firewall, 429U, 1003U) != DCC_OK) {
        fprintf(stderr, "firewall soft delay failed\n");
        dcc_rest_firewall_deinit(&firewall);
        return 0;
    }

    request.now_ms = 1004U;
    if (dcc_rest_firewall_check(&firewall, &request, &result) != DCC_OK ||
        result.decision != DCC_REST_FIREWALL_REJECT ||
        result.reason != DCC_REST_FIREWALL_REASON_HARD_INVALID_REQUEST_LIMIT) {
        fprintf(stderr, "firewall hard reject failed\n");
        dcc_rest_firewall_deinit(&firewall);
        return 0;
    }

    request.path = "/interactions/1/tok/callback";
    request.now_ms = 1005U;
    if (dcc_rest_firewall_check(&firewall, &request, &result) != DCC_OK ||
        result.decision != DCC_REST_FIREWALL_DELAY ||
        !result.critical) {
        fprintf(stderr, "firewall critical path handling failed\n");
        dcc_rest_firewall_deinit(&firewall);
        return 0;
    }

    dcc_rest_firewall_deinit(&firewall);
    return 1;
}

#if !defined(_WIN32)
static void firewall_response_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    if (response != NULL && user_data != NULL) {
        *(uint16_t *)user_data = response->status;
    }
}

static int check_attached_firewall(void) {
    dcc_client_options_t client_opts = {
        .size = sizeof(client_opts),
        .token = "firewall-token",
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&client_opts, &client) != DCC_OK) {
        fprintf(stderr, "client create failed\n");
        return 0;
    }

    dcc_rest_firewall_options_t opts;
    dcc_rest_firewall_options_init(&opts);
    opts.invalid_request_soft_limit = 1U;
    opts.invalid_request_hard_limit = 1U;
    opts.soft_limit_delay_ms = 0U;
    if (dcc_rest_firewall_attach(client, &opts) != DCC_OK) {
        fprintf(stderr, "firewall attach failed\n");
        dcc_client_destroy(client);
        return 0;
    }

    http_server_t server;
    pthread_t thread;
    if (start_server_mode(&server, &thread, 6) != 0) {
        fprintf(stderr, "failed to start firewall server\n");
        dcc_client_destroy(client);
        return 0;
    }
    set_api_base_for_server(&server);

    uint16_t first_status = 0;
    if (dcc_rest_request(client, "GET", "/channels/123", NULL, firewall_response_cb, &first_status) != DCC_OK ||
        first_status != 401U) {
        fprintf(stderr, "first attached request failed: %u\n", (unsigned)first_status);
        dcc_client_destroy(client);
        close(server.fd);
        (void)pthread_join(thread, NULL);
        return 0;
    }
    (void)pthread_join(thread, NULL);
    close(server.fd);

    dcc_rest_firewall_snapshot_t snapshot = {
        .size = sizeof(snapshot),
    };
    if (dcc_rest_firewall_snapshot(client, &snapshot) != DCC_OK ||
        !snapshot.attached ||
        snapshot.invalid_request_count != 1U ||
        !snapshot.hard_limit_exceeded) {
        fprintf(stderr, "attached snapshot failed\n");
        dcc_client_destroy(client);
        return 0;
    }

    dcc_status_t rejected = dcc_rest_request(client, "GET", "/channels/456", NULL, NULL, NULL);
    if (rejected != DCC_ERR_STATE ||
        atomic_load_explicit(&server.requests_seen, memory_order_acquire) != 1U) {
        fprintf(stderr, "attached preflight reject failed: %s requests=%u\n",
                dcc_status_string(rejected),
                atomic_load_explicit(&server.requests_seen, memory_order_acquire));
        dcc_client_destroy(client);
        return 0;
    }

    char json[1024];
    size_t json_len = 0;
    if (dcc_rest_firewall_snapshot_json(&snapshot, json, sizeof(json), &json_len) != DCC_OK ||
        json_len == 0 ||
        strstr(json, "\"hard_limit_exceeded\":true") == NULL ||
        strstr(json, "\"invalid_request_count\":1") == NULL) {
        fprintf(stderr, "snapshot json failed: %s\n", json);
        dcc_client_destroy(client);
        return 0;
    }

    dcc_rest_firewall_detach(client);
    dcc_client_destroy(client);
    return 1;
}
#endif

int main(void) {
    if (!check_standalone_firewall()) {
        return 1;
    }
#if !defined(_WIN32)
    if (!check_attached_firewall()) {
        return 1;
    }
#endif
    if (!dcc_rest_firewall_status_is_invalid(401U) ||
        !dcc_rest_firewall_status_is_invalid(403U) ||
        !dcc_rest_firewall_status_is_invalid(429U) ||
        dcc_rest_firewall_status_is_invalid(400U) ||
        strcmp(dcc_rest_firewall_decision_string(DCC_REST_FIREWALL_DELAY), "delay") != 0 ||
        strcmp(
            dcc_rest_firewall_reason_string(DCC_REST_FIREWALL_REASON_HARD_INVALID_REQUEST_LIMIT),
            "hard_invalid_request_limit"
        ) != 0) {
        fprintf(stderr, "firewall helpers failed\n");
        return 1;
    }
    return 0;
}
