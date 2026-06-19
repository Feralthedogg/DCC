#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdlib.h>

void dcc_hot_reload_worker_process_io_init(dcc_hot_reload_worker_process_t *worker) {
    if (worker == NULL || worker->io_lock_initialized) {
        return;
    }
#if defined(_WIN32)
    InitializeCriticalSection(&worker->io_lock);
#else
    (void)pthread_mutex_init(&worker->io_lock, NULL);
#endif
    worker->io_lock_initialized = 1U;
}

void dcc_hot_reload_worker_process_io_deinit(dcc_hot_reload_worker_process_t *worker) {
    if (worker == NULL || !worker->io_lock_initialized) {
        return;
    }
#if defined(_WIN32)
    DeleteCriticalSection(&worker->io_lock);
#else
    (void)pthread_mutex_destroy(&worker->io_lock);
#endif
    worker->io_lock_initialized = 0U;
}

void dcc_hot_reload_worker_process_io_lock(dcc_hot_reload_worker_process_t *worker) {
    if (worker == NULL || !worker->io_lock_initialized) {
        return;
    }
#if defined(_WIN32)
    EnterCriticalSection(&worker->io_lock);
#else
    (void)pthread_mutex_lock(&worker->io_lock);
#endif
}

void dcc_hot_reload_worker_process_io_unlock(dcc_hot_reload_worker_process_t *worker) {
    if (worker == NULL || !worker->io_lock_initialized) {
        return;
    }
#if defined(_WIN32)
    LeaveCriticalSection(&worker->io_lock);
#else
    (void)pthread_mutex_unlock(&worker->io_lock);
#endif
}

uint8_t dcc_hot_reload_worker_loaded(const dcc_hot_reload_t *hot_reload) {
    return hot_reload != NULL &&
           hot_reload->active_worker != NULL &&
           hot_reload->active_worker->loaded ? 1U : 0U;
}

void dcc_hot_reload_worker_destroy_all(dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return;
    }
    dcc_hot_reload_worker_process_stop(hot_reload->active_worker, hot_reload->worker_drain_timeout_ms);
    dcc_hot_reload_worker_process_stop(hot_reload->last_good_worker, hot_reload->worker_drain_timeout_ms);
    dcc_hot_reload_worker_process_stop(hot_reload->candidate_worker, hot_reload->worker_drain_timeout_ms);
    free(hot_reload->active_worker);
    free(hot_reload->last_good_worker);
    free(hot_reload->candidate_worker);
    hot_reload->active_worker = NULL;
    hot_reload->last_good_worker = NULL;
    hot_reload->candidate_worker = NULL;
    dcc_hot_reload_canary_stop(&hot_reload->worker_canary);
}
