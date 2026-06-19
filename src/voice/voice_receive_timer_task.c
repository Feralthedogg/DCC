#include "internal/voice/dcc_voice_internal.h"

static void dcc_voice_jitter_timer_lock(dcc_voice_client_t *voice_client) {
    while (atomic_flag_test_and_set_explicit(
        &voice_client->receive_jitter_timer_lock,
        memory_order_acquire
    )) {
    }
}

static void dcc_voice_jitter_timer_unlock(dcc_voice_client_t *voice_client) {
    atomic_flag_clear_explicit(&voice_client->receive_jitter_timer_lock, memory_order_release);
}

void dcc_voice_receive_jitter_timer_cancel(dcc_voice_client_t *voice_client) {
    if (voice_client == NULL) {
        return;
    }

    dcc_voice_jitter_timer_lock(voice_client);
    uintptr_t handle =
        atomic_load_explicit(&voice_client->receive_jitter_timer_handle, memory_order_acquire);
    if (handle != (uintptr_t)0) {
        (void)llam_timer_cancel((llam_timer_t *)handle);
    }
    dcc_voice_jitter_timer_unlock(voice_client);
}

void dcc_voice_receive_jitter_timer_task(void *arg) {
    dcc_voice_client_t *voice_client = (dcc_voice_client_t *)arg;
    llam_timer_t *timer = NULL;

    if (voice_client == NULL) {
        return;
    }
    if (voice_client->receive_jitter_timer_interval_ms == 0) {
        goto done;
    }

    uint64_t interval_ns = (uint64_t)voice_client->receive_jitter_timer_interval_ms * UINT64_C(1000000);
    if (interval_ns == 0 || llam_timer_create(interval_ns, &timer) != 0 || timer == NULL) {
        goto done;
    }

    dcc_voice_jitter_timer_lock(voice_client);
    atomic_store_explicit(&voice_client->receive_jitter_timer_handle, (uintptr_t)timer, memory_order_release);
    dcc_voice_jitter_timer_unlock(voice_client);

    while (!atomic_load_explicit(&voice_client->receive_jitter_timer_stop, memory_order_acquire)) {
        uint64_t ticks = 0;
        if (llam_timer_wait(timer, &ticks) != 0) {
            break;
        }
        if (atomic_load_explicit(&voice_client->receive_jitter_timer_stop, memory_order_acquire)) {
            break;
        }
        (void)dcc_voice_client_tick_receive_jitter(voice_client, dcc_voice_now_ms());
    }

    dcc_voice_jitter_timer_lock(voice_client);
    atomic_store_explicit(&voice_client->receive_jitter_timer_handle, (uintptr_t)0, memory_order_release);
    (void)llam_timer_destroy(timer);
    dcc_voice_jitter_timer_unlock(voice_client);

done:
    atomic_store_explicit(&voice_client->receive_jitter_timer_completed, true, memory_order_release);
    atomic_store_explicit(&voice_client->receive_jitter_timer_running, false, memory_order_release);
}
