#include "internal/voice/dcc_voice_ws_internal.h"

static dcc_status_t dcc_voice_client_wait_websocket_ready(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    for (;;) {
        if (dcc_voice_client_websocket_stop_requested(voice_client)) {
            return DCC_ERR_CANCELED;
        }
        dcc_client_t *client = voice_client->client;
        if (client == NULL ||
            !client->runtime.initialized ||
            !atomic_load_explicit(&client->started, memory_order_acquire)) {
            return DCC_ERR_STATE;
        }
        if (atomic_load_explicit(&client->stopping, memory_order_acquire)) {
            return DCC_ERR_CANCELED;
        }
        if (dcc_voice_session_descriptor_ready(voice_client)) {
            return DCC_OK;
        }
        if (voice_client->guild_id == 0 ||
            voice_client->session_state == DCC_VOICE_SESSION_TERMINATED ||
            voice_client->session_state == DCC_VOICE_SESSION_DISCONNECTED) {
            return DCC_ERR_STATE;
        }
        dcc_voice_sleep_ms(1);
    }
}

void dcc_voice_client_websocket_loop_task(void *arg) {
    dcc_voice_client_t *voice_client = (dcc_voice_client_t *)arg;
    if (voice_client == NULL) {
        return;
    }

    dcc_status_t status = DCC_OK;
    for (;;) {
        status = dcc_voice_client_wait_websocket_ready(voice_client);
        if (status != DCC_OK) {
            break;
        }
        status = dcc_voice_client_run_websocket_loop(
            voice_client,
            voice_client->websocket_self_user_id,
            voice_client->websocket_reconnect_delay_ms
        );
        if (dcc_voice_client_websocket_stop_requested(voice_client) ||
            !voice_client->full_reconnect_requested) {
            break;
        }
        status = dcc_voice_client_request_full_reconnect(voice_client);
        if (status != DCC_OK) {
            break;
        }
    }
    if (dcc_voice_client_websocket_stop_requested(voice_client)) {
        status = DCC_ERR_CANCELED;
    }

    atomic_store_explicit(&voice_client->websocket_last_status, (int)status, memory_order_release);
    atomic_store_explicit(&voice_client->websocket_task_completed, true, memory_order_release);
    atomic_store_explicit(&voice_client->websocket_task_running, false, memory_order_release);
}
