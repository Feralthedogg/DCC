#ifndef DCC_HOT_RELOAD_SYNC_INTERNAL_H
#define DCC_HOT_RELOAD_SYNC_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_state_internal.h"

#include <dcc/hot_reload.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void dcc_hot_reload_lock(dcc_hot_reload_t *hot_reload) {
#if defined(_WIN32)
    EnterCriticalSection(&hot_reload->mutex);
#else
    (void)pthread_mutex_lock(&hot_reload->mutex);
#endif
}

static inline void dcc_hot_reload_unlock(dcc_hot_reload_t *hot_reload) {
#if defined(_WIN32)
    LeaveCriticalSection(&hot_reload->mutex);
#else
    (void)pthread_mutex_unlock(&hot_reload->mutex);
#endif
}

void dcc_hot_reload_broadcast(dcc_hot_reload_t *hot_reload);
void dcc_hot_reload_wait_locked(dcc_hot_reload_t *hot_reload);
dcc_status_t dcc_hot_reload_wait_timeout_locked(dcc_hot_reload_t *hot_reload, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
