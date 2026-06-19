#include "internal/client/dcc_cluster_internal.h"

#include <stdatomic.h>

void dcc_cluster_supervisor_apply_decision_error(
    dcc_cluster_t *cluster,
    dcc_client_t *client,
    uint32_t index,
    dcc_status_t gateway_status,
    dcc_status_t action_status,
    const dcc_cluster_supervisor_action_t *action
) {
    dcc_cluster_supervisor_decision_event_t decision_event;
    dcc_cluster_health_lock(cluster);
    dcc_cluster_supervisor_record_locked(
        cluster,
        index,
        gateway_status,
        action_status,
        action,
        0,
        0,
        &decision_event
    );
    dcc_cluster_shard_info_t *info = &cluster->health[index];
    info->status = DCC_CLUSTER_SHARD_ERROR;
    info->log_errors++;
    dcc_cluster_copy_text(
        info->last_error,
        sizeof(info->last_error),
        dcc_status_string(action_status)
    );
    atomic_store_explicit(&client->gateway_task_completed, false, memory_order_release);
    dcc_cluster_health_unlock(cluster);
    dcc_cluster_supervisor_notify(cluster, &decision_event);
}
