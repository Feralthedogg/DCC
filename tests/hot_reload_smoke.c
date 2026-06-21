#include <dcc/dcc.h>

#include "hot_reload_test_state.h"
#include "support/hot_reload_smoke_support.h"

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    if (dcc_hot_reload_smoke_generation_overflow() != 0) {
        return 1;
    }

    char path[256];
    int n = snprintf(path, sizeof(path), "/tmp/dcc-hot-reload-smoke-%ld.so", (long)getpid());
    if (n <= 0 || (size_t)n >= sizeof(path)) {
        return 1;
    }

    if (dcc_hot_reload_smoke_copy_file(DCC_HOT_RELOAD_MODULE_V1, path) != 0) {
        fprintf(stderr, "failed to copy v1 module\n");
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t status = dcc_client_create(&client_options, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "client create failed: %s\n", dcc_status_string(status));
        unlink(path);
        return 1;
    }

    hot_reload_test_state_t state;
    memset(&state, 0, sizeof(state));
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .poll_interval_ms = 20,
        .settle_interval_ms = DCC_HOT_RELOAD_NO_SETTLE,
        .user_data = &state,
    };
    status = dcc_hot_reload_create(client, path, &options, &hot_reload);
    if (status != DCC_OK) {
        fprintf(stderr, "hot reload create failed: %s\n", dcc_status_string(status));
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    status = dcc_hot_reload_reload(hot_reload);
    dcc_hot_reload_health_snapshot_t reload_health = {
        .size = sizeof(reload_health),
    };
    dcc_hot_reload_health_snapshot_t bad_reload_health = {0};
    char reload_health_json[1536];
    size_t reload_health_json_len = 0;
    if (status != DCC_OK ||
        dcc_hot_reload_generation(hot_reload) != 1 ||
        dcc_hot_reload_state_size(hot_reload) != sizeof(hot_reload_shared_state_t) ||
        atomic_load_explicit(&state.init_v1, memory_order_acquire) != 1U ||
        atomic_load_explicit(&state.deinit_v1, memory_order_acquire) != 0U ||
        atomic_load_explicit(&state.shared_init_v1, memory_order_acquire) != 0U) {
        fprintf(stderr,
                "initial hot reload failed: status=%s gen=%llu init=%u deinit=%u shared=%u err=%s\n",
                dcc_status_string(status),
                (unsigned long long)dcc_hot_reload_generation(hot_reload),
                atomic_load_explicit(&state.init_v1, memory_order_acquire),
                atomic_load_explicit(&state.deinit_v1, memory_order_acquire),
                atomic_load_explicit(&state.shared_init_v1, memory_order_acquire),
                dcc_hot_reload_last_error(hot_reload));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }
    if (dcc_hot_reload_health_snapshot(NULL, &reload_health) != DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_health_snapshot(hot_reload, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_health_snapshot(hot_reload, &bad_reload_health) != DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_health_snapshot(hot_reload, &reload_health) != DCC_OK ||
        reload_health.health != DCC_HOT_RELOAD_HEALTH_STOPPED ||
        reload_health.backend != DCC_HOT_RELOAD_BACKEND_IN_PROCESS ||
        reload_health.ok != 0U ||
        reload_health.loaded != 1U ||
        reload_health.running != 0U ||
        reload_health.active_worker_loaded != 0U ||
        reload_health.generation != 1U ||
        reload_health.last_status != DCC_OK ||
        strcmp(dcc_hot_reload_health_string(DCC_HOT_RELOAD_HEALTH_STOPPED), "stopped") != 0 ||
        strcmp(dcc_hot_reload_health_string((dcc_hot_reload_health_t)99), "unknown") != 0 ||
        dcc_hot_reload_health_snapshot_json(NULL, reload_health_json, sizeof(reload_health_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_health_snapshot_json(&bad_reload_health, reload_health_json, sizeof(reload_health_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_health_snapshot_json(&reload_health, NULL, sizeof(reload_health_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_health_snapshot_json(
            &reload_health,
            reload_health_json,
            sizeof(reload_health_json),
            &reload_health_json_len
        ) != DCC_OK ||
        reload_health_json_len == 0U ||
        strstr(reload_health_json, "\"backend\":\"in_process\"") == NULL ||
        strstr(reload_health_json, "\"health\":\"stopped\"") == NULL ||
        strstr(reload_health_json, "\"loaded\":true") == NULL) {
        fprintf(stderr, "hot reload stopped health snapshot failed: %s\n", reload_health_json);
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    if (dcc_hot_reload_smoke_dispatch_ready(client, "v1") != 0 ||
        atomic_load_explicit(&state.ready_v1, memory_order_acquire) != 1U ||
        atomic_load_explicit(&state.ready_v2, memory_order_acquire) != 0U ||
        atomic_load_explicit(&state.shared_ready_after_v1, memory_order_acquire) != 1U) {
        fprintf(stderr, "v1 handler dispatch failed\n");
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    usleep(20000U);
    if (dcc_hot_reload_smoke_copy_file(DCC_HOT_RELOAD_MODULE_BAD, path) != 0) {
        fprintf(stderr, "failed to copy bad module\n");
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    status = dcc_hot_reload_poll(hot_reload);
    if (status == DCC_OK ||
        dcc_hot_reload_generation(hot_reload) != 1 ||
        atomic_load_explicit(&state.deinit_v1, memory_order_acquire) != 0U) {
        fprintf(stderr,
                "bad hot reload did not preserve v1: status=%s gen=%llu deinit1=%u err=%s\n",
                dcc_status_string(status),
                (unsigned long long)dcc_hot_reload_generation(hot_reload),
                atomic_load_explicit(&state.deinit_v1, memory_order_acquire),
                dcc_hot_reload_last_error(hot_reload));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }
    reload_health.size = sizeof(reload_health);
    if (dcc_hot_reload_health_snapshot(hot_reload, &reload_health) != DCC_OK ||
        reload_health.health != DCC_HOT_RELOAD_HEALTH_DEGRADED ||
        reload_health.ok != 0U ||
        reload_health.loaded != 1U ||
        reload_health.generation != 1U ||
        reload_health.last_status == DCC_OK ||
        dcc_hot_reload_health_snapshot_json(&reload_health, reload_health_json, sizeof(reload_health_json), NULL) !=
            DCC_OK ||
        strstr(reload_health_json, "\"health\":\"degraded\"") == NULL ||
        strstr(reload_health_json, "\"loaded\":true") == NULL ||
        strstr(reload_health_json, "\"last_error\":") == NULL) {
        fprintf(stderr, "hot reload degraded health snapshot failed: %s\n", reload_health_json);
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    status = dcc_hot_reload_poll(hot_reload);
    if (status != DCC_OK ||
        dcc_hot_reload_generation(hot_reload) != 1 ||
        atomic_load_explicit(&state.deinit_v1, memory_order_acquire) != 0U) {
        fprintf(stderr,
                "bad hot reload was retried instead of being rolled back: status=%s gen=%llu deinit1=%u err=%s\n",
                dcc_status_string(status),
                (unsigned long long)dcc_hot_reload_generation(hot_reload),
                atomic_load_explicit(&state.deinit_v1, memory_order_acquire),
                dcc_hot_reload_last_error(hot_reload));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    dcc_client_t *restart_client = NULL;
    dcc_hot_reload_t *restart_reload = NULL;
    hot_reload_test_state_t restart_state;
    memset(&restart_state, 0, sizeof(restart_state));
    dcc_client_options_t restart_client_options = {
        .size = sizeof(restart_client_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_hot_reload_options_t restart_options = {
        .size = sizeof(restart_options),
        .poll_interval_ms = 20,
        .settle_interval_ms = DCC_HOT_RELOAD_NO_SETTLE,
        .user_data = &restart_state,
    };
    status = dcc_client_create(&restart_client_options, &restart_client);
    if (status == DCC_OK) {
        status = dcc_hot_reload_create(restart_client, path, &restart_options, &restart_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(restart_reload);
    }
    if (status != DCC_OK ||
        dcc_hot_reload_generation(restart_reload) != 1 ||
        atomic_load_explicit(&restart_state.init_v1, memory_order_acquire) != 1U ||
        atomic_load_explicit(&restart_state.init_v2, memory_order_acquire) != 0U) {
        fprintf(stderr,
                "rolled-back module path did not restart with v1: status=%s gen=%llu init1=%u init2=%u err=%s\n",
                dcc_status_string(status),
                (unsigned long long)dcc_hot_reload_generation(restart_reload),
                atomic_load_explicit(&restart_state.init_v1, memory_order_acquire),
                atomic_load_explicit(&restart_state.init_v2, memory_order_acquire),
                restart_reload != NULL ? dcc_hot_reload_last_error(restart_reload) : "");
        dcc_hot_reload_destroy(restart_reload);
        dcc_client_destroy(restart_client);
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }
    dcc_hot_reload_destroy(restart_reload);
    dcc_client_destroy(restart_client);

    if (dcc_hot_reload_smoke_dispatch_ready(client, "v1-after-bad") != 0 ||
        atomic_load_explicit(&state.ready_v1, memory_order_acquire) != 2U ||
        atomic_load_explicit(&state.ready_v2, memory_order_acquire) != 0U ||
        atomic_load_explicit(&state.shared_ready_after_v1, memory_order_acquire) != 2U) {
        fprintf(stderr,
                "v1 handler did not survive bad reload: ready1=%u ready2=%u shared=%u\n",
                atomic_load_explicit(&state.ready_v1, memory_order_acquire),
                atomic_load_explicit(&state.ready_v2, memory_order_acquire),
                atomic_load_explicit(&state.shared_ready_after_v1, memory_order_acquire));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }
    usleep(20000U);
    if (dcc_hot_reload_smoke_copy_file(DCC_HOT_RELOAD_MODULE_V2, path) != 0) {
        fprintf(stderr, "failed to copy v2 module\n");
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    status = dcc_hot_reload_poll(hot_reload);
    if (status != DCC_OK) {
        fprintf(stderr, "hot reload poll failed: %s %s\n", dcc_status_string(status), dcc_hot_reload_last_error(hot_reload));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }
    if (dcc_hot_reload_generation(hot_reload) == 1) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status != DCC_OK ||
        dcc_hot_reload_generation(hot_reload) != 2 ||
        atomic_load_explicit(&state.init_v2, memory_order_acquire) != 1U ||
        atomic_load_explicit(&state.deinit_v1, memory_order_acquire) != 1U ||
        atomic_load_explicit(&state.shared_seen_v2, memory_order_acquire) != 2U) {
        fprintf(stderr,
                "second hot reload failed: status=%s gen=%llu init2=%u deinit1=%u shared=%u err=%s\n",
                dcc_status_string(status),
                (unsigned long long)dcc_hot_reload_generation(hot_reload),
                atomic_load_explicit(&state.init_v2, memory_order_acquire),
                atomic_load_explicit(&state.deinit_v1, memory_order_acquire),
                atomic_load_explicit(&state.shared_seen_v2, memory_order_acquire),
                dcc_hot_reload_last_error(hot_reload));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }
    reload_health.size = sizeof(reload_health);
    if (dcc_hot_reload_health_snapshot(hot_reload, &reload_health) != DCC_OK ||
        reload_health.health != DCC_HOT_RELOAD_HEALTH_STOPPED ||
        reload_health.loaded != 1U ||
        reload_health.generation != 2U ||
        reload_health.last_status != DCC_OK) {
        fprintf(stderr,
                "hot reload v2 stopped health snapshot failed: health=%s gen=%llu status=%s\n",
                dcc_hot_reload_health_string(reload_health.health),
                (unsigned long long)reload_health.generation,
                dcc_status_string(reload_health.last_status));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    if (dcc_hot_reload_smoke_dispatch_ready(client, "v2") != 0 ||
        atomic_load_explicit(&state.ready_v1, memory_order_acquire) != 2U ||
        atomic_load_explicit(&state.ready_v2, memory_order_acquire) != 1U ||
        atomic_load_explicit(&state.shared_ready_after_v2, memory_order_acquire) != 3U) {
        fprintf(stderr,
                "v2 handler dispatch failed: ready1=%u ready2=%u shared=%u\n",
                atomic_load_explicit(&state.ready_v1, memory_order_acquire),
                atomic_load_explicit(&state.ready_v2, memory_order_acquire),
                atomic_load_explicit(&state.shared_ready_after_v2, memory_order_acquire));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    dcc_hot_reload_destroy(hot_reload);
    if (atomic_load_explicit(&state.deinit_v2, memory_order_acquire) != 1U) {
        fprintf(stderr, "v2 deinit did not run\n");
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    dcc_client_destroy(client);
    unlink(path);
    if (dcc_hot_reload_smoke_settle_poll() != 0) {
        return 1;
    }
    return dcc_hot_reload_smoke_watcher();
}

#endif
