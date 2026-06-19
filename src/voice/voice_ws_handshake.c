#include "internal/voice/dcc_voice_ws_internal.h"

#include <string.h>

dcc_status_t dcc_voice_ws_read_hello(dcc_voice_client_t *voice_client, dcc_ws_t *ws) {
    dcc_ws_message_t message;
    memset(&message, 0, sizeof(message));
    dcc_status_t status = dcc_ws_recv(ws, &message);
    if (status != DCC_OK) {
        dcc_ws_message_deinit(&message);
        return status;
    }
    if (message.opcode != DCC_WS_TEXT || message.data == NULL) {
        dcc_ws_message_deinit(&message);
        return DCC_ERR_DISCORD;
    }
    status = dcc_voice_client_handle_gateway_frame(voice_client, (const char *)message.data, message.len);
    dcc_ws_message_deinit(&message);
    if (status != DCC_OK) {
        return status;
    }
    return voice_client->heartbeat_interval_ms != 0 ? DCC_OK : DCC_ERR_DISCORD;
}

dcc_status_t dcc_voice_ws_maybe_select_protocol(dcc_voice_ws_session_t *session) {
    if (session == NULL || session->voice_client == NULL || session->ws == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_client_t *voice_client = session->voice_client;
    if (voice_client->select_protocol_sent ||
        voice_client->ssrc == 0 ||
        voice_client->udp_host[0] == '\0' ||
        voice_client->udp_port == 0) {
        return DCC_OK;
    }

    if (LLAM_FD_IS_INVALID(voice_client->udp_fd)) {
        dcc_status_t status =
            dcc_voice_client_connect_udp(voice_client, voice_client->udp_host, voice_client->udp_port);
        if (status != DCC_OK) {
            return status;
        }
    }

    char address[sizeof(voice_client->discovered_address)];
    uint16_t port = 0;
    dcc_status_t status = dcc_voice_client_discover_udp_ip(
        voice_client,
        address,
        sizeof(address),
        &port,
        1000U
    );
    if (status != DCC_OK) {
        return status;
    }

    char payload[256];
    size_t payload_len = 0;
    status = dcc_voice_client_build_select_protocol_payload(
        address,
        port,
        DCC_VOICE_ENCRYPTION_AEAD_XCHACHA20_POLY1305_RTPSIZE,
        payload,
        sizeof(payload),
        &payload_len
    );
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_ws_send_text(session->ws, payload, payload_len);
    if (status == DCC_OK) {
        voice_client->select_protocol_sent = 1;
    }
    return status;
}
