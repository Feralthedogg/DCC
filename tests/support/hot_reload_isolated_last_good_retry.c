#include "hot_reload_isolated_support.h"

#if !defined(_WIN32)

#include <dcc/hot_reload.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int dcc_hot_reload_isolated_last_good_retry_smoke(void) {
    dcc_hot_reload_test_server_t server;
    if (dcc_hot_reload_test_server_start(&server) != 0) {
        fprintf(stderr, "isolated last-good server start failed: %s\n", strerror(errno));
        return 1;
    }
    pthread_t thread;
    if (pthread_create(&thread, NULL, dcc_hot_reload_test_server_main, &server) != 0) {
        close(server.fd);
        return 1;
    }

    char module_path[256];
    int n = snprintf(module_path, sizeof(module_path), "/tmp/dcc-isolated-last-good-%ld.so", (long)getpid());
    dcc_status_t status = DCC_OK;
    if (n <= 0 || (size_t)n >= sizeof(module_path) ||
        dcc_hot_reload_test_copy_file(DCC_HOT_RELOAD_ISOLATED_MODULE, module_path) != 0) {
        status = DCC_ERR_RUNTIME;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u", (unsigned)server.port);
    setenv("DCC_ISOLATED_TEST_URL", url, 1);
    setenv("DCC_DISCORD_API_BASE", url, 1);

    dcc_client_options_t client_options = { .size = sizeof(client_options), .token = "" };
    dcc_client_t *client = NULL;
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = DCC_HOT_RELOAD_WORKER,
        .worker_health_timeout_ms = 3000,
        .worker_drain_timeout_ms = 300,
    };
    dcc_hot_reload_health_snapshot_t restored_health = {
        .size = sizeof(restored_health),
    };
    dcc_hot_reload_health_snapshot_t fallback_health = {
        .size = sizeof(fallback_health),
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
    if (status == DCC_OK && dcc_hot_reload_test_dispatch_raw_slash(client, 8U) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK &&
        strstr(dcc_hot_reload_last_error(hot_reload), "restored last-good worker") == NULL) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &restored_health);
    }
    if (status == DCC_OK &&
        (restored_health.active_worker_generation != 1U ||
         restored_health.last_good_worker_loaded != 0U ||
         restored_health.last_good_worker_generation != 0U ||
         restored_health.worker_active_failures == 0U ||
         restored_health.worker_last_good_promotions == 0U ||
         restored_health.worker_dispatch_failures != 0U ||
         restored_health.worker_temp_error_responses != 0U)) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK && dcc_hot_reload_test_dispatch_raw_slash(client, 9U) != 0) {
        status = DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_destroy(hot_reload);
    hot_reload = NULL;

    if (status == DCC_OK && dcc_hot_reload_test_copy_file(DCC_HOT_RELOAD_ISOLATED_CRASH_MODULE, module_path) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_create(client, module_path, &options, &hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK && dcc_hot_reload_test_dispatch_raw_slash(client, 10U) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK &&
        strstr(dcc_hot_reload_last_error(hot_reload), "temporary interaction error") == NULL) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_health_snapshot(hot_reload, &fallback_health);
    }
    if (status == DCC_OK &&
        (fallback_health.worker_active_failures == 0U ||
         fallback_health.worker_last_good_promotions != 0U ||
         fallback_health.worker_dispatch_failures == 0U ||
         fallback_health.worker_temp_error_responses == 0U)) {
        status = DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_destroy(hot_reload);
    dcc_client_destroy(client);
    close(server.fd);
    pthread_join(thread, NULL);
    unlink(module_path);
    unsetenv("DCC_DISCORD_API_BASE");

    if (status != DCC_OK || server.request_count != 5U ||
        strstr(server.request[0], "POST /interactions/555/retry-token/callback HTTP/1.1") == NULL ||
        strstr(server.request[0], "\"content\":\"worker slash\"") == NULL ||
        strstr(server.request[1], "POST /channels/123/messages HTTP/1.1") == NULL ||
        strstr(server.request[1], "\"content\":\"worker async\"") == NULL ||
        strstr(server.request[2], "POST /interactions/555/retry-token/callback HTTP/1.1") == NULL ||
        strstr(server.request[2], "\"content\":\"worker slash\"") == NULL ||
        strstr(server.request[3], "POST /channels/123/messages HTTP/1.1") == NULL ||
        strstr(server.request[3], "\"content\":\"worker async\"") == NULL ||
        strstr(server.request[4], "POST /interactions/555/retry-token/callback HTTP/1.1") == NULL ||
        strstr(server.request[4], "\"content\":\"Bot worker is reloading. Please try again.\"") == NULL ||
        strstr(server.request[4], "\"flags\":64") == NULL) {
        fprintf(
            stderr,
            "isolated last-good retry failed: %s count=%zu restored=%llu/%llu/%llu/%llu "
            "fallback=%llu/%llu/%llu/%llu\n"
            "first:\n%s\nsecond:\n%s\nthird:\n%s\nfourth:\n%s\nfifth:\n%s\n",
            dcc_status_string(status),
            server.request_count,
            (unsigned long long)restored_health.worker_active_failures,
            (unsigned long long)restored_health.worker_last_good_promotions,
            (unsigned long long)restored_health.worker_dispatch_failures,
            (unsigned long long)restored_health.worker_temp_error_responses,
            (unsigned long long)fallback_health.worker_active_failures,
            (unsigned long long)fallback_health.worker_last_good_promotions,
            (unsigned long long)fallback_health.worker_dispatch_failures,
            (unsigned long long)fallback_health.worker_temp_error_responses,
            server.request[0],
            server.request[1],
            server.request[2],
            server.request[3],
            server.request[4]
        );
        return 1;
    }
    return 0;
}

#endif
