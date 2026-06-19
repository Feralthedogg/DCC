#include "internal/rest/dcc_rest_async_drain_internal.h"
#include "internal/rest/dcc_rest_async_worker_task_internal.h"

dcc_status_t dcc_rest_async_drain_locked(dcc_client_t *client) {
    if (client == NULL || !client->runtime.initialized) {
        return DCC_ERR_STATE;
    }

    uint32_t concurrency = dcc_rest_async_concurrency(client);
    dcc_status_t status = DCC_OK;

    while (!atomic_load_explicit(&client->stopping, memory_order_acquire) &&
           client->rest_async_active < concurrency &&
           status == DCC_OK) {
        dcc_rest_async_request_t *request = dcc_rest_async_take_next_locked(client);
        if (request == NULL) {
            break;
        }

        status = dcc_rest_async_mark_route_active_locked(client, request->route);
        if (status != DCC_OK) {
            dcc_rest_async_push_head_locked(client, request);
            break;
        }
        client->rest_async_active++;
        dcc_rest_async_push_active_locked(client, request);

        status = dcc_runtime_spawn(&client->runtime, dcc_rest_async_worker_task, request);
        if (status != DCC_OK) {
            dcc_rest_async_remove_active_locked(client, request);
            dcc_rest_async_unmark_route_active_locked(client, request->route);
            client->rest_async_active--;
            dcc_rest_async_push_head_locked(client, request);
            break;
        }
    }

    return status;
}
