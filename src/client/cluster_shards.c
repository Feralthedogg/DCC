#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_drain_shard(dcc_cluster_t *cluster, uint32_t index) {
    if (cluster == NULL || index >= cluster->shard_count) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!cluster->started) {
        return DCC_ERR_STATE;
    }

    dcc_cluster_health_lock(cluster);
    cluster->health[index].status = DCC_CLUSTER_SHARD_STOPPING;
    dcc_cluster_health_unlock(cluster);

    dcc_status_t status = dcc_client_gateway_stop(cluster->shards[index]);
    if (status != DCC_OK) {
        dcc_cluster_health_lock(cluster);
        cluster->health[index].status = DCC_CLUSTER_SHARD_ERROR;
        dcc_cluster_copy_text(cluster->health[index].last_error, sizeof(cluster->health[index].last_error), dcc_status_string(status));
        dcc_cluster_health_unlock(cluster);
    }
    return status;
}

dcc_status_t dcc_cluster_resume_shard(dcc_cluster_t *cluster, uint32_t index) {
    if (cluster == NULL || index >= cluster->shard_count) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!cluster->started) {
        return DCC_ERR_STATE;
    }
    if (!dcc_cluster_gateway_admission_open(cluster)) {
        return DCC_ERR_STATE;
    }

    dcc_cluster_health_lock(cluster);
    cluster->health[index].status = DCC_CLUSTER_SHARD_STARTING;
    cluster->health[index].starts++;
    dcc_cluster_health_unlock(cluster);

    dcc_status_t status = dcc_client_gateway_start(cluster->shards[index]);
    if (status != DCC_OK) {
        dcc_cluster_health_lock(cluster);
        cluster->health[index].status = DCC_CLUSTER_SHARD_ERROR;
        dcc_cluster_copy_text(cluster->health[index].last_error, sizeof(cluster->health[index].last_error), dcc_status_string(status));
        dcc_cluster_health_unlock(cluster);
    }
    return status;
}

dcc_status_t dcc_cluster_reconnect_shard(dcc_cluster_t *cluster, uint32_t index, uint8_t resume) {
    if (cluster == NULL || index >= cluster->shard_count) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!cluster->started) {
        return DCC_ERR_STATE;
    }
    if (!dcc_cluster_gateway_admission_open(cluster)) {
        return DCC_ERR_STATE;
    }

    dcc_cluster_health_lock(cluster);
    cluster->health[index].status = DCC_CLUSTER_SHARD_STARTING;
    cluster->health[index].restart_attempts++;
    dcc_cluster_health_unlock(cluster);

    dcc_status_t status = dcc_client_gateway_reconnect(cluster->shards[index], resume);
    if (status != DCC_OK) {
        dcc_cluster_health_lock(cluster);
        cluster->health[index].status = DCC_CLUSTER_SHARD_ERROR;
        dcc_cluster_copy_text(cluster->health[index].last_error, sizeof(cluster->health[index].last_error), dcc_status_string(status));
        dcc_cluster_health_unlock(cluster);
    }
    return status;
}

dcc_status_t dcc_cluster_reconnect_all(dcc_cluster_t *cluster, uint8_t resume) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!cluster->started) {
        return DCC_ERR_STATE;
    }
    if (!dcc_cluster_gateway_admission_open(cluster)) {
        return DCC_ERR_STATE;
    }

    dcc_status_t first_error = DCC_OK;
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        dcc_status_t status = dcc_cluster_reconnect_shard(cluster, i, resume);
        if (first_error == DCC_OK && status != DCC_OK) {
            first_error = status;
        }
    }
    return first_error;
}
