#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_reset_shard_counters(dcc_cluster_t *cluster, uint32_t index) {
    if (cluster == NULL || index >= cluster->shard_count) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_health_lock(cluster);
    dcc_cluster_reset_info_counters(&cluster->health[index]);
    dcc_cluster_health_unlock(cluster);
    return DCC_OK;
}

dcc_status_t dcc_cluster_reset_all_counters(dcc_cluster_t *cluster) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_health_lock(cluster);
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        dcc_cluster_reset_info_counters(&cluster->health[i]);
    }
    dcc_cluster_health_unlock(cluster);
    return DCC_OK;
}

static void dcc_cluster_rearm_info_restart(dcc_cluster_t *cluster, dcc_cluster_shard_info_t *info) {
    info->restart_budget_used = 0;
    if (info->status == DCC_CLUSTER_SHARD_ERROR && info->restart_suppressed != 0) {
        info->status = cluster->started ? DCC_CLUSTER_SHARD_STOPPED : DCC_CLUSTER_SHARD_CREATED;
        info->last_error[0] = '\0';
    }
}

dcc_status_t dcc_cluster_rearm_shard_restart(dcc_cluster_t *cluster, uint32_t index) {
    if (cluster == NULL || index >= cluster->shard_count) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_health_lock(cluster);
    dcc_cluster_rearm_info_restart(cluster, &cluster->health[index]);
    dcc_cluster_health_unlock(cluster);
    return DCC_OK;
}

dcc_status_t dcc_cluster_rearm_all_shard_restarts(dcc_cluster_t *cluster) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_health_lock(cluster);
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        dcc_cluster_rearm_info_restart(cluster, &cluster->health[i]);
    }
    dcc_cluster_health_unlock(cluster);
    return DCC_OK;
}
