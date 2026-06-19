#ifndef DCC_CACHE_LOCK_INTERNAL_H
#define DCC_CACHE_LOCK_INTERNAL_H

#include "internal/cache/dcc_cache_state_internal.h"

#include <stdatomic.h>

static inline void dcc_cache_lock(dcc_cache_t *cache) {
    while (atomic_flag_test_and_set_explicit(&cache->lock, memory_order_acquire)) {
    }
}

static inline void dcc_cache_unlock(dcc_cache_t *cache) {
    atomic_flag_clear_explicit(&cache->lock, memory_order_release);
}

#endif
