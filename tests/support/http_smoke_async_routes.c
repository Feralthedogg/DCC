#include "http_smoke_support.h"

#include "internal/rest/dcc_rest_async_drain_internal.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct spawn_admission_seen {
    unsigned called;
} spawn_admission_seen_t;

static void spawn_admission_cb(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    (void)client;
    spawn_admission_seen_t *seen = (spawn_admission_seen_t *)user_data;
    if (seen != NULL && response != NULL) {
        seen->called++;
    }
}

static void spawn_admission_error(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    (void)client;
    spawn_admission_seen_t *seen = (spawn_admission_seen_t *)user_data;
    if (seen != NULL && error != NULL) {
        seen->called++;
    }
}

static int spawn_admission_queue_equal(
    const dcc_rest_async_status_t *left,
    const dcc_rest_async_status_t *right
) {
    return left != NULL && right != NULL &&
        left->pending == right->pending &&
        left->active == right->active &&
        left->active_routes == right->active_routes &&
        left->pending_low == right->pending_low &&
        left->pending_normal == right->pending_normal &&
        left->pending_high == right->pending_high &&
        left->pending_ready == right->pending_ready &&
        left->pending_blocked_by_active_route ==
            right->pending_blocked_by_active_route &&
        left->active_capacity == right->active_capacity &&
        left->pending_blocked_by_rate_limit ==
            right->pending_blocked_by_rate_limit &&
        left->pending_retry == right->pending_retry;
}

int run_public_rest_async_spawn_admission_smoke(void) {
    http_server_t server;
    pthread_t server_thread;
    if (start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "failed to start spawn-admission server: %s\n", strerror(errno));
        return 1;
    }

    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 1U,
    };
    dcc_client_t *client = NULL;
    dcc_status_t status = dcc_client_create(&options, &client);
    if (status == DCC_OK) {
        status = dcc_client_start(client);
    }
    spawn_admission_seen_t observer_seen = {0};
    if (status == DCC_OK) {
        status = dcc_client_on_error(client, spawn_admission_error, &observer_seen);
    }

    dcc_rest_async_status_t before = {.size = sizeof(before)};
    dcc_rest_async_status_t after = {.size = sizeof(after)};
    if (status == DCC_OK) {
        status = dcc_rest_async_status(client, &before);
    }

    spawn_admission_seen_t *rejected =
        (spawn_admission_seen_t *)calloc(1U, sizeof(*rejected));
    if (rejected == NULL) {
        status = DCC_ERR_NOMEM;
    }
    char url[160];
    snprintf(
        url,
        sizeof(url),
        "http://127.0.0.1:%u/spawn-admission",
        (unsigned)server.port
    );
    dcc_status_t rejected_status = DCC_OK;
    if (status == DCC_OK) {
        dcc_rest_test_fail_next_worker_spawn(client);
        rejected_status = dcc_rest_request_async(
            client,
            "GET",
            url,
            NULL,
            spawn_admission_cb,
            rejected
        );
        status = dcc_rest_async_status(client, &after);
    }
    unsigned rejected_calls = rejected != NULL ? rejected->called : 0U;
    free(rejected);

    int rollback_ok = status == DCC_OK && rejected_status == DCC_ERR_RUNTIME &&
        rejected_calls == 0U && observer_seen.called == 0U &&
        spawn_admission_queue_equal(&before, &after);

    spawn_admission_seen_t accepted = {0};
    if (status == DCC_OK) {
        status = dcc_rest_request_async(
            client,
            "GET",
            url,
            NULL,
            spawn_admission_cb,
            &accepted
        );
    }
    client_wait_thread_state_t runner = {
        .client = client,
        .status = DCC_ERR_STATE,
    };
    pthread_t runner_thread;
    int runner_started = status == DCC_OK &&
        pthread_create(&runner_thread, NULL, client_wait_thread_main, &runner) == 0;
    if (runner_started) {
        status = dcc_rest_async_wait(client, 5000U);
        (void)dcc_client_stop(client);
        (void)pthread_join(runner_thread, NULL);
    } else {
        (void)dcc_client_stop(client);
    }
    dcc_client_destroy(client);
    (void)pthread_join(server_thread, NULL);
    close(server.fd);

    if (!rollback_ok || status != DCC_OK || !runner_started ||
        runner.status != DCC_OK || accepted.called != 1U ||
        observer_seen.called != 0U || server.request_count != 1U) {
        fprintf(
            stderr,
            "spawn admission rollback failed: rejected=%s rollback=%d accepted=%u "
            "observer=%u requests=%u wait=%s runner=%s pending=%zu active=%zu routes=%zu\n",
            dcc_status_string(rejected_status),
            rollback_ok,
            accepted.called,
            observer_seen.called,
            server.request_count,
            dcc_status_string(status),
            dcc_status_string(runner.status),
            after.pending,
            after.active,
            after.active_routes
        );
        return 1;
    }
    return 0;
}

