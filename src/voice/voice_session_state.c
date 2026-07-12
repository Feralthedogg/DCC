#include "internal/voice/dcc_voice_internal.h"

void dcc_voice_clear_session_descriptor(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return;
    }
    voice_client->session_id[0] = '\0';
    voice_client->session_token[0] = '\0';
    voice_client->endpoint[0] = '\0';
}

uint8_t dcc_voice_session_descriptor_ready(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL &&
        voice_client->session_id[0] != '\0' &&
        voice_client->session_token[0] != '\0' &&
        voice_client->endpoint[0] != '\0'
        ? 1U
        : 0U;
}

void dcc_voice_refresh_session_state(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL ||
        voice_client->session_state == DCC_VOICE_SESSION_DISCONNECTED ||
        voice_client->session_state == DCC_VOICE_SESSION_ACTIVE ||
        voice_client->session_state == DCC_VOICE_SESSION_TERMINATED) {
        return;
    }
    voice_client->session_state = dcc_voice_session_descriptor_ready(voice_client)
        ? DCC_VOICE_SESSION_READY
        : DCC_VOICE_SESSION_REQUESTED;
}

const char *dcc_voice_session_state_string(dcc_voice_session_state_t state) {
    switch (state) {
        case DCC_VOICE_SESSION_DISCONNECTED:
            return "disconnected";
        case DCC_VOICE_SESSION_REQUESTED:
            return "requested";
        case DCC_VOICE_SESSION_READY:
            return "ready";
        case DCC_VOICE_SESSION_ACTIVE:
            return "active";
        case DCC_VOICE_SESSION_RECONNECTING:
            return "reconnecting";
        case DCC_VOICE_SESSION_TERMINATED:
            return "terminated";
        default:
            return "unknown";
    }
}

dcc_voice_session_state_t dcc_voice_client_session_state(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->session_state : DCC_VOICE_SESSION_DISCONNECTED;
}

uint8_t dcc_voice_client_session_ready(const dcc_voice_client_t *voice_client) {
    return dcc_voice_session_descriptor_ready(voice_client);
}

uint8_t dcc_voice_client_session_active(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL && voice_client->session_state == DCC_VOICE_SESSION_ACTIVE ? 1U : 0U;
}

dcc_status_t dcc_voice_client_mark_session_active(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->session_state = DCC_VOICE_SESSION_ACTIVE;
    voice_client->reconnect_requested = 0;
    voice_client->full_reconnect_requested = 0;
    voice_client->reconnect_attempts = 0;
    return DCC_OK;
}

dcc_status_t dcc_voice_client_disconnect_session(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    (void)dcc_voice_client_stop_receive_jitter_timer(voice_client);
    dcc_voice_dave_backend_reset_session(voice_client);
    dcc_voice_clear_session_descriptor(voice_client);
    dcc_voice_reset_transport_state(voice_client);
    voice_client->guild_id = 0;
    voice_client->channel_id = 0;
    voice_client->session_state = DCC_VOICE_SESSION_DISCONNECTED;
    voice_client->reconnect_requested = 0;
    voice_client->full_reconnect_requested = 0;
    voice_client->reconnect_attempts = 0;
    voice_client->heartbeat_interval_ms = 0;
    voice_client->receive_sequence = -1;
    voice_client->self_mute = 0;
    voice_client->self_deaf = 0;
    voice_client->dave_enabled = 0;
    voice_client->dave_version = DCC_VOICE_DAVE_NONE;
    voice_client->dave_pending_version = DCC_VOICE_DAVE_NONE;
    voice_client->dave_transition_pending = 0;
    voice_client->dave_transition_ready = 0;
    voice_client->dave_transition_id = 0;
    voice_client->dave_media_ready = 0U;
    voice_client->dave_epoch = 0U;
    voice_client->dave_participant_count = 0U;
    return DCC_OK;
}
