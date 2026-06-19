#include "internal/client/dcc_client_state_internal.h"
#include "internal/gateway/dcc_gateway_control_internal.h"
#include "internal/gateway/dcc_gateway_lifecycle_internal.h"
#include "internal/gateway/dcc_gateway_runtime_internal.h"

dcc_status_t dcc_gateway_start(dcc_client_t *client) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (atomic_load_explicit(&client->gateway_task_running, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }
    atomic_store_explicit(&client->gateway_stop_requested, false, memory_order_release);
    atomic_store_explicit(&client->gateway_reconnect_requested, false, memory_order_release);
    atomic_store_explicit(&client->gateway_reconnect_resume, false, memory_order_release);
    if ((client->gateway_url == NULL || client->gateway_url[0] == '\0') &&
        (client->token == NULL || client->token[0] == '\0')) {
        return DCC_OK;
    }
    atomic_store_explicit(&client->gateway_last_status, DCC_OK, memory_order_release);
    atomic_store_explicit(&client->gateway_task_completed, false, memory_order_release);
    atomic_store_explicit(&client->gateway_task_running, true, memory_order_release);
    dcc_status_t status = dcc_runtime_spawn(&client->runtime, dcc_gateway_task, client);
    if (status != DCC_OK) {
        atomic_store_explicit(&client->gateway_task_running, false, memory_order_release);
        atomic_store_explicit(&client->gateway_task_completed, true, memory_order_release);
        atomic_store_explicit(&client->gateway_last_status, (int)status, memory_order_release);
    }
    return status;
}

dcc_status_t dcc_gateway_request_stop(dcc_client_t *client) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    atomic_store_explicit(&client->gateway_stop_requested, true, memory_order_release);
    atomic_store_explicit(&client->gateway_reconnect_requested, false, memory_order_release);
    atomic_store_explicit(&client->gateway_reconnect_resume, false, memory_order_release);

    dcc_gateway_control_lock(client);
    dcc_gateway_interrupt_ws((dcc_ws_t *)client->gateway_current_ws, 1);
    dcc_gateway_control_unlock(client);
    return DCC_OK;
}

dcc_status_t dcc_gateway_request_reconnect(dcc_client_t *client, uint8_t resume) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_gateway_should_stop(client)) {
        return DCC_ERR_STATE;
    }
    if (!client->runtime.initialized) {
        return DCC_ERR_STATE;
    }
    if (!atomic_load_explicit(&client->gateway_task_running, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }

    atomic_store_explicit(&client->gateway_reconnect_resume, resume != 0, memory_order_release);
    atomic_store_explicit(&client->gateway_reconnect_requested, true, memory_order_release);

    dcc_gateway_control_lock(client);
    dcc_gateway_interrupt_ws((dcc_ws_t *)client->gateway_current_ws, 0);
    dcc_gateway_control_unlock(client);
    return DCC_OK;
}
