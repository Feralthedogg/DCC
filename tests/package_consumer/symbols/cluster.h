#ifndef DCC_PACKAGE_CONSUMER_SYMBOLS_CLUSTER_H
#define DCC_PACKAGE_CONSUMER_SYMBOLS_CLUSTER_H

#include <dcc/dcc.h>

typedef dcc_status_t (*dcc_package_cluster_rearm_one_fn)(dcc_cluster_t *, uint32_t);
typedef dcc_status_t (*dcc_package_cluster_rearm_all_fn)(dcc_cluster_t *);
typedef dcc_status_t (*dcc_package_cluster_recovery_plan_fn)(const dcc_cluster_t *, dcc_cluster_recovery_plan_t *);
typedef dcc_status_t (*dcc_package_cluster_wait_ready_fn)(dcc_cluster_t *, uint32_t, dcc_cluster_health_summary_t *);
typedef void (*dcc_package_cluster_health_wait_options_init_fn)(dcc_cluster_health_wait_options_t *);
typedef dcc_status_t (*dcc_package_cluster_wait_health_fn)(dcc_cluster_t *, const dcc_cluster_health_wait_options_t *, dcc_cluster_recovery_plan_t *);
typedef dcc_cluster_health_state_t (*dcc_package_cluster_health_state_fn)(const dcc_cluster_health_summary_t *);
typedef const char *(*dcc_package_cluster_health_state_string_fn)(dcc_cluster_health_state_t);
typedef dcc_cluster_recovery_action_t (*dcc_package_cluster_recovery_action_fn)(const dcc_cluster_shard_info_t *);
typedef const char *(*dcc_package_cluster_recovery_action_string_fn)(dcc_cluster_recovery_action_t);
typedef dcc_status_t (*dcc_package_cluster_remediate_fn)(
        dcc_cluster_t *,
        const dcc_cluster_remediation_options_t *,
        dcc_cluster_remediation_result_t *
    );
typedef void (*dcc_package_cluster_supervisor_action_init_fn)(
        dcc_cluster_supervisor_action_t *
    );
typedef const char *(*dcc_package_cluster_supervisor_decision_string_fn)(
        dcc_cluster_supervisor_decision_t
    );
typedef dcc_status_t (*dcc_package_cluster_supervisor_status_fn)(const dcc_cluster_t *, dcc_cluster_supervisor_status_t *);
typedef void (*dcc_package_cluster_remediation_loop_options_init_fn)(
        dcc_cluster_remediation_loop_options_t *
    );
typedef dcc_status_t (*dcc_package_cluster_remediate_loop_fn)(
        dcc_cluster_t *,
        const dcc_cluster_remediation_loop_options_t *,
        dcc_cluster_remediation_loop_result_t *
    );
typedef dcc_status_t (*dcc_package_cluster_summary_json_fn)(
        const dcc_cluster_health_summary_t *,
        char *,
        size_t,
        size_t *
    );
typedef dcc_status_t (*dcc_package_cluster_plan_json_fn)(
        const dcc_cluster_recovery_plan_t *,
        char *,
        size_t,
        size_t *
    );
typedef dcc_status_t (*dcc_package_cluster_remediation_json_fn)(
        const dcc_cluster_remediation_result_t *,
        char *,
        size_t,
        size_t *
    );
typedef dcc_status_t (*dcc_package_cluster_remediation_loop_json_fn)(
        const dcc_cluster_remediation_loop_result_t *,
        char *,
        size_t,
        size_t *
    );
typedef dcc_status_t (*dcc_package_cluster_operation_json_fn)(
        const dcc_cluster_operation_status_t *,
        char *,
        size_t,
        size_t *
    );
typedef dcc_status_t (*dcc_package_cluster_status_json_fn)(
        const dcc_cluster_t *,
        char *,
        size_t,
        size_t *
    );
typedef dcc_status_t (*dcc_package_cluster_add_status_route_fn)(dcc_interaction_server_t *, const char *, dcc_cluster_t *);

#endif
