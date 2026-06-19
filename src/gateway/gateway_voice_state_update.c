#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_voice_state_update_internal.h"
#include "internal/gateway/dcc_gateway_ws_control_internal.h"
#include "internal/ws/dcc_ws.h"

#include <stdio.h>

dcc_status_t dcc_gateway_send_voice_state_update(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t channel_id,
    uint8_t self_mute,
    uint8_t self_deaf
) {
    if (client == NULL || guild_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_gateway_should_stop(client) ||
        !client->runtime.initialized ||
        !atomic_load_explicit(&client->started, memory_order_acquire) ||
        !atomic_load_explicit(&client->gateway_task_running, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }

    char payload[256];
    int n = 0;
    if (channel_id == 0) {
        n = snprintf(
            payload,
            sizeof(payload),
            "{\"op\":4,\"d\":{\"guild_id\":\"%llu\",\"channel_id\":null,"
            "\"self_mute\":%s,\"self_deaf\":%s}}",
            (unsigned long long)guild_id,
            self_mute ? "true" : "false",
            self_deaf ? "true" : "false"
        );
    } else {
        n = snprintf(
            payload,
            sizeof(payload),
            "{\"op\":4,\"d\":{\"guild_id\":\"%llu\",\"channel_id\":\"%llu\","
            "\"self_mute\":%s,\"self_deaf\":%s}}",
            (unsigned long long)guild_id,
            (unsigned long long)channel_id,
            self_mute ? "true" : "false",
            self_deaf ? "true" : "false"
        );
    }
    if (n <= 0 || (size_t)n >= sizeof(payload)) {
        return DCC_ERR_NOMEM;
    }

    dcc_gateway_control_lock(client);
    dcc_ws_t *ws = (dcc_ws_t *)client->gateway_current_ws;
    dcc_status_t status = ws != NULL ? dcc_ws_retain(ws) : DCC_ERR_STATE;
    dcc_gateway_control_unlock(client);
    if (status == DCC_OK) {
        status = dcc_ws_send_text(ws, payload, (size_t)n);
        dcc_ws_destroy(ws);
    }
    if (status != DCC_OK) {
        dcc_set_error(client, "gateway voice state update send failed");
    }
    return status;
}
