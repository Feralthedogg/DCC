#ifndef DCC_TEST_CLUSTER_SMOKE_SUPPORT_H
#define DCC_TEST_CLUSTER_SMOKE_SUPPORT_H

#include <dcc/dcc.h>

#include <stdatomic.h>

typedef struct cluster_smoke_state {
    dcc_cluster_t *cluster;
    atomic_uint ready_count;
    atomic_uint bad_event;
    atomic_uint drain_status;
    atomic_uint drain_seen;
    atomic_uint admission_reconnect_status;
    atomic_uint admission_restore_status;
    atomic_uint planned_run_status;
    atomic_uint planned_completed;
    atomic_uint planned_failed_shards;
    atomic_uint planned_last_status;
    atomic_uint seen[3];
} cluster_smoke_state_t;

dcc_status_t cluster_smoke_supervisor_policy_probe(
    const dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_event_t *event,
    dcc_cluster_supervisor_action_t *action,
    void *user_data
);

void cluster_smoke_supervisor_decision_probe(
    const dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_decision_event_t *event,
    void *user_data
);

void cluster_smoke_on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void cluster_smoke_state_init(cluster_smoke_state_t *state, dcc_cluster_t *cluster);

int cluster_smoke_check_supervisor_helpers(
    dcc_cluster_t *cluster,
    dcc_cluster_supervisor_policy_fn expected_policy,
    void *expected_policy_user_data,
    dcc_cluster_supervisor_decision_fn expected_decision_callback,
    void *expected_decision_user_data
);

int cluster_smoke_check_shard_accessors(dcc_cluster_t *cluster);
int cluster_smoke_check_initial_recovery(
    dcc_cluster_t *cluster,
    dcc_cluster_recovery_plan_t *plan,
    dcc_cluster_remediation_result_t *remediation,
    dcc_cluster_remediation_loop_result_t *loop_result
);
int cluster_smoke_check_json_contracts(
    dcc_cluster_t *cluster,
    dcc_cluster_recovery_plan_t *plan,
    const dcc_cluster_remediation_result_t *remediation,
    const dcc_cluster_remediation_loop_result_t *loop_result
);
int cluster_smoke_check_health_classifiers(void);
int cluster_smoke_check_operation_and_admission_contracts(dcc_cluster_t *cluster);
int cluster_smoke_check_initial_shards(dcc_cluster_t *cluster);
int cluster_smoke_check_runtime_state(dcc_cluster_t *cluster, const cluster_smoke_state_t *state);
int cluster_smoke_check_post_run_health(dcc_cluster_t *cluster);
int cluster_smoke_check_counter_resets(dcc_cluster_t *cluster);

#endif