int run_public_rest_async_route_serial_smoke(void) {
    http_server_t first;
    http_server_t second;
    pthread_t first_thread;
    pthread_t second_thread;
    if (start_server(&first, &first_thread) != 0) {
        fprintf(stderr, "failed to start first route serial server: %s\n", strerror(errno));
        return 1;
    }
    first.delay_ms = 100;
    if (start_server(&second, &second_thread) != 0) {
        fprintf(stderr, "failed to start second route serial server: %s\n", strerror(errno));
        close(first.fd);
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 2,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        close(first.fd);
        close(second.fd);
        return 1;
    }

    st = dcc_client_start(client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_start failed: %s\n", dcc_status_string(st));
        close(first.fd);
        close(second.fd);
        dcc_client_destroy(client);
        return 1;
    }

    char first_url[160];
    char second_url[160];
    snprintf(first_url, sizeof(first_url), "http://127.0.0.1:%u/channels/777/messages/100", (unsigned)first.port);
    snprintf(second_url, sizeof(second_url), "http://127.0.0.1:%u/channels/777/messages/101", (unsigned)second.port);

    rest_async_order_state_t state;
    memset(&state, 0, sizeof(state));
    state.expected = 2;
    rest_async_order_item_t first_item = { .state = &state, .label = 1 };
    rest_async_order_item_t second_item = { .state = &state, .label = 2 };
    dcc_rest_async_status_t route_status;
    memset(&route_status, 0, sizeof(route_status));
    route_status.size = sizeof(route_status);

    st = dcc_rest_request_async_priority(
        client,
        "GET",
        first_url,
        NULL,
        DCC_REST_PRIORITY_NORMAL,
        async_order_cb,
        &first_item
    );
    if (st == DCC_OK) {
        st = dcc_rest_request_async_priority(
            client,
            "GET",
            second_url,
            NULL,
            DCC_REST_PRIORITY_HIGH,
            async_order_cb,
            &second_item
        );
    }
    if (st == DCC_OK) {
        st = dcc_rest_async_status(client, &route_status);
    }
    if (st == DCC_OK &&
        (route_status.pending != 1 ||
         route_status.active != 1 ||
         route_status.active_routes != 1 ||
         route_status.concurrency != 2 ||
         route_status.pending_high != 1 ||
         route_status.pending_ready != 0 ||
         route_status.pending_blocked_by_active_route != 1 ||
         route_status.active_capacity != 1)) {
        fprintf(stderr,
                "unexpected route serial status: pending=%zu active=%zu routes=%zu concurrency=%u "
                "high=%zu ready=%zu blocked=%zu capacity=%zu\n",
                route_status.pending,
                route_status.active,
                route_status.active_routes,
                route_status.concurrency,
                route_status.pending_high,
                route_status.pending_ready,
                route_status.pending_blocked_by_active_route,
                route_status.active_capacity);
        st = DCC_ERR_STATE;
    }
    if (st == DCC_OK) {
        st = dcc_client_wait(client);
    } else {
        (void)dcc_client_stop(client);
    }

    (void)pthread_join(first_thread, NULL);
    (void)pthread_join(second_thread, NULL);
    close(first.fd);
    close(second.fd);
    dcc_client_destroy(client);

    if (st != DCC_OK ||
        state.called != 2 ||
        state.labels[0] != 1 ||
        state.labels[1] != 2 ||
        state.statuses[0] != 200 ||
        state.statuses[1] != 200 ||
        state.errors[0] != DCC_OK ||
        state.errors[1] != DCC_OK ||
        strcmp(first.path, "/channels/777/messages/100") != 0 ||
        strcmp(second.path, "/channels/777/messages/101") != 0) {
        fprintf(stderr,
                "unexpected route serial result: st=%s called=%d labels=%d,%d statuses=%u,%u paths=%s,%s\n",
                dcc_status_string(st),
                state.called,
                state.labels[0],
                state.labels[1],
                state.statuses[0],
                state.statuses[1],
                first.path,
                second.path);
        return 1;
    }
    return 0;
}

