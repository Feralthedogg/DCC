#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_session_receive_internal.h"
#include "internal/ws/dcc_ws.h"

#include <string.h>

static _Thread_local dcc_json_gateway_payload_t dcc_gateway_hello_payload_scratch;

dcc_status_t dcc_gateway_read_hello(dcc_gateway_session_t *session) {
    dcc_ws_message_t message;
    memset(&message, 0, sizeof(message));
    dcc_status_t status = dcc_ws_recv(session->ws, &message);
    if (status != DCC_OK) {
        dcc_set_error(session->client, "gateway did not send HELLO");
        return status;
    }

    dcc_json_gateway_payload_t *payload = &dcc_gateway_hello_payload_scratch;

    status = dcc_json_parse_gateway_payload((const char *)message.data, message.len, payload);
    if (status != DCC_OK) {
        dcc_ws_message_deinit(&message);
        dcc_set_error(session->client, "invalid gateway JSON");
        return status;
    }

    if (!payload->has_op || payload->op != 10 ||
        !payload->has_heartbeat_interval ||
        payload->heartbeat_interval <= 0) {
        dcc_ws_message_deinit(&message);
        dcc_set_error(session->client, "invalid gateway HELLO");
        return DCC_ERR_DISCORD;
    }

    session->heartbeat_interval_ms = (uint32_t)payload->heartbeat_interval;
    dcc_ws_message_deinit(&message);
    return DCC_OK;
}
