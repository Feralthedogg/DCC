#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_heartbeat_internal.h"
#include "internal/gateway/dcc_gateway_timing_internal.h"
#include "internal/ws/dcc_ws.h"

#include <llam/runtime.h>

void dcc_gateway_heartbeat_task(void *arg) {
    dcc_gateway_session_t *session = (dcc_gateway_session_t *)arg;
    uint32_t interval_ms = session->heartbeat_interval_ms == 0 ? 45000U : session->heartbeat_interval_ms;
    uint32_t delay_ms = dcc_gateway_now_jitter(interval_ms, session->client->shard_id + 1U);

    while (!atomic_load_explicit(&session->stop_heartbeat, memory_order_acquire) &&
           !atomic_load_explicit(&session->client->stopping, memory_order_acquire)) {
        uint32_t remaining = delay_ms;
        while (remaining > 0 &&
               !atomic_load_explicit(&session->stop_heartbeat, memory_order_acquire) &&
               !atomic_load_explicit(&session->client->stopping, memory_order_acquire)) {
            uint32_t slice = remaining > 100U ? 100U : remaining;
            (void)llam_sleep_ns((uint64_t)slice * 1000000ULL);
            remaining -= slice;
        }
        if (atomic_load_explicit(&session->stop_heartbeat, memory_order_acquire) ||
            atomic_load_explicit(&session->client->stopping, memory_order_acquire)) {
            break;
        }
        if (atomic_load_explicit(&session->waiting_heartbeat_ack, memory_order_acquire)) {
            atomic_store_explicit(&session->heartbeat_failed, true, memory_order_release);
            dcc_set_error(session->client, "gateway heartbeat ACK timeout");
            dcc_ws_abort(session->ws);
            break;
        }
        if (dcc_gateway_send_heartbeat(session) != DCC_OK) {
            atomic_store_explicit(&session->heartbeat_failed, true, memory_order_release);
            dcc_set_error(session->client, "gateway heartbeat send failed");
            dcc_ws_abort(session->ws);
            break;
        }
        atomic_store_explicit(&session->waiting_heartbeat_ack, true, memory_order_release);
        delay_ms = interval_ms;
    }
}
