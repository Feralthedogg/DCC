#include "internal/rest/dcc_rest_async_drain_internal.h"
#include "internal/rest/dcc_rest_async_worker_task_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"

void dcc_rest_test_fail_next_worker_spawn(dcc_client_t *client) {
    if (client == NULL) {
        return;
    }
    dcc_rest_lock(client);
    client->rest_test_fail_next_worker_spawn = 1U;
    dcc_rest_unlock(client);
}

static dcc_status_t dcc_rest_async_spawn_worker_locked(
    dcc_client_t *client,
    dcc_rest_async_request_t *request
) {
    if (client->rest_test_fail_next_worker_spawn) {
        client->rest_test_fail_next_worker_spawn = 0U;
        return DCC_ERR_RUNTIME;
    }
    return dcc_runtime_spawn(&client->runtime, dcc_rest_async_worker_task, request);
}

static dcc_status_t dcc_rest_async_drain_impl_locked(
    dcc_client_t *client,
    dcc_rest_async_request_t *admission,
    dcc_rest_async_request_t **out_rejected
) {
    if (client == NULL || !client->runtime.initialized) {
        return DCC_ERR_STATE;
    }
    if (out_rejected != NULL) {
        *out_rejected = NULL;
    }

    uint32_t concurrency = dcc_rest_async_concurrency(client);
    dcc_status_t status = DCC_OK;

    while (!atomic_load_explicit(&client->stopping, memory_order_acquire) &&
           client->rest_async_active < concurrency &&
           status == DCC_OK) {
        dcc_rest_async_queue_position_t position = {0};
        dcc_rest_async_request_t *request = dcc_rest_async_take_next_locked(
            client,
            &position
        );
        if (request == NULL) {
            break;
        }

        request->route_claimed = 0U;
        int canceled = atomic_load_explicit(
            &request->cancel_requested,
            memory_order_acquire
        );
        status = canceled
            ? DCC_OK
            : dcc_rest_async_mark_route_active_locked(client, request->route);
        if (status != DCC_OK) {
            if (request == admission && out_rejected != NULL) {
                *out_rejected = request;
            } else {
                dcc_rest_async_restore_locked(client, request, &position);
                if (admission != NULL) {
                    status = DCC_OK;
                }
            }
            break;
        }
        request->route_claimed = !canceled && request->route[0] != '\0' ? 1U : 0U;
        client->rest_async_active++;
        dcc_rest_async_push_active_locked(client, request);

        status = dcc_rest_async_spawn_worker_locked(client, request);
        if (status != DCC_OK) {
            dcc_rest_async_remove_active_locked(client, request);
            if (request->route_claimed) {
                dcc_rest_async_unmark_route_active_locked(client, request->route);
                request->route_claimed = 0U;
            }
            client->rest_async_active--;
            if (request == admission && out_rejected != NULL) {
                *out_rejected = request;
            } else {
                dcc_rest_async_restore_locked(client, request, &position);
                if (admission != NULL) {
                    status = DCC_OK;
                }
            }
            break;
        }
    }

    return status;
}

dcc_status_t dcc_rest_async_drain_locked(dcc_client_t *client) {
    return dcc_rest_async_drain_impl_locked(client, NULL, NULL);
}

dcc_status_t dcc_rest_async_drain_admission_locked(
    dcc_client_t *client,
    dcc_rest_async_request_t *admission,
    dcc_rest_async_request_t **out_rejected
) {
    if (admission == NULL || out_rejected == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_async_drain_impl_locked(client, admission, out_rejected);
}
