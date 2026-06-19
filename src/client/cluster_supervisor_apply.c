#include "internal/client/dcc_cluster_internal.h"

#include <stdatomic.h>

void dcc_cluster_supervisor_apply_action(
    dcc_cluster_t *cluster,
    dcc_client_t *client,
    uint32_t index,
    dcc_status_t gateway_status,
    dcc_status_t action_status,
    const dcc_cluster_supervisor_action_t *action,
    int *out_should_restart,
    uint32_t *out_delay_ms
) {
    int should_restart = 0;
    uint32_t delay_ms = 0;

    if (action_status != DCC_OK) {
        dcc_cluster_supervisor_apply_decision_error(
            cluster,
            client,
            index,
            gateway_status,
            action_status,
            action
        );
        *out_should_restart = 0;
        *out_delay_ms = 0;
        return;
    }

    dcc_cluster_supervisor_decision_event_t decision_event;
    dcc_cluster_health_lock(cluster);
    dcc_cluster_shard_info_t *info = &cluster->health[index];
    if (action->decision == DCC_CLUSTER_SUPERVISOR_SUPPRESS) {
        info->restart_suppressed++;
        info->status = DCC_CLUSTER_SHARD_ERROR;
        dcc_cluster_copy_text(
            info->last_error,
            sizeof(info->last_error),
            "restart suppressed by supervisor policy"
        );
        atomic_store_explicit(&client->gateway_task_completed, false, memory_order_release);
    } else if (action->decision == DCC_CLUSTER_SUPERVISOR_RESTART) {
        info->restart_attempts++;
        delay_ms = action->delay_ms;
        info->restart_budget_used++;
        info->status = DCC_CLUSTER_SHARD_STARTING;
        dcc_cluster_copy_text(info->last_error, sizeof(info->last_error), dcc_status_string(gateway_status));
        should_restart = 1;
    } else if (info->restart_budget_used < cluster->restart_policy.max_restarts) {
        info->restart_attempts++;
        delay_ms = dcc_cluster_restart_delay_ms(&cluster->restart_policy, info->restart_budget_used);
        info->restart_budget_used++;
        info->status = DCC_CLUSTER_SHARD_STARTING;
        dcc_cluster_copy_text(info->last_error, sizeof(info->last_error), dcc_status_string(gateway_status));
        should_restart = 1;
    } else {
        info->restart_suppressed++;
        info->status = DCC_CLUSTER_SHARD_ERROR;
        dcc_cluster_copy_text(info->last_error, sizeof(info->last_error), "restart limit reached");
        atomic_store_explicit(&client->gateway_task_completed, false, memory_order_release);
    }
    dcc_cluster_supervisor_record_locked(
        cluster,
        index,
        gateway_status,
        action_status,
        action,
        should_restart,
        !should_restart,
        &decision_event
    );
    dcc_cluster_health_unlock(cluster);
    dcc_cluster_supervisor_notify(cluster, &decision_event);

    *out_should_restart = should_restart;
    *out_delay_ms = delay_ms;
}
