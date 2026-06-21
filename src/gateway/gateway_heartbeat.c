#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_heartbeat_internal.h"
#include "internal/gateway/dcc_gateway_timing_internal.h"
#include "internal/ws/dcc_ws.h"

#include <llam/runtime.h>

#include <stdio.h>
#include <stdlib.h>

void dcc_gateway_heartbeat_state_release(dcc_gateway_heartbeat_state_t *state) {
    if (state == NULL) {
        return;
    }
    if (atomic_fetch_sub_explicit(&state->refs, 1U, memory_order_acq_rel) != 1U) {
        return;
    }
    dcc_ws_destroy(state->ws);
    free(state);
}

static dcc_status_t dcc_gateway_send_heartbeat_state(dcc_gateway_heartbeat_state_t *state) {
    char heartbeat[128];
    int n;
    if (atomic_load_explicit(&state->has_seq, memory_order_acquire)) {
        unsigned long long seq = atomic_load_explicit(&state->seq, memory_order_acquire);
        n = snprintf(
            heartbeat,
            sizeof(heartbeat),
            "{\"op\":1,\"d\":%llu}",
            seq
        );
    } else {
        n = snprintf(heartbeat, sizeof(heartbeat), "{\"op\":1,\"d\":null}");
    }
    if (n <= 0 || (size_t)n >= sizeof(heartbeat)) {
        return DCC_ERR_NOMEM;
    }
    return dcc_ws_send_text(state->ws, heartbeat, (size_t)n);
}

void dcc_gateway_heartbeat_task(void *arg) {
    dcc_gateway_heartbeat_state_t *state = (dcc_gateway_heartbeat_state_t *)arg;
    uint32_t interval_ms = state->interval_ms == 0 ? 45000U : state->interval_ms;
    uint32_t delay_ms = dcc_gateway_now_jitter(interval_ms, state->jitter_seed);

    while (!atomic_load_explicit(&state->stop, memory_order_acquire)) {
        uint32_t remaining = delay_ms;
        while (remaining > 0 &&
               !atomic_load_explicit(&state->stop, memory_order_acquire)) {
            uint32_t slice = remaining > 100U ? 100U : remaining;
            (void)llam_sleep_ns((uint64_t)slice * 1000000ULL);
            remaining -= slice;
        }
        if (atomic_load_explicit(&state->stop, memory_order_acquire)) {
            break;
        }
        if (atomic_load_explicit(&state->waiting_ack, memory_order_acquire)) {
            atomic_store_explicit(&state->failed, true, memory_order_release);
            dcc_ws_abort(state->ws);
            break;
        }
        /* Mark before send so a fast ACK cannot be overwritten after send returns. */
        atomic_store_explicit(&state->waiting_ack, true, memory_order_release);
        if (dcc_gateway_send_heartbeat_state(state) != DCC_OK) {
            atomic_store_explicit(&state->failed, true, memory_order_release);
            dcc_ws_abort(state->ws);
            break;
        }
        delay_ms = interval_ms;
    }
    dcc_gateway_heartbeat_state_release(state);
}
