#include "internal/client/dcc_cluster_internal.h"

void dcc_cluster_supervisor_record_gateway_start_result(
    dcc_cluster_t *cluster,
    uint32_t index,
    dcc_status_t status
) {
    dcc_cluster_health_lock(cluster);
    dcc_cluster_shard_info_t *info = &cluster->health[index];
    if (status == DCC_OK) {
        info->restarts++;
        info->status = DCC_CLUSTER_SHARD_STARTING;
    } else {
        info->status = DCC_CLUSTER_SHARD_ERROR;
        info->log_errors++;
        dcc_cluster_copy_text(info->last_error, sizeof(info->last_error), dcc_status_string(status));
    }
    dcc_cluster_health_unlock(cluster);
}
