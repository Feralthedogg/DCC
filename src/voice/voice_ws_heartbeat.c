#include "internal/voice/dcc_voice_ws_internal.h"

void dcc_voice_ws_heartbeat_task(void *arg) {
    dcc_voice_ws_session_t *session = (dcc_voice_ws_session_t *)arg;
    if (session == NULL || session->voice_client == NULL || session->ws == NULL) {
        return;
    }

    uint32_t interval_ms = session->voice_client->heartbeat_interval_ms;
    if (interval_ms == 0) {
        interval_ms = 45000U;
    }
    uint32_t delay_ms = (interval_ms / 4U) * 3U;
    if (delay_ms == 0) {
        delay_ms = 1U;
    }

    while (!atomic_load_explicit(&session->stop_heartbeat, memory_order_acquire)) {
        uint32_t remaining = delay_ms;
        while (remaining > 0 && !atomic_load_explicit(&session->stop_heartbeat, memory_order_acquire)) {
            uint32_t slice = remaining > 100U ? 100U : remaining;
            (void)llam_sleep_ns((uint64_t)slice * UINT64_C(1000000));
            remaining -= slice;
        }
        if (atomic_load_explicit(&session->stop_heartbeat, memory_order_acquire)) {
            break;
        }

        char heartbeat[160];
        size_t heartbeat_len = 0;
        dcc_status_t status = dcc_voice_client_build_heartbeat_payload(
            session->voice_client,
            llam_now_ns(),
            heartbeat,
            sizeof(heartbeat),
            &heartbeat_len
        );
        if (status != DCC_OK ||
            dcc_ws_send_text(session->ws, heartbeat, heartbeat_len) != DCC_OK) {
            atomic_store_explicit(&session->heartbeat_failed, true, memory_order_release);
            dcc_ws_abort(session->ws);
            break;
        }
        delay_ms = interval_ms;
    }
}
