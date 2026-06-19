#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_bucket_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    http_server_t server;
    pthread_t thread;
    rest_seen_t seen;
    memset(&seen, 0, sizeof(seen));

    if (start_bucket_empty_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start bucket empty server: %s\n", strerror(errno));
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    st = dcc_rest_get_message(client, 777, 100, rest_cb, &seen);
    (void)pthread_join(thread, NULL);
    close(server.fd);
    if (st != DCC_OK ||
        !seen.called ||
        seen.status != 200 ||
        strcmp(server.path, "/channels/777/messages/100") != 0) {
        fprintf(stderr, "bucket primer request failed: st=%s called=%d status=%u path=%s\n",
                dcc_status_string(st),
                seen.called,
                seen.status,
                server.path);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    dcc_rest_async_status_t bucket_status;
    memset(&bucket_status, 0, sizeof(bucket_status));
    bucket_status.size = sizeof(bucket_status);
    st = dcc_rest_async_status(client, &bucket_status);
    if (st != DCC_OK ||
        bucket_status.rate_limit_buckets != 1 ||
        bucket_status.rate_limit_blocked_buckets != 1 ||
        bucket_status.next_bucket_wait_ms == 0 ||
        bucket_status.max_bucket_wait_ms == 0 ||
        bucket_status.global_wait_ms != 0) {
        fprintf(stderr,
                "bucket status failed: st=%s buckets=%zu blocked=%zu next=%llu max=%llu global=%llu\n",
                dcc_status_string(st),
                bucket_status.rate_limit_buckets,
                bucket_status.rate_limit_blocked_buckets,
                (unsigned long long)bucket_status.next_bucket_wait_ms,
                (unsigned long long)bucket_status.max_bucket_wait_ms,
                (unsigned long long)bucket_status.global_wait_ms);
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start bucket wait server: %s\n", strerror(errno));
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    set_api_base_for_server(&server);
    memset(&seen, 0, sizeof(seen));
    uint64_t started_ms = test_now_ms();
    st = dcc_rest_get_message(client, 777, 101, rest_cb, &seen);
    uint64_t elapsed_ms = test_now_ms() - started_ms;
    (void)pthread_join(thread, NULL);
    close(server.fd);

    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);

    if (st != DCC_OK ||
        !seen.called ||
        seen.status != 200 ||
        strcmp(server.path, "/channels/777/messages/101") != 0 ||
        elapsed_ms < 10U) {
        fprintf(stderr, "bucket wait request failed: st=%s called=%d status=%u path=%s elapsed=%llu\n",
                dcc_status_string(st),
                seen.called,
                seen.status,
                server.path,
                (unsigned long long)elapsed_ms);
        return 1;
    }
    return 0;
}
#endif
