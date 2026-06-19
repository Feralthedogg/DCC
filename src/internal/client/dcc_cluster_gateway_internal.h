#ifndef DCC_CLUSTER_GATEWAY_INTERNAL_H
#define DCC_CLUSTER_GATEWAY_INTERNAL_H

#include "internal/client/dcc_cluster_state_internal.h"

#include <dcc/cluster.h>

#include <stdatomic.h>

static inline int dcc_cluster_gateway_admission_open(const dcc_cluster_t *cluster) {
    return cluster != NULL &&
        atomic_load_explicit(&cluster->gateway_admission_open, memory_order_acquire);
}

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_cluster_copy_gateway_info_from_first(dcc_cluster_t *cluster);
int dcc_cluster_should_fetch_gateway_info(const dcc_cluster_t *cluster);

#ifdef __cplusplus
}
#endif

#endif
