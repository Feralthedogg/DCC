#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <dcc/dcc.h>
#include <dcc/hot_reload.h>

#include "support/hot_reload_isolated_support.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static dcc_status_t wait_for_canary_promotion(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_health_snapshot_t *out
) {
    for (size_t i = 0; i < 80U; ++i) {
        dcc_status_t status = dcc_hot_reload_health_snapshot(hot_reload, out);
        if (status != DCC_OK) {
            return status;
        }
        if (out->active_worker_generation == 2U &&
            out->candidate_worker_loaded == 0U &&
            out->last_good_worker_generation == 1U &&
            out->worker_canary_promotions == 1U) {
            return DCC_OK;
        }
        usleep(50000U);
    }
    return DCC_ERR_TIMEOUT;
}

static dcc_status_t canary_idle_promotion_smoke(void) {
    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-canary-promote-%ld.so", (long)getpid());
    if (n <= 0 || (size_t)n >= sizeof(module_path) ||
        dcc_hot_reload_test_copy_file(DCC_HOT_RELOAD_ISOLATED_MODULE, module_path) != 0) {
        return DCC_ERR_RUNTIME;
    }

    dcc_client_options_t client_options = { .size = sizeof(client_options), .token = "" };
    dcc_client_t *client = NULL;
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_canary_options_t canary;
    dcc_hot_reload_canary_options_init(&canary);
    canary.canary_percent = 100U;
    canary.promote_after_ms = 50U;
    canary.max_error_rate = 1.0;
    canary.max_consecutive_failures = 5U;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = DCC_HOT_RELOAD_WORKER,
        .worker_health_timeout_ms = 100U,
        .worker_drain_timeout_ms = 25U,
        .worker_canary_enabled = 1U,
        .worker_canary_options = canary,
    };
    dcc_hot_reload_test_wait_state_t wait_state = {
        .client = NULL,
        .status = DCC_ERR_RUNTIME,
    };
    pthread_t wait_thread;
    int wait_thread_started = 0;
    dcc_hot_reload_health_snapshot_t before = {
        .size = sizeof(before),
    };
    dcc_hot_reload_health_snapshot_t after = {
        .size = sizeof(after),
    };
    char json[2048] = {0};

    dcc_status_t status = dcc_client_create(&client_options, &client);
    wait_state.client = client;
    if (status == DCC_OK) {
        status = dcc_client_start(client);
    }
    if (status == DCC_OK && pthread_create(&wait_thread, NULL, dcc_hot_reload_test_wait_main, &wait_state) == 0) {
        wait_thread_started = 1;
    } else if (status == DCC_OK) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_create(client, module_path, &options, &hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &before);
    }
    if (status == DCC_OK &&
        (before.active_worker_generation != 1U ||
         before.candidate_worker_generation != 2U ||
         before.worker_canary_active == 0U ||
         before.worker_canary_percent != 100U)) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_start(hot_reload);
    }
    if (status == DCC_OK) {
        status = wait_for_canary_promotion(hot_reload, &after);
    }
    if (status == DCC_OK &&
        (dcc_hot_reload_health_snapshot_json(&after, json, sizeof(json), NULL) != DCC_OK ||
         strstr(json, "\"canary\":{\"enabled\":true,\"active\":false,\"percent\":100") == NULL ||
         strstr(json, "\"candidate_worker\":{\"loaded\":false,\"generation\":0,\"pid\":0}") == NULL)) {
        status = DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_destroy(hot_reload);
    (void)dcc_client_stop(client);
    if (wait_thread_started) {
        pthread_join(wait_thread, NULL);
        if (wait_state.status != DCC_OK && status == DCC_OK) {
            status = wait_state.status;
        }
    }
    dcc_client_destroy(client);
    unlink(module_path);

    if (status != DCC_OK) {
        fprintf(
            stderr,
            "canary idle promotion failed: %s before active=%llu candidate=%llu canary=%u "
            "after active=%llu last_good=%llu candidate=%llu promotions=%llu json=%s\n",
            dcc_status_string(status),
            (unsigned long long)before.active_worker_generation,
            (unsigned long long)before.candidate_worker_generation,
            (unsigned)before.worker_canary_active,
            (unsigned long long)after.active_worker_generation,
            (unsigned long long)after.last_good_worker_generation,
            (unsigned long long)after.candidate_worker_generation,
            (unsigned long long)after.worker_canary_promotions,
            json
        );
    }
    return status;
}

