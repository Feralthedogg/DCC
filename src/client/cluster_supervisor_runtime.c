#include "internal/client/dcc_cluster_internal.h"

#include <stdatomic.h>
#include <string.h>

dcc_status_t dcc_cluster_supervisor_wait(dcc_cluster_t *cluster, uint32_t delay_ms) {
    uint32_t remaining = delay_ms;
    while (remaining > 0 && !atomic_load_explicit(&cluster->supervisor_stop, memory_order_acquire)) {
        uint32_t slice = remaining > 100U ? 100U : remaining;
        (void)llam_sleep_ns((uint64_t)slice * 1000000ULL);
        remaining -= slice;
    }
    return atomic_load_explicit(&cluster->supervisor_stop, memory_order_acquire) ? DCC_ERR_CANCELED : DCC_OK;
}

void dcc_cluster_supervisor_record_locked(
    dcc_cluster_t *cluster,
    uint32_t index,
    dcc_status_t gateway_status,
    dcc_status_t action_status,
    const dcc_cluster_supervisor_action_t *action,
    int restart_scheduled,
    int restart_suppressed,
    dcc_cluster_supervisor_decision_event_t *event
) {
    dcc_cluster_supervisor_status_t *status = &cluster->supervisor_status;
    status->size = sizeof(*status);
    status->decisions++;
    status->last_shard_index = index;
    status->last_shard_id = index < cluster->shard_count
        ? cluster->health[index].shard_id
        : DCC_CLUSTER_NO_SHARD_INDEX;
    status->last_gateway_status = gateway_status;
    status->last_action_status = action_status;
    status->last_decision = action != NULL ? action->decision : DCC_CLUSTER_SUPERVISOR_DEFAULT;
    status->last_delay_ms = action != NULL ? action->delay_ms : 0U;
    status->last_restart_scheduled = restart_scheduled ? 1U : 0U;
    if (restart_scheduled) {
        status->restart_scheduled++;
    }
    if (restart_suppressed) {
        status->restart_suppressed++;
    }
    if (action_status != DCC_OK) {
        status->policy_errors++;
    }

    if (event != NULL) {
        memset(event, 0, sizeof(*event));
        event->size = sizeof(*event);
        event->shard_index = index;
        event->shard_id = status->last_shard_id;
        event->gateway_status = gateway_status;
        event->action_status = action_status;
        if (action != NULL) {
            event->action = *action;
        } else {
            dcc_cluster_supervisor_action_init(&event->action);
        }
        if (index < cluster->shard_count) {
            event->shard = cluster->health[index];
        }
        event->restart_policy = cluster->restart_policy;
        event->status = *status;
        event->restart_scheduled = restart_scheduled ? 1U : 0U;
        event->restart_suppressed = restart_suppressed ? 1U : 0U;
    }
}

void dcc_cluster_supervisor_notify(
    dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_decision_event_t *event
) {
    if (cluster == NULL || event == NULL || cluster->supervisor_decision_callback == NULL) {
        return;
    }
    cluster->supervisor_decision_callback(
        cluster,
        event,
        cluster->supervisor_decision_user_data
    );
}
