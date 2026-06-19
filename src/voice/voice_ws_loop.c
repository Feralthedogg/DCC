#include "internal/voice/dcc_voice_ws_internal.h"

static uint8_t dcc_voice_client_should_resume_websocket(const dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return 0;
    }
    return voice_client->session_state == DCC_VOICE_SESSION_RECONNECTING ||
        voice_client->select_protocol_sent ||
        voice_client->receive_sequence >= 0
        ? 1U
        : 0U;
}

static uint8_t dcc_voice_client_reconnect_loop_terminal(const dcc_voice_client_t *voice_client) {
    return voice_client == NULL ||
        voice_client->full_reconnect_requested ||
        voice_client->session_state == DCC_VOICE_SESSION_TERMINATED
        ? 1U
        : 0U;
}

dcc_status_t dcc_voice_client_run_websocket_loop_url(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    const char *url,
    uint32_t reconnect_delay_ms
) {
    if (voice_client == NULL || self_user_id == 0 || url == NULL || url[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t last_status = DCC_OK;
    for (;;) {
        if (dcc_voice_client_websocket_stop_requested(voice_client)) {
            return DCC_ERR_CANCELED;
        }
        uint8_t resume = dcc_voice_client_should_resume_websocket(voice_client);
        last_status = dcc_voice_client_run_websocket_url(voice_client, self_user_id, url, resume);
        if (dcc_voice_client_websocket_stop_requested(voice_client)) {
            return DCC_ERR_CANCELED;
        }
        if (last_status == DCC_ERR_INVALID_ARG || last_status == DCC_ERR_STATE) {
            return last_status;
        }
        if (last_status != DCC_OK &&
            !voice_client->reconnect_requested &&
            !voice_client->full_reconnect_requested &&
            voice_client->session_state != DCC_VOICE_SESSION_DISCONNECTED &&
            voice_client->session_state != DCC_VOICE_SESSION_TERMINATED) {
            (void)dcc_voice_client_handle_disconnect(voice_client, 1006U, "voice websocket reconnect loop error");
        }
        if (dcc_voice_client_reconnect_loop_terminal(voice_client)) {
            return last_status == DCC_OK ? DCC_ERR_CANCELED : last_status;
        }
        if (!voice_client->reconnect_requested) {
            return last_status;
        }
        if (dcc_voice_client_websocket_sleep(voice_client, reconnect_delay_ms)) {
            return DCC_ERR_CANCELED;
        }
    }
}

dcc_status_t dcc_voice_client_run_websocket_loop(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    uint32_t reconnect_delay_ms
) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    char url[768];
    size_t url_len = 0;
    dcc_status_t status = dcc_voice_client_build_websocket_url(voice_client, url, sizeof(url), &url_len);
    if (status != DCC_OK) {
        return status;
    }
    (void)url_len;
    return dcc_voice_client_run_websocket_loop_url(voice_client, self_user_id, url, reconnect_delay_ms);
}
