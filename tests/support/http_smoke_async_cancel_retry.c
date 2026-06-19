#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_async_active_cancel_smoke(void) {
    http_server_t server;
    pthread_t server_thread;
    if (start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "failed to start async active cancel server: %s\n", strerror(errno));
        return 1;
    }
    server.delay_ms = 300;

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 1,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        close(server.fd);
        return 1;
    }
    st = dcc_client_start(client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_start failed: %s\n", dcc_status_string(st));
        close(server.fd);
        dcc_client_destroy(client);
        return 1;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u/async-active-cancel", (unsigned)server.port);

    rest_async_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_request_async(client, "GET", url, NULL, async_record_cb, &seen);
    if (st != DCC_OK) {
        fprintf(stderr, "active cancel enqueue failed: %s\n", dcc_status_string(st));
        close(server.fd);
        dcc_client_destroy(client);
        return 1;
    }

    client_wait_thread_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_STATE,
    };
    pthread_t wait_thread;
    if (pthread_create(&wait_thread, NULL, client_wait_thread_main, &wait_state) != 0) {
        fprintf(stderr, "failed to start active cancel wait thread: %s\n", strerror(errno));
        close(server.fd);
        dcc_client_destroy(client);
        return 1;
    }

    uint64_t started_ms = test_now_ms();
    while (atomic_load_explicit(&server.requests_seen, memory_order_acquire) == 0U &&
           test_now_ms() - started_ms < 1000U) {
        usleep(1000);
    }

    size_t canceled = 0;
    if (atomic_load_explicit(&server.requests_seen, memory_order_acquire) == 0U) {
        fprintf(stderr, "active cancel server did not receive request\n");
        st = DCC_ERR_TIMEOUT;
    } else {
        st = dcc_rest_async_cancel_active(client, &canceled);
    }

    dcc_rest_async_status_t status;
    memset(&status, 0, sizeof(status));
    status.size = sizeof(status);
    if (st == DCC_OK) {
        st = dcc_rest_async_status(client, &status);
    }
    if (st == DCC_OK && canceled != 1) {
        fprintf(stderr, "unexpected active cancel count: %zu\n", canceled);
        st = DCC_ERR_STATE;
    }
    if (st == DCC_OK && status.active_cancel_requested != 1 && status.active != 0) {
        fprintf(
            stderr,
            "unexpected active cancel status: active=%zu canceled_active=%zu\n",
            status.active,
            status.active_cancel_requested
        );
        st = DCC_ERR_STATE;
    }
    if (st == DCC_OK) {
        st = dcc_rest_async_wait(client, 2000);
    }

    (void)dcc_client_stop(client);
    (void)pthread_join(wait_thread, NULL);
    (void)pthread_join(server_thread, NULL);
    close(server.fd);
    dcc_client_destroy(client);

    if (st != DCC_OK ||
        wait_state.status != DCC_OK ||
        seen.called != 1 ||
        seen.statuses[0] != 0 ||
        seen.errors[0] != DCC_ERR_CANCELED ||
        server.request_count != 1 ||
        strcmp(server.path, "/async-active-cancel") != 0) {
        fprintf(
            stderr,
            "unexpected active cancel result: st=%s wait=%s called=%d status=%u err=%s count=%u path=%s\n",
            dcc_status_string(st),
            dcc_status_string(wait_state.status),
            seen.called,
            seen.statuses[0],
            dcc_status_string(seen.errors[0]),
            server.request_count,
            server.path
        );
        return 1;
    }
    return 0;
}

