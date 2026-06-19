#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_control_internal.h"
#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_identify_internal.h"
#include "internal/gateway/dcc_gateway_runtime_internal.h"
#include "internal/gateway/dcc_gateway_session_receive_internal.h"
#include "internal/json/dcc_json.h"
#include "internal/ws/dcc_ws.h"

#include <string.h>

static _Thread_local dcc_json_gateway_payload_t dcc_gateway_payload_scratch;

dcc_status_t dcc_gateway_session_receive_loop(dcc_gateway_session_t *session) {
    dcc_client_t *client = session->client;
    dcc_status_t status = DCC_OK;

    while (!dcc_gateway_should_stop(client) && !dcc_gateway_reconnect_requested(client)) {
        dcc_ws_message_t message;
        memset(&message, 0, sizeof(message));
        status = dcc_ws_recv(session->ws, &message);
        if (status == DCC_ERR_CANCELED && dcc_gateway_should_stop(client)) {
            status = DCC_OK;
            session->next = DCC_GATEWAY_NEXT_STOP;
            break;
        }
        if (status == DCC_ERR_CANCELED && dcc_gateway_reconnect_requested(client)) {
            session->next = dcc_gateway_take_reconnect_request(client);
            break;
        }
        if (status == DCC_ERR_CANCELED) {
            uint16_t close_code = dcc_ws_last_close_code(session->ws);
            (void)dcc_gateway_emit_socket_close(session, close_code, dcc_ws_last_close_reason(session->ws));
            session->next = dcc_gateway_next_for_close(session, close_code);
            break;
        }
        if (status != DCC_OK && dcc_gateway_reconnect_requested(client)) {
            session->next = dcc_gateway_take_reconnect_request(client);
            break;
        }
        if (status != DCC_OK) {
            session->next = dcc_gateway_reconnect_next(client);
            if (session->heartbeat == NULL ||
                !atomic_load_explicit(&session->heartbeat->failed, memory_order_acquire)) {
                dcc_set_error(client, "gateway receive failed");
            }
            break;
        }

        dcc_json_gateway_payload_t *payload = &dcc_gateway_payload_scratch;

        status = dcc_json_parse_gateway_payload((const char *)message.data, message.len, payload);
        if (status != DCC_OK) {
            dcc_ws_message_deinit(&message);
            dcc_set_error(client, "invalid gateway JSON");
            session->next = DCC_GATEWAY_NEXT_RECONNECT_IDENTIFY;
            break;
        }

        status = dcc_gateway_session_handle_payload(
            session,
            payload,
            (const char *)message.data,
            message.len
        );

        dcc_ws_message_deinit(&message);

        if (status != DCC_OK || dcc_gateway_should_stop(client) || dcc_gateway_reconnect_requested(client)) {
            break;
        }
    }

    return status;
}