int run_public_rest_async_control_smoke(void) {
    http_server_t first;
    pthread_t first_thread;
    if (start_server(&first, &first_thread) != 0) {
        fprintf(stderr, "failed to start async control server: %s\n", strerror(errno));
        return 1;
    }
    first.delay_ms = 50;

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
        close(first.fd);
        return 1;
    }

    st = dcc_client_start(client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_start failed: %s\n", dcc_status_string(st));
        close(first.fd);
        dcc_client_destroy(client);
        return 1;
    }

    char first_url[128];
    snprintf(first_url, sizeof(first_url), "http://127.0.0.1:%u/async-control-first", (unsigned)first.port);

    rest_async_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_request_async(client, "GET", first_url, NULL, async_record_cb, &seen);
    if (st == DCC_OK) {
        st = dcc_rest_request_async(
            client,
            "GET",
            "http://127.0.0.1:9/async-control-canceled",
            NULL,
            async_record_cb,
            &seen
        );
    }

    size_t pending = 0;
    size_t active = 0;
    if (st == DCC_OK) {
        st = dcc_rest_async_pending(client, &pending, &active);
    }
    dcc_rest_async_status_t queue_status = {
        .size = sizeof(queue_status),
    };
    if (st == DCC_OK) {
        st = dcc_rest_async_status(client, &queue_status);
    }
    size_t canceled = 0;
    if (st == DCC_OK &&
        (pending != 1 ||
         active != 1 ||
         queue_status.pending != 1 ||
         queue_status.active != 1 ||
         queue_status.active_routes != 1 ||
         queue_status.concurrency != 1 ||
         queue_status.pending_low != 0 ||
         queue_status.pending_normal != 1 ||
         queue_status.pending_high != 0 ||
         queue_status.pending_ready != 1 ||
         queue_status.pending_blocked_by_active_route != 0 ||
         queue_status.active_capacity != 0 ||
         queue_status.rate_limit_buckets != 0 ||
         queue_status.rate_limit_blocked_buckets != 0)) {
        fprintf(
            stderr,
            "unexpected async status before cancel: pending=%zu active=%zu status=%zu/%zu routes=%zu "
            "concurrency=%u priorities=%zu,%zu,%zu ready=%zu blocked=%zu capacity=%zu buckets=%zu/%zu\n",
            pending,
            active,
            queue_status.pending,
            queue_status.active,
            queue_status.active_routes,
            queue_status.concurrency,
            queue_status.pending_low,
            queue_status.pending_normal,
            queue_status.pending_high,
            queue_status.pending_ready,
            queue_status.pending_blocked_by_active_route,
            queue_status.active_capacity,
            queue_status.rate_limit_buckets,
            queue_status.rate_limit_blocked_buckets
        );
        st = DCC_ERR_STATE;
    }
    if (st == DCC_OK) {
        st = dcc_rest_async_cancel_pending(client, &canceled);
    }
    if (st == DCC_OK && canceled != 1) {
        fprintf(stderr, "unexpected async canceled count: %zu\n", canceled);
        st = DCC_ERR_STATE;
    }

    client_wait_thread_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_STATE,
    };
    pthread_t wait_thread;
    int wait_thread_started = 0;
    if (st == DCC_OK) {
        if (pthread_create(&wait_thread, NULL, client_wait_thread_main, &wait_state) != 0) {
            fprintf(stderr, "failed to start client wait thread: %s\n", strerror(errno));
            st = DCC_ERR_RUNTIME;
        } else {
            wait_thread_started = 1;
        }
    }
    if (st == DCC_OK) {
        st = dcc_rest_async_wait(client, 2000);
    }
    if (st == DCC_OK) {
        pending = 1;
        active = 1;
        st = dcc_rest_async_pending(client, &pending, &active);
    }
    if (st == DCC_OK) {
        memset(&queue_status, 0, sizeof(queue_status));
        queue_status.size = sizeof(queue_status);
        st = dcc_rest_async_status(client, &queue_status);
    }

    (void)dcc_client_stop(client);
    if (wait_thread_started) {
        (void)pthread_join(wait_thread, NULL);
    }
    if (!wait_thread_started) {
        close(first.fd);
    }
    (void)pthread_join(first_thread, NULL);
    if (wait_thread_started) {
        close(first.fd);
    }
    dcc_client_destroy(client);

    if (st != DCC_OK ||
        wait_state.status != DCC_OK ||
        pending != 0 ||
        active != 0 ||
        queue_status.pending != 0 ||
        queue_status.active != 0 ||
        queue_status.active_routes != 0 ||
        queue_status.pending_ready != 0 ||
        queue_status.pending_blocked_by_active_route != 0 ||
        queue_status.active_capacity != 1 ||
        seen.called != 2 ||
        seen.statuses[0] != 0 ||
        seen.errors[0] != DCC_ERR_CANCELED ||
        seen.statuses[1] != 200 ||
        seen.errors[1] != DCC_OK ||
        strcmp(seen.bodies[1], "hello") != 0 ||
        first.request_count != 1 ||
        strcmp(first.path, "/async-control-first") != 0) {
        fprintf(stderr,
                "unexpected async control result: st=%s wait=%s pending=%zu active=%zu status=%zu/%zu/%zu "
                "canceled=%zu called=%d status=%u,%u err=%s,%s path=%s count=%u\n",
                dcc_status_string(st),
                dcc_status_string(wait_state.status),
                pending,
                active,
                queue_status.pending,
                queue_status.active,
                queue_status.active_routes,
                canceled,
                seen.called,
                seen.statuses[0],
                seen.statuses[1],
                dcc_status_string(seen.errors[0]),
                dcc_status_string(seen.errors[1]),
                first.path,
                first.request_count);
        return 1;
    }

    return 0;
}


#endif
