#include "internal/rest/dcc_rest_async_drain_internal.h"
#include "internal/rest/dcc_rest_async_cancel_internal.h"
#include "internal/rest/dcc_rest_async_signal_internal.h"
#include "internal/rest/dcc_rest_async_worker_lifecycle_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"

void dcc_rest_async_finish_active_locked(dcc_client_t *client, dcc_rest_async_request_t *request) {
    if (client == NULL || request == NULL) {
        return;
    }
    if (client->rest_async_active > 0) {
        client->rest_async_active--;
    }
    dcc_rest_async_remove_active_locked(client, request);
    if (request->route_claimed) {
        dcc_rest_async_unmark_route_active_locked(client, request->route);
        request->route_claimed = 0U;
    }
}

void dcc_rest_async_complete(dcc_rest_async_request_t *request) {
    dcc_client_t *client = request != NULL ? request->client : NULL;
    if (client == NULL) {
        return;
    }

    dcc_rest_async_request_t *pending = NULL;
    dcc_rest_lock(client);
    dcc_rest_async_finish_active_locked(client, request);
    if (atomic_load_explicit(&client->stopping, memory_order_acquire)) {
        pending = dcc_rest_async_detach_pending_all_locked(client);
    } else {
        dcc_status_t drain_status = dcc_rest_async_drain_locked(client);
        if (drain_status != DCC_OK) {
            /* A failed spawn restores the exact queue position. Retry once so
             * a transient rejection cannot strand a canceled request after
             * the last active worker completes. Persistent failure remains
             * owned by the pending queue for wait/teardown recovery. */
            (void)dcc_rest_async_drain_locked(client);
        }
    }
    dcc_rest_unlock(client);
    dcc_rest_async_signal(client);
    (void)dcc_rest_async_cancel_pending_list(client, pending);
}
