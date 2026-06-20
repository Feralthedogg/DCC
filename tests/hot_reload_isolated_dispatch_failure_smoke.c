#include <dcc/dcc.h>
#include <dcc/hot_reload.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "support/hot_reload_isolated_support.h"

static dcc_status_t dispatch_failure_smoke(const char *marker, const char *label, uint64_t sequence) {
    dcc_hot_reload_test_server_t server;
    if (dcc_hot_reload_test_server_start(&server) != 0) {
        fprintf(stderr, "%s dispatch server start failed: %s\n", label, strerror(errno));
        return DCC_ERR_RUNTIME;
    }
    pthread_t thread;
    int thread_started = 0;
    server.max_requests = 1U;
    if (pthread_create(&thread, NULL, dcc_hot_reload_test_server_main, &server) == 0) {
        thread_started = 1;
    } else {
        close(server.fd);
        return DCC_ERR_RUNTIME;
    }

    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-isolated-%s-dispatch-%ld.txt", label, (long)getpid());
    dcc_status_t status = DCC_OK;
    if (n <= 0 || (size_t)n >= sizeof(module_path) ||
        dcc_hot_reload_test_write_marker_file(module_path, marker) != 0) {
        status = DCC_ERR_RUNTIME;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u", (unsigned)server.port);
    setenv("DCC_DISCORD_API_BASE", url, 1);

    dcc_client_options_t client_options = { .size = sizeof(client_options), .token = "" };
    dcc_client_t *client = NULL;
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = DCC_HOT_RELOAD_INVALID_READY_WORKER,
        .worker_health_timeout_ms = 750,
        .worker_drain_timeout_ms = 25,
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
    if (status == DCC_OK && dcc_hot_reload_test_dispatch_raw_slash(client, sequence) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK &&
        strstr(dcc_hot_reload_last_error(hot_reload), "temporary interaction error") == NULL) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &after);
    }
    if (status == DCC_OK &&
        (after.active_worker_loaded != 0U ||
         after.last_good_worker_loaded != 0U ||
         after.worker_active_failures == 0U ||
         after.worker_last_good_promotions != 0U ||
         after.worker_dispatch_failures == 0U ||
         after.worker_temp_error_responses == 0U)) {
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
        server.request_count != 1U ||
        strstr(server.request[0], "POST /interactions/555/retry-token/callback HTTP/1.1") == NULL ||
        strstr(server.request[0], "\"content\":\"Bot worker is reloading. Please try again.\"") == NULL ||
        strstr(server.request[0], "\"flags\":64") == NULL) {
        fprintf(
            stderr,
            "isolated %s dispatch failure failed: %s count=%zu active=%u last_good=%u "
            "counters=%llu/%llu/%llu/%llu request:\n%s\n",
            label,
            dcc_status_string(status),
            server.request_count,
            (unsigned)after.active_worker_loaded,
            (unsigned)after.last_good_worker_loaded,
            (unsigned long long)after.worker_active_failures,
            (unsigned long long)after.worker_last_good_promotions,
            (unsigned long long)after.worker_dispatch_failures,
            (unsigned long long)after.worker_temp_error_responses,
            server.request[0]
        );
        return DCC_ERR_RUNTIME;
    }
    return status;
}

static dcc_status_t dispatch_last_good_failure_smoke(void) {
    dcc_hot_reload_test_server_t server;
    if (dcc_hot_reload_test_server_start(&server) != 0) {
        fprintf(stderr, "last-good dispatch server start failed: %s\n", strerror(errno));
        return DCC_ERR_RUNTIME;
    }
    pthread_t thread;
    int thread_started = 0;
    server.max_requests = 1U;
    if (pthread_create(&thread, NULL, dcc_hot_reload_test_server_main, &server) == 0) {
        thread_started = 1;
    } else {
        close(server.fd);
        return DCC_ERR_RUNTIME;
    }

    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-isolated-double-dispatch-%ld.txt", (long)getpid());
    dcc_status_t status = DCC_OK;
    if (n <= 0 || (size_t)n >= sizeof(module_path) ||
        dcc_hot_reload_test_write_marker_file(module_path, "malformed-event") != 0) {
        status = DCC_ERR_RUNTIME;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u", (unsigned)server.port);
    setenv("DCC_DISCORD_API_BASE", url, 1);

    dcc_client_options_t client_options = { .size = sizeof(client_options), .token = "" };
    dcc_client_t *client = NULL;
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = DCC_HOT_RELOAD_INVALID_READY_WORKER,
        .worker_health_timeout_ms = 750,
        .worker_drain_timeout_ms = 25,
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
    if (status == DCC_OK && dcc_hot_reload_test_write_marker_file(module_path, "hung-event") != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK && dcc_hot_reload_test_dispatch_raw_slash(client, 103U) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK &&
        strstr(dcc_hot_reload_last_error(hot_reload), "temporary interaction error") == NULL) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &after);
    }
    if (status == DCC_OK &&
        (after.active_worker_loaded != 0U ||
         after.last_good_worker_loaded != 0U ||
         after.worker_active_failures == 0U ||
         after.worker_last_good_promotions == 0U ||
         after.worker_dispatch_failures == 0U ||
         after.worker_temp_error_responses == 0U)) {
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
        server.request_count != 1U ||
        strstr(server.request[0], "POST /interactions/555/retry-token/callback HTTP/1.1") == NULL ||
        strstr(server.request[0], "\"content\":\"Bot worker is reloading. Please try again.\"") == NULL ||
        strstr(server.request[0], "\"flags\":64") == NULL) {
        fprintf(
            stderr,
            "isolated last-good dispatch failure failed: %s count=%zu active=%u last_good=%u "
            "counters=%llu/%llu/%llu/%llu request:\n%s\n",
            dcc_status_string(status),
            server.request_count,
            (unsigned)after.active_worker_loaded,
            (unsigned)after.last_good_worker_loaded,
            (unsigned long long)after.worker_active_failures,
            (unsigned long long)after.worker_last_good_promotions,
            (unsigned long long)after.worker_dispatch_failures,
            (unsigned long long)after.worker_temp_error_responses,
            server.request[0]
        );
        return DCC_ERR_RUNTIME;
    }
    return status;
}

int main(void) {
    dcc_status_t malformed_status = dispatch_failure_smoke("malformed-event", "malformed", 101U);
    if (malformed_status != DCC_OK) {
        return 1;
    }

    dcc_status_t hung_status = dispatch_failure_smoke("hung-event", "hung", 102U);
    if (hung_status != DCC_OK) {
        return 1;
    }

    dcc_status_t last_good_status = dispatch_last_good_failure_smoke();
    if (last_good_status != DCC_OK) {
        return 1;
    }
    return 0;
}
#endif
