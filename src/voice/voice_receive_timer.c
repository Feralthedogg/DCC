#include "internal/voice/dcc_voice_internal.h"

dcc_status_t dcc_voice_client_start_receive_jitter_timer(
    dcc_voice_client_t *voice_client,
    uint32_t interval_ms
) {
    if (voice_client == NULL || interval_ms == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_client_t *client = voice_client->client;
    if (client == NULL ||
        !client->runtime.initialized ||
        !atomic_load_explicit(&client->started, memory_order_acquire) ||
        llam_current_task() == NULL) {
        return DCC_ERR_STATE;
    }

    bool expected = false;
    if (!atomic_compare_exchange_strong_explicit(
            &voice_client->receive_jitter_timer_running,
            &expected,
            true,
            memory_order_acq_rel,
            memory_order_acquire
        )) {
        return DCC_ERR_STATE;
    }

    voice_client->receive_jitter_timer_interval_ms = interval_ms;
    atomic_store_explicit(&voice_client->receive_jitter_timer_stop, false, memory_order_release);
    atomic_store_explicit(&voice_client->receive_jitter_timer_completed, false, memory_order_release);
    atomic_store_explicit(&voice_client->receive_jitter_timer_handle, (uintptr_t)0, memory_order_release);

    dcc_status_t status =
        dcc_runtime_spawn(&client->runtime, dcc_voice_receive_jitter_timer_task, voice_client);
    if (status != DCC_OK) {
        atomic_store_explicit(&voice_client->receive_jitter_timer_completed, true, memory_order_release);
        atomic_store_explicit(&voice_client->receive_jitter_timer_running, false, memory_order_release);
        return status;
    }

    return DCC_OK;
}

dcc_status_t dcc_voice_client_stop_receive_jitter_timer(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (!atomic_load_explicit(&voice_client->receive_jitter_timer_running, memory_order_acquire)) {
        return DCC_OK;
    }

    atomic_store_explicit(&voice_client->receive_jitter_timer_stop, true, memory_order_release);
    dcc_voice_receive_jitter_timer_cancel(voice_client);

    for (uint32_t i = 0; i < 1000U; ++i) {
        if (atomic_load_explicit(&voice_client->receive_jitter_timer_completed, memory_order_acquire) ||
            !atomic_load_explicit(&voice_client->receive_jitter_timer_running, memory_order_acquire)) {
            return DCC_OK;
        }

        dcc_voice_receive_jitter_timer_cancel(voice_client);
        dcc_voice_sleep_ms(1);
    }

    return DCC_ERR_TIMEOUT;
}
