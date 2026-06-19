#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdbool.h>

static void dcc_hot_reload_worker_supervisor_task(void *arg) {
    dcc_hot_reload_t *hot_reload = (dcc_hot_reload_t *)arg;
    if (hot_reload == NULL) {
        return;
    }

    while (!atomic_load_explicit(&hot_reload->worker_supervisor_stop, memory_order_acquire)) {
        dcc_hot_reload_worker_supervise_once(hot_reload);
        uint32_t interval = hot_reload->worker_health_timeout_ms / 2U;
        if (interval == 0U || interval > 250U) {
            interval = 250U;
        }
        uint32_t slept = 0U;
        while (slept < interval &&
               !atomic_load_explicit(&hot_reload->worker_supervisor_stop, memory_order_acquire)) {
            uint32_t slice = interval - slept;
            if (slice > 100U) {
                slice = 100U;
            }
            dcc_hot_reload_worker_supervisor_sleep_ms(slice);
            slept += slice;
        }
    }

    dcc_hot_reload_lock(hot_reload);
    atomic_store_explicit(&hot_reload->worker_supervisor_running, false, memory_order_release);
    dcc_hot_reload_broadcast(hot_reload);
    dcc_hot_reload_unlock(hot_reload);
}

dcc_status_t dcc_hot_reload_worker_supervisor_start(dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    bool expected = false;
    if (!atomic_compare_exchange_strong_explicit(
            &hot_reload->worker_supervisor_running,
            &expected,
            true,
            memory_order_acq_rel,
            memory_order_acquire
        )) {
        return DCC_OK;
    }
    atomic_store_explicit(&hot_reload->worker_supervisor_stop, false, memory_order_release);

    dcc_status_t status =
        dcc_runtime_spawn(&hot_reload->client->runtime, dcc_hot_reload_worker_supervisor_task, hot_reload);
    if (status != DCC_OK) {
        atomic_store_explicit(&hot_reload->worker_supervisor_running, false, memory_order_release);
    }
    return status;
}

void dcc_hot_reload_worker_supervisor_stop(dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return;
    }
    atomic_store_explicit(&hot_reload->worker_supervisor_stop, true, memory_order_release);
    dcc_hot_reload_lock(hot_reload);
    while (atomic_load_explicit(&hot_reload->worker_supervisor_running, memory_order_acquire)) {
        dcc_hot_reload_wait_locked(hot_reload);
    }
    dcc_hot_reload_unlock(hot_reload);
}
