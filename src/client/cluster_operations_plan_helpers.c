#include "internal/client/dcc_cluster_internal.h"

#include <limits.h>
#include <string.h>

dcc_status_t dcc_cluster_normalize_rolling_options(
    const dcc_cluster_t *cluster,
    const dcc_cluster_rolling_reconnect_options_t *options,
    uint32_t *out_first,
    uint32_t *out_count,
    uint32_t *out_batch_size
) {
    if (cluster == NULL || options == NULL || options->size < sizeof(*options) ||
        out_first == NULL || out_count == NULL || out_batch_size == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    uint32_t first = options->first_index;
    uint32_t count = options->shard_count;
    if (first >= cluster->shard_count) {
        return DCC_ERR_INVALID_ARG;
    }
    if (count == 0) {
        count = cluster->shard_count - first;
    }
    if (count == 0 || count > cluster->shard_count - first) {
        return DCC_ERR_INVALID_ARG;
    }

    uint32_t batch_size = options->batch_size != 0 ? options->batch_size : 1U;
    if (batch_size > count) {
        batch_size = count;
    }

    *out_first = first;
    *out_count = count;
    *out_batch_size = batch_size;
    return DCC_OK;
}

dcc_status_t dcc_cluster_store_rolling_plan(
    dcc_cluster_t *cluster,
    const dcc_cluster_rolling_reconnect_options_t *options,
    uint32_t first,
    uint32_t count,
    uint32_t batch_size
) {
    dcc_cluster_health_lock(cluster);
    if (cluster->operation.active) {
        dcc_cluster_health_unlock(cluster);
        return DCC_ERR_STATE;
    }

    uint64_t generation = cluster->operation.generation;
    if (generation != UINT64_MAX) {
        generation++;
    }
    dcc_cluster_operation_status_t *operation = &cluster->operation;
    memset(operation, 0, sizeof(*operation));
    operation->size = sizeof(*operation);
    operation->type = DCC_CLUSTER_OPERATION_ROLLING_RECONNECT;
    operation->generation = generation;
    operation->resume = options->resume != 0 ? 1U : 0U;
    operation->first_index = first;
    operation->shard_count = count;
    operation->batch_size = batch_size;
    operation->delay_ms = options->delay_ms;
    operation->next_index = first;
    operation->total_batches = dcc_cluster_rolling_total_batches(count, batch_size);
    operation->last_status = DCC_OK;
    dcc_cluster_health_unlock(cluster);
    return DCC_OK;
}
