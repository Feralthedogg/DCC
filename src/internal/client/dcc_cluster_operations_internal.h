#ifndef DCC_CLUSTER_OPERATIONS_INTERNAL_H
#define DCC_CLUSTER_OPERATIONS_INTERNAL_H

#include <dcc/cluster.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_cluster_operation_reset(dcc_cluster_operation_status_t *operation, uint64_t generation);
uint32_t dcc_cluster_rolling_total_batches(uint32_t count, uint32_t batch_size);
dcc_status_t dcc_cluster_normalize_rolling_options(
    const dcc_cluster_t *cluster,
    const dcc_cluster_rolling_reconnect_options_t *options,
    uint32_t *out_first,
    uint32_t *out_count,
    uint32_t *out_batch_size
);
dcc_status_t dcc_cluster_store_rolling_plan(
    dcc_cluster_t *cluster,
    const dcc_cluster_rolling_reconnect_options_t *options,
    uint32_t first,
    uint32_t count,
    uint32_t batch_size
);

#ifdef __cplusplus
}
#endif

#endif
