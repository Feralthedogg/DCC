#include "internal/client/dcc_cluster_internal.h"

#include <stdatomic.h>

dcc_status_t dcc_cluster_start(dcc_cluster_t *cluster) {
    if (cluster == NULL || cluster->shard_count == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (cluster->started) {
        return DCC_ERR_STATE;
    }
    if (!dcc_cluster_gateway_admission_open(cluster)) {
        return DCC_ERR_STATE;
    }

    atomic_store_explicit(&cluster->supervisor_stop, false, memory_order_release);
    int supervisor_enabled =
        cluster->restart_policy.max_restarts != 0 || cluster->supervisor_policy != NULL;
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        cluster->shards[i]->gateway_stop_runtime_on_error = supervisor_enabled ? 0U : 1U;
    }

    dcc_cluster_health_lock(cluster);
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        cluster->health[i].status = DCC_CLUSTER_SHARD_STARTING;
        cluster->health[i].starts++;
    }
    dcc_cluster_health_unlock(cluster);

    dcc_status_t runtime_status = dcc_cluster_init_shard_runtimes(cluster);
    if (runtime_status != DCC_OK) {
        return runtime_status;
    }

    if (dcc_cluster_should_fetch_gateway_info(cluster)) {
        dcc_status_t status = dcc_cluster_fetch_gateway_info(cluster);
        if (status != DCC_OK) {
            dcc_cluster_health_lock(cluster);
            cluster->health[0].status = DCC_CLUSTER_SHARD_ERROR;
            dcc_cluster_copy_text(cluster->health[0].last_error, sizeof(cluster->health[0].last_error), dcc_status_string(status));
            dcc_cluster_health_unlock(cluster);
            dcc_cluster_shutdown_shard_runtimes(cluster);
            return status;
        }
    }

    dcc_status_t identify_budget_status = dcc_cluster_check_initial_identify_budget(cluster);
    if (identify_budget_status != DCC_OK) {
        dcc_cluster_shutdown_shard_runtimes(cluster);
        return identify_budget_status;
    }

    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        dcc_status_t status = dcc_client_start(cluster->shards[i]);
        if (status != DCC_OK) {
            dcc_cluster_health_lock(cluster);
            cluster->health[i].status = DCC_CLUSTER_SHARD_ERROR;
            dcc_cluster_copy_text(cluster->health[i].last_error, sizeof(cluster->health[i].last_error), dcc_status_string(status));
            dcc_cluster_health_unlock(cluster);
            cluster->started_count = i;
            (void)dcc_cluster_stop(cluster);
            return status;
        }
        cluster->started_count = i + 1U;
    }
    cluster->started = true;
    if (supervisor_enabled) {
        dcc_status_t status =
            dcc_runtime_spawn(&cluster->shards[0]->runtime, dcc_cluster_supervisor_task, cluster);
        if (status != DCC_OK) {
            dcc_cluster_health_lock(cluster);
            dcc_cluster_copy_text(
                cluster->health[0].last_error,
                sizeof(cluster->health[0].last_error),
                dcc_status_string(status)
            );
            cluster->health[0].status = DCC_CLUSTER_SHARD_ERROR;
            dcc_cluster_health_unlock(cluster);
            (void)dcc_cluster_stop(cluster);
            return status;
        }
    }
    return DCC_OK;
}
