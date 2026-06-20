#include <dcc/dcc.h>
#include <dcc/hot_reload.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "support/hot_reload_isolated_support.h"

static dcc_status_t wait_for_last_good_promotion(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_health_snapshot_t *out
) {
    for (size_t i = 0; i < 60U; ++i) {
        dcc_status_t status = dcc_hot_reload_health_snapshot(hot_reload, out);
        if (status != DCC_OK) {
            return status;
        }
        if (out->active_worker_loaded != 0U &&
            out->active_worker_generation == 1U &&
            out->last_good_worker_loaded == 0U &&
            out->worker_active_failures != 0U &&
            out->worker_last_good_promotions != 0U &&
            strstr(out->last_error, "restored last-good worker") != NULL) {
            return DCC_OK;
        }
        usleep(50000U);
    }
    return DCC_ERR_TIMEOUT;
}

static dcc_status_t isolated_health_failure_smoke(const char *marker, const char *label) {
    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-isolated-%s-health-%ld.txt", label, (long)getpid());
    if (n <= 0 || (size_t)n >= sizeof(module_path) ||
        dcc_hot_reload_test_write_marker_file(module_path, "good") != 0) {
        return DCC_ERR_RUNTIME;
    }

    dcc_client_options_t client_options = { .size = sizeof(client_options), .token = "" };
    dcc_client_t *client = NULL;
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_test_wait_state_t wait_state = {
        .client = NULL,
        .status = DCC_ERR_RUNTIME,
    };
    pthread_t wait_thread;
    int wait_thread_started = 0;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = DCC_HOT_RELOAD_INVALID_READY_WORKER,
        .worker_health_timeout_ms = 750,
        .worker_drain_timeout_ms = 25,
    };
    dcc_hot_reload_health_snapshot_t before = {
        .size = sizeof(before),
    };
    dcc_hot_reload_health_snapshot_t candidate = {
        .size = sizeof(candidate),
    };
    dcc_hot_reload_health_snapshot_t promoted = {
        .size = sizeof(promoted),
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
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &before);
    }
    if (status == DCC_OK &&
        (before.active_worker_loaded == 0U ||
         before.active_worker_generation != 1U ||
         before.last_good_worker_loaded != 0U)) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK && dcc_hot_reload_test_write_marker_file(module_path, marker) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &candidate);
    }
    if (status == DCC_OK &&
        (candidate.active_worker_loaded == 0U ||
         candidate.active_worker_generation != 2U ||
         candidate.last_good_worker_loaded == 0U ||
         candidate.last_good_worker_generation != 1U)) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_start(hot_reload);
    }
    if (status == DCC_OK) {
        status = wait_for_last_good_promotion(hot_reload, &promoted);
    }
    if (status == DCC_OK &&
        (promoted.worker_dispatch_failures != 0U ||
         promoted.worker_temp_error_responses != 0U)) {
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
            "isolated %s health failure failed: %s before=%llu/%llu candidate=%llu/%llu/%llu "
            "promoted=%llu/%llu/%llu/%llu err=%s errno=%d\n",
            label,
            dcc_status_string(status),
            (unsigned long long)before.active_worker_generation,
            (unsigned long long)before.last_good_worker_generation,
            (unsigned long long)candidate.active_worker_generation,
            (unsigned long long)candidate.last_good_worker_generation,
            (unsigned long long)candidate.worker_active_failures,
            (unsigned long long)promoted.active_worker_generation,
            (unsigned long long)promoted.last_good_worker_generation,
            (unsigned long long)promoted.worker_active_failures,
            (unsigned long long)promoted.worker_last_good_promotions,
            promoted.last_error,
            errno
        );
    }
    return status;
}

int main(void) {
    dcc_status_t malformed_status =
        isolated_health_failure_smoke("malformed-health", "malformed");
    if (malformed_status != DCC_OK) {
        return 1;
    }

    dcc_status_t hung_status = isolated_health_failure_smoke("hung-health", "hung");
    if (hung_status != DCC_OK) {
        return 1;
    }

    return 0;
}
#endif
