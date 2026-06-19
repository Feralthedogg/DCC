#include "internal/client/dcc_cluster_internal.h"

#include <stdatomic.h>

dcc_status_t dcc_cluster_stop(dcc_cluster_t *cluster) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!cluster->started && cluster->started_count == 0) {
        return DCC_OK;
    }
    atomic_store_explicit(&cluster->supervisor_stop, true, memory_order_release);

    dcc_status_t first_error = DCC_OK;
    uint32_t limit = cluster->started_count != 0 ? cluster->started_count : cluster->shard_count;
    dcc_cluster_health_lock(cluster);
    for (uint32_t i = 0; i < limit; ++i) {
        if (cluster->health[i].status != DCC_CLUSTER_SHARD_STOPPED &&
            cluster->health[i].status != DCC_CLUSTER_SHARD_ERROR) {
            cluster->health[i].status = DCC_CLUSTER_SHARD_STOPPING;
        }
    }
    dcc_cluster_health_unlock(cluster);
    for (uint32_t i = 0; i < limit; ++i) {
        dcc_status_t status = dcc_client_stop(cluster->shards[i]);
        if (first_error == DCC_OK && status != DCC_OK) {
            first_error = status;
        }
    }
    return first_error;
}

dcc_status_t dcc_cluster_wait(dcc_cluster_t *cluster) {
    if (cluster == NULL || cluster->shard_count == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!cluster->started) {
        return DCC_ERR_STATE;
    }

    dcc_status_t status = dcc_runtime_run(&cluster->shards[0]->runtime);
    dcc_cluster_health_lock(cluster);
    for (uint32_t i = 0; i < cluster->started_count; ++i) {
        atomic_store(&cluster->shards[i]->started, false);
        if (cluster->health[i].status != DCC_CLUSTER_SHARD_ERROR) {
            cluster->health[i].status = DCC_CLUSTER_SHARD_STOPPED;
        }
    }
    dcc_cluster_health_unlock(cluster);
    cluster->started = false;
    cluster->started_count = 0;
    return status == DCC_ERR_CANCELED ? DCC_OK : status;
}
