#include "internal/voice/dcc_voice_internal.h"

void dcc_voice_receive_jitter_timer_cancel(dcc_voice_client_t *voice_client) {
    (void)voice_client;
}

static int dcc_voice_jitter_timer_sleep(uint64_t interval_ns, dcc_voice_client_t *voice_client) {
    uint64_t remaining_ns = interval_ns;
    while (remaining_ns > 0) {
        if (atomic_load_explicit(&voice_client->receive_jitter_timer_stop, memory_order_acquire)) {
            return 1;
        }

        uint64_t slice_ns = remaining_ns > UINT64_C(10000000) ? UINT64_C(10000000) : remaining_ns;
        if (llam_sleep_ns(slice_ns) != 0) {
            dcc_voice_sleep_ms(1);
        }
        remaining_ns -= slice_ns;
    }
    return atomic_load_explicit(&voice_client->receive_jitter_timer_stop, memory_order_acquire) ? 1 : 0;
}

void dcc_voice_receive_jitter_timer_task(void *arg) {
    dcc_voice_client_t *voice_client = (dcc_voice_client_t *)arg;

    if (voice_client == NULL) {
        return;
    }
    if (voice_client->receive_jitter_timer_interval_ms == 0) {
        goto done;
    }

    uint64_t interval_ns = (uint64_t)voice_client->receive_jitter_timer_interval_ms * UINT64_C(1000000);
    if (interval_ns == 0) {
        goto done;
    }

    while (!atomic_load_explicit(&voice_client->receive_jitter_timer_stop, memory_order_acquire)) {
        if (dcc_voice_jitter_timer_sleep(interval_ns, voice_client)) {
            break;
        }
        (void)dcc_voice_client_tick_receive_jitter(voice_client, dcc_voice_now_ms());
    }

done:
    atomic_store_explicit(&voice_client->receive_jitter_timer_completed, true, memory_order_release);
    atomic_store_explicit(&voice_client->receive_jitter_timer_running, false, memory_order_release);
}
