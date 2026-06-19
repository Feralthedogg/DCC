#include "hot_reload_isolated_support.h"

#if !defined(_WIN32)

#include <dcc/hot_reload.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static dcc_status_t isolated_event_result_smoke(const char *label, const char *marker, uint64_t sequence) {
    dcc_hot_reload_test_server_t server;
    if (dcc_hot_reload_test_server_start(&server) != 0) {
        fprintf(stderr, "isolated %s event server start failed: %s\n", label, strerror(errno));
        return DCC_ERR_RUNTIME;
    }
    server.max_requests = 1U;
    pthread_t thread;
    int thread_started = 0;
    if (pthread_create(&thread, NULL, dcc_hot_reload_test_server_main, &server) == 0) {
        thread_started = 1;
    } else {
        close(server.fd);
        return DCC_ERR_RUNTIME;
    }

    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-isolated-%s-event-%ld.txt", label, (long)getpid());
    dcc_status_t status = DCC_OK;
    if (n <= 0 || (size_t)n >= sizeof(module_path) ||
        dcc_hot_reload_test_write_marker_file(module_path, "good-event") != 0) {
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
        .worker_health_timeout_ms = strcmp(marker, "hung-event") == 0 ? 100U : 500U,
        .worker_drain_timeout_ms = 25,
    };
    dcc_hot_reload_health_snapshot_t restored = {
        .size = sizeof(restored),
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
    if (status == DCC_OK && dcc_hot_reload_test_write_marker_file(module_path, marker) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK && dcc_hot_reload_test_dispatch_raw_slash(client, sequence) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK &&
        strstr(dcc_hot_reload_last_error(hot_reload), "restored last-good worker") == NULL) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &restored);
    }
    if (status == DCC_OK &&
        (restored.active_worker_generation != 1U ||
         restored.last_good_worker_loaded != 0U ||
         restored.last_good_worker_generation != 0U ||
         restored.worker_active_failures == 0U ||
         restored.worker_last_good_promotions == 0U ||
         restored.worker_dispatch_failures != 0U ||
         restored.worker_temp_error_responses != 0U)) {
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
        strstr(server.request[0], "POST /fake-worker HTTP/1.1") == NULL ||
        strstr(server.request[0], "\"content\":\"fake worker slash\"") == NULL) {
        fprintf(
            stderr,
            "isolated %s event result failed: %s count=%zu restored=%llu/%llu/%llu/%llu "
            "request:\n%s\n",
            label,
            dcc_status_string(status),
            server.request_count,
            (unsigned long long)restored.worker_active_failures,
            (unsigned long long)restored.worker_last_good_promotions,
            (unsigned long long)restored.worker_dispatch_failures,
            (unsigned long long)restored.worker_temp_error_responses,
            server.request[0]
        );
        return DCC_ERR_RUNTIME;
    }
    return status;
}

dcc_status_t dcc_hot_reload_isolated_malformed_event_result_smoke(void) {
    return isolated_event_result_smoke("malformed", "malformed-event", 44U);
}

dcc_status_t dcc_hot_reload_isolated_hung_event_result_smoke(void) {
    return isolated_event_result_smoke("hung", "hung-event", 45U);
}

#endif
