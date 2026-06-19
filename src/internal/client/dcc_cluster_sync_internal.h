#ifndef DCC_CLUSTER_SYNC_INTERNAL_H
#define DCC_CLUSTER_SYNC_INTERNAL_H

#include "internal/client/dcc_cluster_state_internal.h"
#include "internal/runtime/dcc_runtime_internal.h"

#include <stdatomic.h>

static inline void dcc_cluster_health_lock(dcc_cluster_t *cluster) {
    while (atomic_flag_test_and_set_explicit(&cluster->health_lock, memory_order_acquire)) {
        llam_yield();
    }
}

static inline void dcc_cluster_health_unlock(dcc_cluster_t *cluster) {
    atomic_flag_clear_explicit(&cluster->health_lock, memory_order_release);
}

#endif
