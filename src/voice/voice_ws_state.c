#include "internal/voice/dcc_voice_ws_internal.h"

uint8_t dcc_voice_client_websocket_stop_requested(const dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return 1U;
    }
    return atomic_load_explicit(&voice_client->websocket_task_running, memory_order_acquire) &&
            atomic_load_explicit(&voice_client->websocket_task_stop, memory_order_acquire)
        ? 1U
        : 0U;
}

static void dcc_voice_client_websocket_lock(dcc_voice_client_t *voice_client) {
    while (atomic_flag_test_and_set_explicit(&voice_client->websocket_lock, memory_order_acquire)) {
        llam_yield();
    }
}

static void dcc_voice_client_websocket_unlock(dcc_voice_client_t *voice_client) {
    atomic_flag_clear_explicit(&voice_client->websocket_lock, memory_order_release);
}

void dcc_voice_client_websocket_bind_current(dcc_voice_client_t *voice_client, dcc_ws_t *ws) {
    if (voice_client == NULL || ws == NULL) {
        return;
    }

    dcc_voice_client_websocket_lock(voice_client);
    voice_client->websocket_current_ws = ws;
    uint8_t stop_requested = dcc_voice_client_websocket_stop_requested(voice_client);
    dcc_voice_client_websocket_unlock(voice_client);

    if (stop_requested) {
        dcc_ws_abort(ws);
    }
}

void dcc_voice_client_websocket_clear_current(dcc_voice_client_t *voice_client, dcc_ws_t *ws) {
    if (voice_client == NULL || ws == NULL) {
        return;
    }

    dcc_voice_client_websocket_lock(voice_client);
    if (voice_client->websocket_current_ws == ws) {
        voice_client->websocket_current_ws = NULL;
    }
    dcc_voice_client_websocket_unlock(voice_client);
}

void dcc_voice_client_websocket_reset_current(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return;
    }

    dcc_voice_client_websocket_lock(voice_client);
    voice_client->websocket_current_ws = NULL;
    dcc_voice_client_websocket_unlock(voice_client);
}

dcc_status_t dcc_voice_client_websocket_retain_current(dcc_voice_client_t *voice_client, dcc_ws_t **out) {
    if (voice_client == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_voice_client_websocket_lock(voice_client);
    dcc_ws_t *ws = (dcc_ws_t *)voice_client->websocket_current_ws;
    dcc_status_t status = ws != NULL ? dcc_ws_retain(ws) : DCC_ERR_STATE;
    dcc_voice_client_websocket_unlock(voice_client);

    *out = status == DCC_OK ? ws : NULL;
    return status;
}

void dcc_voice_client_websocket_abort_current(dcc_voice_client_t *voice_client) {
    dcc_ws_t *ws = NULL;
    dcc_status_t status = dcc_voice_client_websocket_retain_current(voice_client, &ws);
    if (status == DCC_OK) {
        dcc_ws_abort(ws);
        dcc_ws_destroy(ws);
    }
}
