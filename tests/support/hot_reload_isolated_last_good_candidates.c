#include "hot_reload_isolated_support.h"

#if !defined(_WIN32)

#include <dcc/hot_reload.h>

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static dcc_status_t wait_for_idle_promotion(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_health_snapshot_t *out
) {
    for (size_t i = 0; i < 120U; ++i) {
        dcc_status_t status = dcc_hot_reload_health_snapshot(hot_reload, out);
        if (status != DCC_OK) {
            return status;
        }
        if (out->active_worker_loaded != 0U &&
            out->active_worker_generation == 1U &&
            out->last_good_worker_loaded == 0U &&
            out->worker_active_failures != 0U &&
            out->worker_last_good_promotions != 0U) {
            return DCC_OK;
        }
        usleep(50000U);
    }
    return DCC_ERR_TIMEOUT;
}

dcc_status_t dcc_hot_reload_isolated_idle_promotion_smoke(void) {
    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-isolated-idle-promotion-%ld.so", (long)getpid());
    if (n <= 0 || (size_t)n >= sizeof(module_path) ||
        dcc_hot_reload_test_copy_file(DCC_HOT_RELOAD_ISOLATED_MODULE, module_path) != 0) {
        return DCC_ERR_RUNTIME;
    }

    dcc_client_options_t client_options = { .size = sizeof(client_options), .token = "" };
    dcc_client_t *client = NULL;
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = DCC_HOT_RELOAD_WORKER,
        .poll_interval_ms = 5000,
        .worker_health_timeout_ms = 1000,
        .worker_drain_timeout_ms = 50,
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
    if (status == DCC_OK && dcc_hot_reload_test_copy_file(DCC_HOT_RELOAD_ISOLATED_CRASH_MODULE, module_path) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &before);
    }
    if (status == DCC_OK &&
        (before.active_worker_loaded == 0U ||
         before.active_worker_generation != 2U ||
         before.active_worker_pid == 0U ||
         before.last_good_worker_loaded == 0U ||
         before.last_good_worker_generation != 1U)) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_start(hot_reload);
    }
    if (status == DCC_OK && kill((pid_t)before.active_worker_pid, SIGKILL) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = wait_for_idle_promotion(hot_reload, &after);
    }
    if (status == DCC_OK &&
        strstr(dcc_hot_reload_last_error(hot_reload), "restored last-good worker") == NULL) {
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
            "isolated idle promotion failed: %s before active=%llu/%llu last_good=%llu/%llu "
            "after active=%llu/%llu last_good=%llu/%llu counters=%llu/%llu\n",
            dcc_status_string(status),
            (unsigned long long)before.active_worker_generation,
            (unsigned long long)before.active_worker_pid,
            (unsigned long long)before.last_good_worker_generation,
            (unsigned long long)before.last_good_worker_pid,
            (unsigned long long)after.active_worker_generation,
            (unsigned long long)after.active_worker_pid,
            (unsigned long long)after.last_good_worker_generation,
            (unsigned long long)after.last_good_worker_pid,
            (unsigned long long)after.worker_active_failures,
            (unsigned long long)after.worker_last_good_promotions
        );
    }
    return status;
}

#endif
