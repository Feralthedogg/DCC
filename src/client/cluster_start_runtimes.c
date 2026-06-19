#include "internal/client/dcc_cluster_internal.h"

void dcc_cluster_shutdown_shard_runtimes(dcc_cluster_t *cluster) {
    if (cluster == NULL) {
        return;
    }
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        if (cluster->shards[i] != NULL) {
            dcc_runtime_shutdown(&cluster->shards[i]->runtime);
        }
    }
}

dcc_status_t dcc_cluster_init_shard_runtimes(dcc_cluster_t *cluster) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        dcc_status_t status = dcc_runtime_init(&cluster->shards[i]->runtime);
        if (status != DCC_OK) {
            dcc_cluster_health_lock(cluster);
            cluster->health[i].status = DCC_CLUSTER_SHARD_ERROR;
            dcc_cluster_copy_text(
                cluster->health[i].last_error,
                sizeof(cluster->health[i].last_error),
                dcc_status_string(status)
            );
            dcc_cluster_health_unlock(cluster);
            dcc_cluster_shutdown_shard_runtimes(cluster);
            return status;
        }
    }
    return DCC_OK;
}
