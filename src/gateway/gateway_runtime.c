#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_runtime_internal.h"

void dcc_gateway_task(void *arg) {
    dcc_client_t *client = (dcc_client_t *)arg;
    dcc_emit_log(client, DCC_LOG_INFO, "gateway task starting");
    dcc_status_t status = dcc_gateway_run(client);
    atomic_store_explicit(&client->gateway_last_status, (int)status, memory_order_release);
    atomic_store_explicit(&client->gateway_task_running, false, memory_order_release);
    atomic_store_explicit(&client->gateway_task_completed, true, memory_order_release);
    if (status != DCC_OK && !atomic_load_explicit(&client->stopping, memory_order_acquire)) {
        dcc_emit_log(client, DCC_LOG_ERROR, dcc_status_string(status));
        if (client->gateway_stop_runtime_on_error) {
            (void)dcc_client_stop(client);
        }
    }
    dcc_emit_log(client, DCC_LOG_INFO, "gateway task stopped");
}
