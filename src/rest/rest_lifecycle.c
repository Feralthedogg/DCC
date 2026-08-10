#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_async_cancel_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"
#include "internal/rest/dcc_rest_async_signal_internal.h"
#include "internal/rest/dcc_rest_firewall_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"

#include <stdatomic.h>
#include <string.h>

static void dcc_rest_async_cancel_shutdown_active_list(
    dcc_client_t *client,
    dcc_rest_async_request_t *request
) {
    while (request != NULL) {
        dcc_rest_async_request_t *next = request->active_next;
        request->active_next = NULL;
        if (!request->callback_called) {
            dcc_rest_terminal_completion_t completion = {
                .operation = request->path,
                .transport_status = DCC_ERR_CANCELED,
                .legacy_error = DCC_ERR_CANCELED,
            };
            request->callback_called = 1;
            dcc_rest_deliver_terminal(
                client,
                &completion,
                request->cb,
                request->user_data
            );
        }
        dcc_rest_async_request_free(request);
        request = next;
    }
}

dcc_status_t dcc_rest_init(dcc_client_t *client) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    atomic_flag_clear(&client->rest_lock);
    atomic_flag_clear(&client->rest_error_observer_lock);
    client->rest_error_observer = NULL;
    client->rest_error_observer_user_data = NULL;
    client->rest_app_error_sink = NULL;
    client->rest_app_error_sink_user_data = NULL;
    atomic_init(&client->rest_app_error_sink_in_flight, 0U);
    atomic_init(&client->rest_terminal_in_flight, 0U);
    client->rest_intercept = NULL;
    client->rest_intercept_user_data = NULL;
    client->rest_firewall = NULL;
#if !defined(_WIN32)
    client->rest_wait_initialized = 0;
    if (pthread_mutex_init(&client->rest_wait_mutex, NULL) != 0) {
        return DCC_ERR_RUNTIME;
    }
    if (pthread_cond_init(&client->rest_wait_cond, NULL) != 0) {
        (void)pthread_mutex_destroy(&client->rest_wait_mutex);
        return DCC_ERR_RUNTIME;
    }
    client->rest_wait_initialized = 1;
#endif
    client->rest_global_reset_at_ms = 0;
    memset(client->rest_buckets, 0, sizeof(client->rest_buckets));
    memset(client->rest_async_heads, 0, sizeof(client->rest_async_heads));
    memset(client->rest_async_tails, 0, sizeof(client->rest_async_tails));
    client->rest_async_active_head = NULL;
    memset(client->rest_async_active_routes, 0, sizeof(client->rest_async_active_routes));
    client->rest_async_active_route_count = 0;
    client->rest_async_active = 0;
    client->rest_test_fail_next_worker_spawn = 0U;
    return DCC_OK;
}

void dcc_rest_deinit(dcc_client_t *client) {
    if (client != NULL) {
        dcc_rest_async_request_t *pending = NULL;
        dcc_rest_async_request_t *active = NULL;
        dcc_rest_lock(client);
        dcc_rest_firewall_state_t *firewall = client->rest_firewall;
        client->rest_firewall = NULL;
        client->rest_global_reset_at_ms = 0;
        memset(client->rest_buckets, 0, sizeof(client->rest_buckets));
        pending = dcc_rest_async_detach_pending_all_locked(client);
        active = client->rest_async_active_head;
        client->rest_async_active_head = NULL;
        memset(client->rest_async_active_routes, 0, sizeof(client->rest_async_active_routes));
        client->rest_async_active_route_count = 0;
        client->rest_async_active = 0;
        dcc_rest_unlock(client);
        (void)dcc_rest_async_cancel_pending_list(client, pending);
        dcc_rest_async_cancel_shutdown_active_list(client, active);
        dcc_rest_firewall_state_free(firewall);
        dcc_rest_async_signal(client);
#if !defined(_WIN32)
        if (client->rest_wait_initialized) {
            client->rest_wait_initialized = 0;
            (void)pthread_cond_destroy(&client->rest_wait_cond);
            (void)pthread_mutex_destroy(&client->rest_wait_mutex);
        }
#endif
    }
}
