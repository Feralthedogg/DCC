#include "internal/hot_reload/dcc_hot_reload_internal.h"

dcc_status_t dcc_hot_reload_start(dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (hot_reload->client == NULL ||
        !hot_reload->client->runtime.initialized ||
        !atomic_load_explicit(&hot_reload->client->started, memory_order_acquire)) {
        dcc_hot_reload_lock(hot_reload);
        dcc_hot_reload_set_error(hot_reload, DCC_ERR_STATE, "hot reload watcher requires a started client");
        dcc_hot_reload_broadcast(hot_reload);
        dcc_hot_reload_unlock(hot_reload);
        return DCC_ERR_STATE;
    }

    bool expected = false;
    if (!atomic_compare_exchange_strong_explicit(
            &hot_reload->watch_running,
            &expected,
            true,
            memory_order_acq_rel,
            memory_order_acquire
        )) {
        return DCC_ERR_STATE;
    }
    atomic_store_explicit(&hot_reload->watch_stop, false, memory_order_release);

    dcc_status_t status = dcc_runtime_spawn(&hot_reload->client->runtime, dcc_hot_reload_watch_task, hot_reload);
    if (status != DCC_OK) {
        atomic_store_explicit(&hot_reload->watch_running, false, memory_order_release);
        dcc_hot_reload_lock(hot_reload);
        dcc_hot_reload_set_error(hot_reload, status, "failed to spawn hot reload watcher");
        dcc_hot_reload_broadcast(hot_reload);
        dcc_hot_reload_unlock(hot_reload);
        return status;
    }
    if (hot_reload->backend == DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER) {
        status = dcc_hot_reload_worker_supervisor_start(hot_reload);
        if (status != DCC_OK) {
            atomic_store_explicit(&hot_reload->watch_stop, true, memory_order_release);
            dcc_hot_reload_lock(hot_reload);
            while (atomic_load_explicit(&hot_reload->watch_running, memory_order_acquire)) {
                dcc_hot_reload_wait_locked(hot_reload);
            }
            dcc_hot_reload_set_error(hot_reload, status, "failed to spawn hot reload worker supervisor");
            dcc_hot_reload_broadcast(hot_reload);
            dcc_hot_reload_unlock(hot_reload);
        }
    }
    return status;
}

void dcc_hot_reload_request_stop(dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return;
    }
    atomic_store_explicit(&hot_reload->worker_supervisor_stop, true, memory_order_release);
    atomic_store_explicit(&hot_reload->watch_stop, true, memory_order_release);
    dcc_hot_reload_lock(hot_reload);
    dcc_hot_reload_broadcast(hot_reload);
    dcc_hot_reload_unlock(hot_reload);
}

dcc_status_t dcc_hot_reload_stop(dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_hot_reload_request_stop(hot_reload);
    dcc_hot_reload_lock(hot_reload);
    while (atomic_load_explicit(&hot_reload->watch_running, memory_order_acquire) ||
           atomic_load_explicit(&hot_reload->worker_supervisor_running, memory_order_acquire)) {
        dcc_hot_reload_wait_locked(hot_reload);
    }
    dcc_hot_reload_unlock(hot_reload);
    return DCC_OK;
}
