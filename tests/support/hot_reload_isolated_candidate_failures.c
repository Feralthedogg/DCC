#include "hot_reload_isolated_support.h"

#if !defined(_WIN32)

#include <dcc/hot_reload.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

dcc_status_t dcc_hot_reload_isolated_bad_candidate_smoke(void) {
    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-isolated-bad-candidate-%ld.so", (long)getpid());
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
        .worker_health_timeout_ms = 500,
        .worker_drain_timeout_ms = 50,
    };
    dcc_hot_reload_health_snapshot_t before = {
        .size = sizeof(before),
    };
    dcc_hot_reload_health_snapshot_t after = {
        .size = sizeof(after),
    };

    dcc_status_t status = dcc_client_create(&client_options, &client);
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
         before.active_worker_pid == 0U ||
         before.last_good_worker_loaded != 0U)) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK && dcc_hot_reload_test_copy_file(DCC_HOT_RELOAD_ISOLATED_BAD_MODULE, module_path) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        dcc_status_t reload_status = dcc_hot_reload_reload(hot_reload);
        if (reload_status == DCC_OK) {
            status = DCC_ERR_RUNTIME;
        }
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &after);
    }
    if (status == DCC_OK &&
        (after.active_worker_loaded == 0U ||
         after.active_worker_generation != before.active_worker_generation ||
         after.active_worker_pid != before.active_worker_pid ||
         after.last_good_worker_loaded != 0U ||
         after.generation != before.generation ||
         strstr(dcc_hot_reload_last_error(hot_reload), "module init failed") == NULL)) {
        status = DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_destroy(hot_reload);
    dcc_client_destroy(client);
    unlink(module_path);

    if (status != DCC_OK) {
        fprintf(
            stderr,
            "isolated bad candidate failed: %s before active=%llu/%llu gen=%llu "
            "after active=%llu/%llu gen=%llu last_good=%u/%llu err=%s\n",
            dcc_status_string(status),
            (unsigned long long)before.active_worker_generation,
            (unsigned long long)before.active_worker_pid,
            (unsigned long long)before.generation,
            (unsigned long long)after.active_worker_generation,
            (unsigned long long)after.active_worker_pid,
            (unsigned long long)after.generation,
            (unsigned)after.last_good_worker_loaded,
            (unsigned long long)after.last_good_worker_generation,
            after.last_error
        );
    }
    return status;
}

dcc_status_t dcc_hot_reload_isolated_timeout_candidate_smoke(void) {
    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-isolated-timeout-candidate-%ld.so", (long)getpid());
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
        .worker_health_timeout_ms = 1000,
        .worker_drain_timeout_ms = 50,
    };
    dcc_hot_reload_health_snapshot_t before = {
        .size = sizeof(before),
    };
    dcc_hot_reload_health_snapshot_t after = {
        .size = sizeof(after),
    };

    dcc_status_t status = dcc_client_create(&client_options, &client);
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
         before.active_worker_pid == 0U ||
         before.last_good_worker_loaded != 0U)) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK && dcc_hot_reload_test_copy_file(DCC_HOT_RELOAD_ISOLATED_HANG_MODULE, module_path) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        dcc_status_t reload_status = dcc_hot_reload_reload(hot_reload);
        if (reload_status != DCC_ERR_TIMEOUT) {
            status = DCC_ERR_RUNTIME;
        }
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &after);
    }
    if (status == DCC_OK &&
        (after.active_worker_loaded == 0U ||
         after.active_worker_generation != before.active_worker_generation ||
         after.active_worker_pid != before.active_worker_pid ||
         after.last_good_worker_loaded != 0U ||
         after.generation != before.generation ||
         after.last_status != DCC_ERR_TIMEOUT ||
         strstr(dcc_hot_reload_last_error(hot_reload), "timeout") == NULL)) {
        status = DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_destroy(hot_reload);
    dcc_client_destroy(client);
    unlink(module_path);

    if (status != DCC_OK) {
        fprintf(
            stderr,
            "isolated timeout candidate failed: %s before active=%llu/%llu gen=%llu "
            "after active=%llu/%llu gen=%llu last_good=%u/%llu last_status=%s err=%s\n",
            dcc_status_string(status),
            (unsigned long long)before.active_worker_generation,
            (unsigned long long)before.active_worker_pid,
            (unsigned long long)before.generation,
            (unsigned long long)after.active_worker_generation,
            (unsigned long long)after.active_worker_pid,
            (unsigned long long)after.generation,
            (unsigned)after.last_good_worker_loaded,
            (unsigned long long)after.last_good_worker_generation,
            dcc_status_string(after.last_status),
            after.last_error
        );
    }
    return status;
}

