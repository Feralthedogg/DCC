#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_apply_voice_state(
    dcc_voice_client_t *voice_client,
    const dcc_voice_state_t *voice_state,
    dcc_snowflake_t self_user_id
) {
    if (voice_client == NULL || voice_state == NULL || self_user_id == 0 || voice_state->guild_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_state->user_id != self_user_id) {
        return DCC_OK;
    }
    if (voice_client->guild_id != 0 && voice_client->guild_id != voice_state->guild_id) {
        return DCC_OK;
    }

    if (voice_state->channel_id == 0) {
        return dcc_voice_client_disconnect_session(voice_client);
    }
    if (voice_state->session_id == NULL || voice_state->session_id[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    if (voice_client->channel_id != 0 && voice_client->channel_id != voice_state->channel_id) {
        dcc_voice_clear_session_descriptor(voice_client);
        dcc_voice_reset_transport_state(voice_client);
        voice_client->session_state = DCC_VOICE_SESSION_REQUESTED;
    }

    voice_client->guild_id = voice_state->guild_id;
    voice_client->channel_id = voice_state->channel_id;
    dcc_status_t status =
        dcc_voice_copy_text(voice_client->session_id, sizeof(voice_client->session_id), voice_state->session_id);
    if (status != DCC_OK) {
        return status;
    }

    voice_client->reconnect_requested = 0;
    voice_client->full_reconnect_requested = 0;
    if (voice_client->session_state == DCC_VOICE_SESSION_DISCONNECTED ||
        voice_client->session_state == DCC_VOICE_SESSION_TERMINATED) {
        voice_client->session_state = DCC_VOICE_SESSION_REQUESTED;
    }
    dcc_voice_refresh_session_state(voice_client);
    return DCC_OK;
}

dcc_status_t dcc_voice_client_apply_voice_server_update(
    dcc_voice_client_t *voice_client,
    const dcc_voice_server_update_t *voice_server
) {
    if (voice_client == NULL ||
        voice_server == NULL ||
        voice_server->guild_id == 0 ||
        voice_server->token == NULL ||
        voice_server->token[0] == '\0' ||
        voice_server->endpoint == NULL ||
        voice_server->endpoint[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->guild_id != 0 && voice_client->guild_id != voice_server->guild_id) {
        return DCC_OK;
    }

    dcc_status_t status =
        dcc_voice_copy_text(voice_client->session_token, sizeof(voice_client->session_token), voice_server->token);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_voice_copy_text(voice_client->endpoint, sizeof(voice_client->endpoint), voice_server->endpoint);
    if (status != DCC_OK) {
        voice_client->session_token[0] = '\0';
        return status;
    }

    voice_client->guild_id = voice_server->guild_id;
    voice_client->reconnect_requested = 0;
    voice_client->full_reconnect_requested = 0;
    if (voice_client->session_state == DCC_VOICE_SESSION_DISCONNECTED ||
        voice_client->session_state == DCC_VOICE_SESSION_TERMINATED) {
        voice_client->session_state = DCC_VOICE_SESSION_REQUESTED;
    }
    dcc_voice_refresh_session_state(voice_client);
    return DCC_OK;
}
