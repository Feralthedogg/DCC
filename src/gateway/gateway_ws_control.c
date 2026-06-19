#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_resume_internal.h"
#include "internal/gateway/dcc_gateway_ws_control_internal.h"
#include "internal/ws/dcc_ws.h"

#include <llam/runtime.h>

void dcc_gateway_control_lock(dcc_client_t *client) {
    while (atomic_flag_test_and_set_explicit(&client->gateway_control_lock, memory_order_acquire)) {
        llam_yield();
    }
}

void dcc_gateway_control_unlock(dcc_client_t *client) {
    atomic_flag_clear_explicit(&client->gateway_control_lock, memory_order_release);
}

int dcc_gateway_should_stop(const dcc_client_t *client) {
    return client == NULL ||
        atomic_load_explicit(&client->stopping, memory_order_acquire) ||
        atomic_load_explicit(&client->gateway_stop_requested, memory_order_acquire);
}

int dcc_gateway_reconnect_requested(const dcc_client_t *client) {
    return client != NULL &&
        atomic_load_explicit(&client->gateway_reconnect_requested, memory_order_acquire);
}

void dcc_gateway_interrupt_ws(dcc_ws_t *ws, int graceful) {
    (void)graceful;
    if (ws == NULL) {
        return;
    }
    dcc_ws_abort(ws);
}

void dcc_gateway_bind_ws(dcc_client_t *client, dcc_ws_t *ws) {
    if (client == NULL) {
        return;
    }
    dcc_gateway_control_lock(client);
    client->gateway_current_ws = ws;
    if (ws != NULL) {
        int stop = dcc_gateway_should_stop(client);
        if (stop || dcc_gateway_reconnect_requested(client)) {
            dcc_gateway_interrupt_ws(ws, stop);
        }
    }
    dcc_gateway_control_unlock(client);
}

void dcc_gateway_clear_ws(dcc_client_t *client, dcc_ws_t *ws) {
    if (client == NULL) {
        return;
    }
    dcc_gateway_control_lock(client);
    if (client->gateway_current_ws == ws) {
        client->gateway_current_ws = NULL;
    }
    dcc_gateway_control_unlock(client);
}

dcc_gateway_next_t dcc_gateway_take_reconnect_request(dcc_client_t *client) {
    int resume = client != NULL &&
        atomic_load_explicit(&client->gateway_reconnect_resume, memory_order_acquire) &&
        dcc_gateway_can_resume(client);
    if (client != NULL) {
        atomic_store_explicit(&client->gateway_reconnect_requested, false, memory_order_release);
        atomic_store_explicit(&client->gateway_reconnect_resume, false, memory_order_release);
        if (!resume) {
            dcc_gateway_clear_resume(client);
        }
    }
    return resume ? DCC_GATEWAY_NEXT_RECONNECT_RESUME : DCC_GATEWAY_NEXT_RECONNECT_IDENTIFY;
}
