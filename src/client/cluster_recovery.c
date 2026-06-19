#include "internal/client/dcc_cluster_internal.h"

#include <string.h>

static void dcc_cluster_recovery_plan_init(dcc_cluster_recovery_plan_t *plan) {
    memset(plan, 0, sizeof(*plan));
    plan->size = sizeof(*plan);
    plan->health = DCC_CLUSTER_HEALTH_UNKNOWN;
    plan->action = DCC_CLUSTER_RECOVERY_UNKNOWN;
    plan->shard_index = DCC_CLUSTER_NO_SHARD_INDEX;
    plan->shard_id = DCC_CLUSTER_NO_SHARD_INDEX;
    plan->shard_status = (dcc_cluster_shard_status_t)-1;
    plan->summary.size = sizeof(plan->summary);
    plan->shard.size = sizeof(plan->shard);
    plan->shard.status = (dcc_cluster_shard_status_t)-1;
}

dcc_status_t dcc_cluster_recovery_plan(
    const dcc_cluster_t *cluster,
    dcc_cluster_recovery_plan_t *out
) {
    if (cluster == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_recovery_plan_t plan;
    dcc_cluster_recovery_plan_init(&plan);
    plan.action = DCC_CLUSTER_RECOVERY_NONE;
    dcc_cluster_copy_text(
        plan.reason,
        sizeof(plan.reason),
        dcc_cluster_recovery_default_reason(plan.action)
    );

    dcc_cluster_t *mutable_cluster = (dcc_cluster_t *)cluster;
    dcc_cluster_health_lock(mutable_cluster);
    plan.summary.total_shards = cluster->shard_count;
    unsigned best_priority = dcc_cluster_recovery_action_priority(plan.action);
    for (uint32_t i = 0; i < cluster->shard_count; ++i) {
        const dcc_cluster_shard_info_t *info = &cluster->health[i];
        dcc_cluster_summary_add_status(&plan.summary, info->status);
        plan.summary.starts += info->starts;
        plan.summary.restart_attempts += info->restart_attempts;
        plan.summary.restarts += info->restarts;
        plan.summary.restart_suppressed += info->restart_suppressed;
        plan.summary.restart_budget_used += info->restart_budget_used;
        plan.summary.ready_events += info->ready_events;
        plan.summary.resumed_events += info->resumed_events;
        plan.summary.socket_closes += info->socket_closes;
        plan.summary.log_errors += info->log_errors;

        dcc_cluster_recovery_action_t action = dcc_cluster_shard_recovery_action(info);
        unsigned priority = dcc_cluster_recovery_action_priority(action);
        if (priority > best_priority) {
            best_priority = priority;
            plan.action = action;
            plan.shard_index = i;
            plan.shard_id = info->shard_id;
            plan.shard_status = info->status;
            plan.shard = *info;
            plan.shard.size = sizeof(plan.shard);
            const char *reason = info->last_error[0] != '\0'
                ? info->last_error
                : (info->last_close_reason[0] != '\0'
                    ? info->last_close_reason
                    : dcc_cluster_recovery_default_reason(action));
            dcc_cluster_copy_text(plan.reason, sizeof(plan.reason), reason);
        }
    }
    dcc_cluster_health_unlock(mutable_cluster);

    plan.health = dcc_cluster_health_summary_state(&plan.summary);
    if (plan.action == DCC_CLUSTER_RECOVERY_NONE &&
        plan.health == DCC_CLUSTER_HEALTH_DEGRADED) {
        plan.action = DCC_CLUSTER_RECOVERY_INVESTIGATE;
        plan.shard_index = DCC_CLUSTER_NO_SHARD_INDEX;
        plan.shard_id = DCC_CLUSTER_NO_SHARD_INDEX;
        plan.shard_status = (dcc_cluster_shard_status_t)-1;
        memset(&plan.shard, 0, sizeof(plan.shard));
        plan.shard.size = sizeof(plan.shard);
        plan.shard.status = (dcc_cluster_shard_status_t)-1;
        dcc_cluster_copy_text(
            plan.reason,
            sizeof(plan.reason),
            dcc_cluster_recovery_default_reason(plan.action)
        );
    }

    *out = plan;
    return DCC_OK;
}
