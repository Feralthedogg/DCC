#ifndef DCC_CLUSTER_SUPERVISOR_INTERNAL_H
#define DCC_CLUSTER_SUPERVISOR_INTERNAL_H

#include "internal/client/dcc_cluster_state_internal.h"

#include <dcc/cluster.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_cluster_restart_policy_t dcc_cluster_restart_policy_from_options(
    const dcc_cluster_options_t *options
);
uint32_t dcc_cluster_restart_delay_ms(
    const dcc_cluster_restart_policy_t *policy,
    uint64_t restarts
);
dcc_status_t dcc_cluster_supervisor_decide(
    dcc_cluster_t *cluster,
    uint32_t index,
    dcc_status_t gateway_status,
    dcc_cluster_supervisor_action_t *out
);
dcc_status_t dcc_cluster_supervisor_wait(
    dcc_cluster_t *cluster,
    uint32_t delay_ms
);
void dcc_cluster_supervisor_record_locked(
    dcc_cluster_t *cluster,
    uint32_t index,
    dcc_status_t gateway_status,
    dcc_status_t action_status,
    const dcc_cluster_supervisor_action_t *action,
    int restart_scheduled,
    int restart_suppressed,
    dcc_cluster_supervisor_decision_event_t *event
);
void dcc_cluster_supervisor_notify(
    dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_decision_event_t *event
);
void dcc_cluster_supervisor_apply_decision_error(
    dcc_cluster_t *cluster,
    dcc_client_t *client,
    uint32_t index,
    dcc_status_t gateway_status,
    dcc_status_t action_status,
    const dcc_cluster_supervisor_action_t *action
);
void dcc_cluster_supervisor_apply_action(
    dcc_cluster_t *cluster,
    dcc_client_t *client,
    uint32_t index,
    dcc_status_t gateway_status,
    dcc_status_t action_status,
    const dcc_cluster_supervisor_action_t *action,
    int *out_should_restart,
    uint32_t *out_delay_ms
);
void dcc_cluster_supervisor_record_gateway_start_result(
    dcc_cluster_t *cluster,
    uint32_t index,
    dcc_status_t status
);
void dcc_cluster_supervisor_task(void *arg);

#ifdef __cplusplus
}
#endif

#endif