static dcc_status_t canary_dispatch_rollback_smoke(void) {
    dcc_hot_reload_test_server_t server;
    if (dcc_hot_reload_test_server_start(&server) != 0) {
        fprintf(stderr, "canary rollback server start failed: %s\n", strerror(errno));
        return DCC_ERR_RUNTIME;
    }
    server.max_requests = 2U;
    pthread_t thread;
    int thread_started = 0;
    if (pthread_create(&thread, NULL, dcc_hot_reload_test_server_main, &server) == 0) {
        thread_started = 1;
    } else {
        close(server.fd);
        return DCC_ERR_RUNTIME;
    }

    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-canary-rollback-%ld.so", (long)getpid());
    dcc_status_t status = DCC_OK;
    if (n <= 0 || (size_t)n >= sizeof(module_path) ||
        dcc_hot_reload_test_copy_file(DCC_HOT_RELOAD_ISOLATED_MODULE, module_path) != 0) {
        status = DCC_ERR_RUNTIME;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u", (unsigned)server.port);
    setenv("DCC_DISCORD_API_BASE", url, 1);

    dcc_client_options_t client_options = { .size = sizeof(client_options), .token = "" };
    dcc_client_t *client = NULL;
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_canary_options_t canary;
    dcc_hot_reload_canary_options_init(&canary);
    canary.canary_percent = 100U;
    canary.promote_after_ms = 30000U;
    canary.max_error_rate = 1.0;
    canary.max_consecutive_failures = 1U;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = DCC_HOT_RELOAD_WORKER,
        .worker_health_timeout_ms = 200U,
        .worker_drain_timeout_ms = 25U,
        .worker_canary_enabled = 1U,
        .worker_canary_options = canary,
    };
    dcc_hot_reload_health_snapshot_t after = {
        .size = sizeof(after),
    };

    if (status == DCC_OK) {
        status = dcc_client_create(&client_options, &client);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_create(client, module_path, &options, &hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK && dcc_hot_reload_test_copy_file(DCC_HOT_RELOAD_ISOLATED_CRASH_MODULE, module_path) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK && dcc_hot_reload_test_dispatch_raw_slash(client, 88U) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &after);
    }
    if (status == DCC_OK &&
        (after.active_worker_generation != 1U ||
         after.candidate_worker_loaded != 0U ||
         after.worker_canary_failures != 1U ||
         after.worker_canary_rollbacks != 1U ||
         after.worker_canary_rollback_reason != DCC_HOT_RELOAD_CANARY_ROLLBACK_CONSECUTIVE_FAILURES)) {
        status = DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_destroy(hot_reload);
    dcc_client_destroy(client);
    close(server.fd);
    if (thread_started) {
        pthread_join(thread, NULL);
    }
    unlink(module_path);
    unsetenv("DCC_DISCORD_API_BASE");

    if (status != DCC_OK ||
        server.request_count != 2U ||
        strstr(server.request[0], "POST /interactions/555/retry-token/callback HTTP/1.1") == NULL ||
        strstr(server.request[0], "\"content\":\"worker slash\"") == NULL ||
        strstr(server.request[1], "POST /channels/123/messages HTTP/1.1") == NULL ||
        strstr(server.request[1], "\"content\":\"worker async\"") == NULL) {
        fprintf(
            stderr,
            "canary dispatch rollback failed: %s count=%zu active=%llu candidate=%llu "
            "canary=%llu/%llu reason=%s\nfirst:\n%s\nsecond:\n%s\n",
            dcc_status_string(status),
            server.request_count,
            (unsigned long long)after.active_worker_generation,
            (unsigned long long)after.candidate_worker_generation,
            (unsigned long long)after.worker_canary_failures,
            (unsigned long long)after.worker_canary_rollbacks,
            dcc_hot_reload_canary_rollback_reason_string(after.worker_canary_rollback_reason),
            server.request[0],
            server.request[1]
        );
        return DCC_ERR_RUNTIME;
    }
    return status;
}

int main(void) {
    dcc_status_t status = canary_idle_promotion_smoke();
    if (status != DCC_OK) {
        return 1;
    }
    return canary_dispatch_rollback_smoke() == DCC_OK ? 0 : 1;
}

#endif
