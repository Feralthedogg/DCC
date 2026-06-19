#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdint.h>

static void dcc_hot_reload_sleep_ms(uint32_t ms) {
    uint32_t remaining = ms == 0 ? 1U : ms;
    while (remaining > 0) {
        uint32_t slice = remaining > 100U ? 100U : remaining;
        (void)llam_sleep_ns((uint64_t)slice * UINT64_C(1000000));
        remaining -= slice;
    }
}

void dcc_hot_reload_watch_task(void *arg) {
    dcc_hot_reload_t *hot_reload = (dcc_hot_reload_t *)arg;
    if (hot_reload == NULL) {
        return;
    }

    while (!atomic_load_explicit(&hot_reload->watch_stop, memory_order_acquire)) {
        dcc_status_t status = dcc_hot_reload_poll(hot_reload);
        if (status != DCC_OK) {
            dcc_hot_reload_log(hot_reload, DCC_LOG_WARN, dcc_hot_reload_last_error(hot_reload));
        }
        uint32_t interval = hot_reload->poll_interval_ms == 0 ? 250U : hot_reload->poll_interval_ms;
        uint32_t slept = 0;
        while (slept < interval && !atomic_load_explicit(&hot_reload->watch_stop, memory_order_acquire)) {
            uint32_t slice = interval - slept;
            if (slice > 100U) {
                slice = 100U;
            }
            dcc_hot_reload_sleep_ms(slice);
            slept += slice;
        }
    }

    dcc_hot_reload_lock(hot_reload);
    atomic_store_explicit(&hot_reload->watch_running, false, memory_order_release);
    dcc_hot_reload_broadcast(hot_reload);
    dcc_hot_reload_unlock(hot_reload);
}
