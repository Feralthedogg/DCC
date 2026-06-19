#include "internal/voice/dcc_voice_internal.h"

#include <limits.h>

dcc_voice_reconnect_action_t dcc_voice_client_reconnect_action(const dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_VOICE_RECONNECT_ACTION_NONE;
    }
    if (voice_client->full_reconnect_requested) {
        return DCC_VOICE_RECONNECT_ACTION_FULL_RECONNECT;
    }
    if (voice_client->reconnect_requested ||
        voice_client->session_state == DCC_VOICE_SESSION_RECONNECTING) {
        return DCC_VOICE_RECONNECT_ACTION_RESUME;
    }
    if (voice_client->session_state == DCC_VOICE_SESSION_TERMINATED) {
        return DCC_VOICE_RECONNECT_ACTION_TERMINATE;
    }
    return DCC_VOICE_RECONNECT_ACTION_NONE;
}

dcc_status_t dcc_voice_client_handle_disconnect(
    dcc_voice_client_t *voice_client,
    uint16_t close_code,
    const char *reason
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    voice_client->last_close_code = close_code;
    dcc_status_t status =
        dcc_voice_copy_text(voice_client->last_close_reason, sizeof(voice_client->last_close_reason), reason);
    if (status != DCC_OK) {
        return status;
    }

    (void)dcc_voice_client_stop_receive_jitter_timer(voice_client);
    dcc_voice_client_disconnect_udp(voice_client);
    dcc_voice_reconnect_action_t action = dcc_voice_reconnect_action_for_close_code(close_code);
    if (action == DCC_VOICE_RECONNECT_ACTION_NONE ||
        action == DCC_VOICE_RECONNECT_ACTION_TERMINATE) {
        voice_client->session_state = action == DCC_VOICE_RECONNECT_ACTION_TERMINATE
            ? DCC_VOICE_SESSION_TERMINATED
            : DCC_VOICE_SESSION_DISCONNECTED;
        voice_client->reconnect_requested = 0;
        voice_client->full_reconnect_requested = 0;
        return DCC_OK;
    }

    if (voice_client->reconnect_attempts < UINT32_MAX) {
        voice_client->reconnect_attempts++;
    }
    if (voice_client->reconnect_attempts >= 5U) {
        dcc_voice_clear_session_descriptor(voice_client);
        dcc_voice_reset_transport_state(voice_client);
        voice_client->session_state = DCC_VOICE_SESSION_REQUESTED;
        voice_client->reconnect_requested = 0;
        voice_client->full_reconnect_requested = 1;
    } else {
        voice_client->session_state = DCC_VOICE_SESSION_RECONNECTING;
        voice_client->reconnect_requested = 1;
        voice_client->full_reconnect_requested = 0;
    }
    return DCC_OK;
}

uint8_t dcc_voice_client_reconnect_requested(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->reconnect_requested : 0;
}

uint8_t dcc_voice_client_full_reconnect_requested(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL ? voice_client->full_reconnect_requested : 0;
}

dcc_status_t dcc_voice_client_clear_reconnect_request(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    voice_client->reconnect_requested = 0;
    voice_client->full_reconnect_requested = 0;
    dcc_voice_refresh_session_state(voice_client);
    return DCC_OK;
}