int run_public_rest_async_retry_queue_smoke(void) {
    http_server_t retry;
    http_server_t bypass;
    pthread_t retry_thread;
    pthread_t bypass_thread;
    if (start_async_retry_queue_server(&retry, &retry_thread) != 0) {
        fprintf(stderr, "failed to start async retry queue server: %s\n", strerror(errno));
        return 1;
    }
    if (start_server(&bypass, &bypass_thread) != 0) {
        fprintf(stderr, "failed to start async retry bypass server: %s\n", strerror(errno));
        close(retry.fd);
        (void)pthread_join(retry_thread, NULL);
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 1,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        close(retry.fd);
        close(bypass.fd);
        (void)pthread_join(retry_thread, NULL);
        (void)pthread_join(bypass_thread, NULL);
        return 1;
    }
    st = dcc_client_start(client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_start failed: %s\n", dcc_status_string(st));
        close(retry.fd);
        close(bypass.fd);
        dcc_client_destroy(client);
        (void)pthread_join(retry_thread, NULL);
        (void)pthread_join(bypass_thread, NULL);
        return 1;
    }

    char retry_url[160];
    char bypass_url[160];
    snprintf(retry_url, sizeof(retry_url), "http://127.0.0.1:%u/channels/777/messages/200", (unsigned)retry.port);
    snprintf(bypass_url, sizeof(bypass_url), "http://127.0.0.1:%u/users/@me", (unsigned)bypass.port);

    rest_async_order_state_t state;
    memset(&state, 0, sizeof(state));
    state.expected = 2;
    rest_async_order_item_t retry_item = { .state = &state, .label = 1 };
    rest_async_order_item_t bypass_item = { .state = &state, .label = 2 };

    st = dcc_rest_request_async_priority(
        client,
        "GET",
        retry_url,
        NULL,
        DCC_REST_PRIORITY_NORMAL,
        async_order_cb,
        &retry_item
    );

    client_wait_thread_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_STATE,
    };
    pthread_t wait_thread;
    int wait_thread_started = 0;
    if (st == DCC_OK) {
        if (pthread_create(&wait_thread, NULL, client_wait_thread_main, &wait_state) != 0) {
            fprintf(stderr, "failed to start async retry queue wait thread: %s\n", strerror(errno));
            st = DCC_ERR_RUNTIME;
        } else {
            wait_thread_started = 1;
        }
    }

    dcc_rest_async_status_t retry_status;
    memset(&retry_status, 0, sizeof(retry_status));
    retry_status.size = sizeof(retry_status);
    int retry_queued = 0;
    uint64_t started_ms = test_now_ms();
    while (st == DCC_OK && test_now_ms() - started_ms < 1000U) {
        memset(&retry_status, 0, sizeof(retry_status));
        retry_status.size = sizeof(retry_status);
        st = dcc_rest_async_status(client, &retry_status);
        if (st != DCC_OK) {
            break;
        }
        if (atomic_load_explicit(&retry.requests_seen, memory_order_acquire) >= 1U &&
            retry_status.pending == 1 &&
            retry_status.active == 0 &&
            retry_status.pending_retry == 1 &&
            retry_status.pending_ready == 0 &&
            retry_status.pending_blocked_by_rate_limit == 1 &&
            retry_status.next_retry_wait_ms != 0 &&
            retry_status.active_capacity == 1) {
            retry_queued = 1;
            break;
        }
        usleep(1000);
    }
    if (st == DCC_OK && !retry_queued) {
        fprintf(
            stderr,
            "async retry was not queued: pending=%zu active=%zu retry=%zu ready=%zu rate_blocked=%zu next=%llu capacity=%zu seen=%u\n",
            retry_status.pending,
            retry_status.active,
            retry_status.pending_retry,
            retry_status.pending_ready,
            retry_status.pending_blocked_by_rate_limit,
            (unsigned long long)retry_status.next_retry_wait_ms,
            retry_status.active_capacity,
            atomic_load_explicit(&retry.requests_seen, memory_order_acquire)
        );
        st = DCC_ERR_TIMEOUT;
    }

    if (st == DCC_OK) {
        st = dcc_rest_request_async_priority(
            client,
            "GET",
            bypass_url,
            NULL,
            DCC_REST_PRIORITY_NORMAL,
            async_order_cb,
            &bypass_item
        );
    }

    int bypass_seen_before_retry = 0;
    started_ms = test_now_ms();
    while (st == DCC_OK && test_now_ms() - started_ms < 250U) {
        if (atomic_load_explicit(&bypass.requests_seen, memory_order_acquire) >= 1U) {
            bypass_seen_before_retry =
                atomic_load_explicit(&retry.requests_seen, memory_order_acquire) == 1U;
            break;
        }
        usleep(1000);
    }
    if (st == DCC_OK && !bypass_seen_before_retry) {
        fprintf(
            stderr,
            "async retry queue did not release active capacity: retry_seen=%u bypass_seen=%u\n",
            atomic_load_explicit(&retry.requests_seen, memory_order_acquire),
            atomic_load_explicit(&bypass.requests_seen, memory_order_acquire)
        );
        st = DCC_ERR_TIMEOUT;
    }

    int fds_closed = 0;
    if (st != DCC_OK) {
        (void)dcc_client_stop(client);
        close(retry.fd);
        close(bypass.fd);
        fds_closed = 1;
    }
    if (wait_thread_started) {
        (void)pthread_join(wait_thread, NULL);
    }
    (void)pthread_join(retry_thread, NULL);
    (void)pthread_join(bypass_thread, NULL);
    if (!fds_closed) {
        close(retry.fd);
        close(bypass.fd);
    }
    dcc_client_destroy(client);

    if (st != DCC_OK ||
        wait_state.status != DCC_OK ||
        state.called != 2 ||
        state.labels[0] != 2 ||
        state.labels[1] != 1 ||
        state.statuses[0] != 200 ||
        state.statuses[1] != 200 ||
        state.errors[0] != DCC_OK ||
        state.errors[1] != DCC_OK ||
        retry.request_count != 2U ||
        bypass.request_count != 1U ||
        strcmp(retry.paths[0], "/channels/777/messages/200") != 0 ||
        strcmp(retry.paths[1], "/channels/777/messages/200") != 0 ||
        strcmp(bypass.path, "/users/@me") != 0) {
        fprintf(
            stderr,
            "unexpected async retry queue result: st=%s wait=%s called=%d labels=%d,%d statuses=%u,%u "
            "errors=%s,%s retry_count=%u bypass_count=%u retry_paths=%s,%s bypass=%s\n",
            dcc_status_string(st),
            dcc_status_string(wait_state.status),
            state.called,
            state.labels[0],
            state.labels[1],
            state.statuses[0],
            state.statuses[1],
            dcc_status_string(state.errors[0]),
            dcc_status_string(state.errors[1]),
            retry.request_count,
            bypass.request_count,
            retry.paths[0],
            retry.paths[1],
            bypass.path
        );
        return 1;
    }

    return 0;
}


#endif