dcc_status_t dcc_hot_reload_isolated_exit_candidate_smoke(void) {
    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-isolated-exit-candidate-%ld.so", (long)getpid());
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
        .worker_health_timeout_ms = 500,
        .worker_drain_timeout_ms = 25,
    };
    dcc_hot_reload_health_snapshot_t before = {
        .size = sizeof(before),
    };
    dcc_hot_reload_health_snapshot_t after = {
        .size = sizeof(after),
    };

    dcc_status_t status = dcc_client_create(&client_options, &client);
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
         before.active_worker_pid == 0U ||
         before.last_good_worker_loaded != 0U)) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK && dcc_hot_reload_test_copy_file(DCC_HOT_RELOAD_ISOLATED_EXIT_MODULE, module_path) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        dcc_status_t reload_status = dcc_hot_reload_reload(hot_reload);
        if (reload_status != DCC_ERR_RUNTIME) {
            status = DCC_ERR_RUNTIME;
        }
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &after);
    }
    if (status == DCC_OK &&
        (after.active_worker_loaded == 0U ||
         after.active_worker_generation != before.active_worker_generation ||
         after.active_worker_pid != before.active_worker_pid ||
         after.last_good_worker_loaded != 0U ||
         after.generation != before.generation ||
         after.last_status != DCC_ERR_RUNTIME ||
         strstr(dcc_hot_reload_last_error(hot_reload), "exited before READY") == NULL)) {
        status = DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_destroy(hot_reload);
    dcc_client_destroy(client);
    unlink(module_path);

    if (status != DCC_OK) {
        fprintf(
            stderr,
            "isolated exit candidate failed: %s before active=%llu/%llu gen=%llu "
            "after active=%llu/%llu gen=%llu last_good=%u/%llu last_status=%s err=%s\n",
            dcc_status_string(status),
            (unsigned long long)before.active_worker_generation,
            (unsigned long long)before.active_worker_pid,
            (unsigned long long)before.generation,
            (unsigned long long)after.active_worker_generation,
            (unsigned long long)after.active_worker_pid,
            (unsigned long long)after.generation,
            (unsigned)after.last_good_worker_loaded,
            (unsigned long long)after.last_good_worker_generation,
            dcc_status_string(after.last_status),
            after.last_error
        );
    }
    return status;
}

dcc_status_t dcc_hot_reload_isolated_invalid_ready_worker_smoke(void) {
    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-isolated-invalid-ready-%ld.txt", (long)getpid());
    if (n <= 0 || (size_t)n >= sizeof(module_path) ||
        dcc_hot_reload_test_write_marker_file(module_path, "good") != 0) {
        return DCC_ERR_RUNTIME;
    }

    dcc_client_options_t client_options = { .size = sizeof(client_options), .token = "" };
    dcc_client_t *client = NULL;
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = DCC_HOT_RELOAD_INVALID_READY_WORKER,
        .worker_health_timeout_ms = 500,
        .worker_drain_timeout_ms = 25,
    };
    dcc_hot_reload_health_snapshot_t before = {
        .size = sizeof(before),
    };
    dcc_hot_reload_health_snapshot_t after = {
        .size = sizeof(after),
    };
    const char *last_error = NULL;

    dcc_status_t status = dcc_client_create(&client_options, &client);
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
         before.active_worker_pid == 0U ||
         before.last_good_worker_loaded != 0U)) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK && dcc_hot_reload_test_write_marker_file(module_path, "invalid") != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        dcc_status_t reload_status = dcc_hot_reload_reload(hot_reload);
        if (reload_status != DCC_ERR_RUNTIME) {
            status = DCC_ERR_RUNTIME;
        }
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &after);
    }
    last_error = dcc_hot_reload_last_error(hot_reload);
    if (status == DCC_OK &&
        (after.active_worker_loaded == 0U ||
         after.active_worker_generation != before.active_worker_generation ||
         after.active_worker_pid != before.active_worker_pid ||
         after.last_good_worker_loaded != 0U ||
         after.generation != before.generation ||
         after.last_status != DCC_ERR_RUNTIME ||
         (strstr(last_error, "invalid READY frame") == NULL &&
          strstr(last_error, "exited before READY") == NULL))) {
        status = DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_destroy(hot_reload);
    dcc_client_destroy(client);
    unlink(module_path);

    if (status != DCC_OK) {
        fprintf(
            stderr,
            "isolated invalid READY worker failed: %s before active=%llu/%llu gen=%llu "
            "after active=%llu/%llu gen=%llu last_good=%u/%llu last_status=%s err=%s\n",
            dcc_status_string(status),
            (unsigned long long)before.active_worker_generation,
            (unsigned long long)before.active_worker_pid,
            (unsigned long long)before.generation,
            (unsigned long long)after.active_worker_generation,
            (unsigned long long)after.active_worker_pid,
            (unsigned long long)after.generation,
            (unsigned)after.last_good_worker_loaded,
            (unsigned long long)after.last_good_worker_generation,
            dcc_status_string(after.last_status),
            after.last_error
        );
    }
    return status;
}

#endif
