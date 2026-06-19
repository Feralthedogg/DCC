#include "internal/voice/dcc_voice_ws_internal.h"

dcc_status_t dcc_voice_client_start_websocket_loop(
    dcc_voice_client_t *voice_client,
    dcc_snowflake_t self_user_id,
    uint32_t reconnect_delay_ms
) {
    if (voice_client == NULL || self_user_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_client_t *client = voice_client->client;
    if (client == NULL ||
        !client->runtime.initialized ||
        !atomic_load_explicit(&client->started, memory_order_acquire) ||
        voice_client->guild_id == 0) {
        return DCC_ERR_STATE;
    }

    dcc_voice_client_websocket_reset_current(voice_client);

    bool expected = false;
    if (!atomic_compare_exchange_strong_explicit(
            &voice_client->websocket_task_running,
            &expected,
            true,
            memory_order_acq_rel,
            memory_order_acquire
        )) {
        return DCC_ERR_STATE;
    }

    voice_client->websocket_self_user_id = self_user_id;
    voice_client->websocket_reconnect_delay_ms = reconnect_delay_ms == 0 ? 1000U : reconnect_delay_ms;
    atomic_store_explicit(&voice_client->websocket_task_stop, false, memory_order_release);
    atomic_store_explicit(&voice_client->websocket_task_completed, false, memory_order_release);
    atomic_store_explicit(&voice_client->websocket_last_status, (int)DCC_OK, memory_order_release);

    dcc_status_t status =
        dcc_runtime_spawn(&client->runtime, dcc_voice_client_websocket_loop_task, voice_client);
    if (status != DCC_OK) {
        atomic_store_explicit(&voice_client->websocket_last_status, (int)status, memory_order_release);
        atomic_store_explicit(&voice_client->websocket_task_completed, true, memory_order_release);
        atomic_store_explicit(&voice_client->websocket_task_running, false, memory_order_release);
        return status;
    }

    return DCC_OK;
}

dcc_status_t dcc_voice_client_stop_websocket_loop(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (!atomic_load_explicit(&voice_client->websocket_task_running, memory_order_acquire)) {
        return DCC_OK;
    }

    atomic_store_explicit(&voice_client->websocket_task_stop, true, memory_order_release);
    dcc_voice_client_websocket_abort_current(voice_client);
    for (uint32_t i = 0; i < 1000U; ++i) {
        if (atomic_load_explicit(&voice_client->websocket_task_completed, memory_order_acquire) ||
            !atomic_load_explicit(&voice_client->websocket_task_running, memory_order_acquire)) {
            return DCC_OK;
        }
        dcc_voice_client_websocket_abort_current(voice_client);
        dcc_voice_sleep_ms(1);
    }

    return DCC_ERR_TIMEOUT;
}

uint8_t dcc_voice_client_websocket_loop_running(const dcc_voice_client_t *voice_client) {
    return voice_client != NULL &&
            atomic_load_explicit(&voice_client->websocket_task_running, memory_order_acquire)
        ? 1U
        : 0U;
}

dcc_status_t dcc_voice_client_websocket_loop_status(
    const dcc_voice_client_t *voice_client,
    dcc_status_t *out_status
) {
    if (voice_client == NULL || out_status == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_status =
        (dcc_status_t)atomic_load_explicit(&voice_client->websocket_last_status, memory_order_acquire);
    return DCC_OK;
}
