#include "internal/voice/dcc_voice_ws_internal.h"

#include <string.h>

dcc_status_t dcc_voice_ws_receive_loop(dcc_voice_ws_session_t *session) {
    dcc_voice_client_t *voice_client = session->voice_client;
    dcc_ws_t *ws = session->ws;

    for (;;) {
        dcc_ws_message_t message;
        memset(&message, 0, sizeof(message));
        dcc_status_t status = dcc_ws_recv(ws, &message);
        if (status == DCC_ERR_CANCELED) {
            if (dcc_voice_client_websocket_stop_requested(voice_client)) {
                status = DCC_ERR_CANCELED;
            } else {
                uint16_t close_code = dcc_ws_last_close_code(ws);
                const char *reason = dcc_ws_last_close_reason(ws);
                dcc_status_t close_status = dcc_voice_client_handle_disconnect(voice_client, close_code, reason);
                if (close_status != DCC_OK) {
                    status = close_status;
                } else if (close_code == 1000U) {
                    status = DCC_OK;
                }
            }
            dcc_ws_message_deinit(&message);
            return status;
        }
        if (status != DCC_OK) {
            if (dcc_voice_client_websocket_stop_requested(voice_client)) {
                status = DCC_ERR_CANCELED;
            } else {
                dcc_status_t close_status =
                    dcc_voice_client_handle_disconnect(voice_client, 1006U, "voice websocket network error");
                if (close_status != DCC_OK) {
                    status = close_status;
                } else if (voice_client->reconnect_requested || voice_client->full_reconnect_requested) {
                    status = DCC_ERR_CANCELED;
                }
            }
            dcc_ws_message_deinit(&message);
            return status;
        }
        if (message.opcode == DCC_WS_TEXT && message.data != NULL) {
            status = dcc_voice_client_handle_gateway_frame(
                voice_client,
                (const char *)message.data,
                message.len
            );
            if (status == DCC_OK) {
                status = dcc_voice_ws_maybe_select_protocol(session);
            }
        } else if (message.opcode == DCC_WS_BINARY && message.data != NULL) {
            status = dcc_voice_client_handle_gateway_binary_frame(
                voice_client,
                (const uint8_t *)message.data,
                message.len
            );
        }
        dcc_ws_message_deinit(&message);
        if (status != DCC_OK) {
            return status;
        }
    }
}
