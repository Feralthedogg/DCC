#include "internal/voice/dcc_voice_ws_internal.h"

#include <string.h>

dcc_status_t dcc_voice_client_run_websocket_url(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    const char *url,
    uint8_t resume
) {
    if (voice_client == NULL || self_user_id == 0 || url == NULL || url[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (voice_client->client == NULL ||
        !voice_client->client->runtime.initialized ||
        !atomic_load_explicit(&voice_client->client->started, memory_order_acquire) ||
        llam_current_task() == NULL) {
        return DCC_ERR_STATE;
    }
    if (!dcc_voice_session_descriptor_ready(voice_client)) {
        return DCC_ERR_STATE;
    }
    if (dcc_voice_client_websocket_stop_requested(voice_client)) {
        return DCC_ERR_CANCELED;
    }

    dcc_ws_t *ws = NULL;
    dcc_status_t status = dcc_ws_connect(&ws, url, 30000);
    if (status != DCC_OK) {
        dcc_set_error(voice_client->client, "voice websocket connect failed");
        return status;
    }
    dcc_voice_client_websocket_bind_current(voice_client, ws);

    status = dcc_voice_ws_read_hello(voice_client, ws);
    if (status != DCC_OK) {
        if (dcc_voice_client_websocket_stop_requested(voice_client)) {
            status = DCC_ERR_CANCELED;
        } else {
            dcc_set_error(voice_client->client, "voice websocket HELLO failed");
        }
        dcc_voice_client_websocket_clear_current(voice_client, ws);
        dcc_ws_destroy(ws);
        return status;
    }

    dcc_voice_ws_session_t session;
    memset(&session, 0, sizeof(session));
    session.voice_client = voice_client;
    session.ws = ws;
    atomic_init(&session.stop_heartbeat, false);
    atomic_init(&session.heartbeat_failed, false);

    llam_task_t *heartbeat = llam_spawn(dcc_voice_ws_heartbeat_task, &session, NULL);
    if (heartbeat == NULL) {
        dcc_voice_client_websocket_clear_current(voice_client, ws);
        dcc_ws_destroy(ws);
        dcc_set_error(voice_client->client, "failed to spawn voice heartbeat");
        return DCC_ERR_RUNTIME;
    }

    status = dcc_voice_ws_send_initial_payload(voice_client, self_user_id, resume, ws);
    if (status == DCC_OK) {
        status = dcc_voice_ws_receive_loop(&session);
    }

    atomic_store_explicit(&session.stop_heartbeat, true, memory_order_release);
    (void)llam_join(heartbeat);

    if (atomic_load_explicit(&session.heartbeat_failed, memory_order_acquire) && status == DCC_ERR_NETWORK) {
        dcc_set_error(voice_client->client, "voice heartbeat send failed");
    }
    dcc_voice_client_websocket_clear_current(voice_client, ws);
    dcc_ws_destroy(ws);
    return status;
}

dcc_status_t dcc_voice_client_run_websocket(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    uint8_t resume
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
    return dcc_voice_client_run_websocket_url(voice_client, self_user_id, url, resume);
}
