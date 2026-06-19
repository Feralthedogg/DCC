#include "internal/voice/dcc_voice_ws_internal.h"

dcc_status_t dcc_voice_client_request_full_reconnect(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_voice_client_websocket_stop_requested(voice_client)) {
        return DCC_ERR_CANCELED;
    }
    if (voice_client->client == NULL || voice_client->guild_id == 0 || voice_client->channel_id == 0) {
        return DCC_ERR_STATE;
    }

    uint32_t previous_attempts = voice_client->reconnect_attempts;
    voice_client->reconnect_requested = 0;
    voice_client->full_reconnect_requested = 0;
    voice_client->reconnect_attempts = 0;
    voice_client->session_state = DCC_VOICE_SESSION_REQUESTED;

    dcc_status_t status = dcc_gateway_send_voice_state_update(
        voice_client->client,
        voice_client->guild_id,
        voice_client->channel_id,
        voice_client->self_mute,
        voice_client->self_deaf
    );
    if (status != DCC_OK) {
        voice_client->reconnect_requested = 0;
        voice_client->full_reconnect_requested = 1;
        voice_client->reconnect_attempts = previous_attempts;
        voice_client->session_state = DCC_VOICE_SESSION_REQUESTED;
        return status;
    }

    return DCC_OK;
}
