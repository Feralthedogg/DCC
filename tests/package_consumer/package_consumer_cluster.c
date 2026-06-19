#include "package_consumer_checks.h"

#include <stdint.h>
#include <string.h>

int dcc_package_consumer_check_cluster_api(void) {
    dcc_package_cluster_rearm_one_fn cluster_rearm_one = dcc_cluster_rearm_shard_restart;
    dcc_package_cluster_rearm_all_fn cluster_rearm_all = dcc_cluster_rearm_all_shard_restarts;
    dcc_package_cluster_recovery_plan_fn cluster_recovery_plan = dcc_cluster_recovery_plan;
    dcc_package_cluster_wait_ready_fn cluster_wait_ready = dcc_cluster_wait_until_ready;
    dcc_package_cluster_health_wait_options_init_fn cluster_health_wait_options_init =
        dcc_cluster_health_wait_options_init;
    dcc_package_cluster_wait_health_fn cluster_wait_health = dcc_cluster_wait_until_health;
    dcc_package_cluster_health_state_fn cluster_health_state = dcc_cluster_health_summary_state;
    dcc_package_cluster_health_state_string_fn cluster_health_state_string = dcc_cluster_health_state_string;
    dcc_package_cluster_recovery_action_fn cluster_recovery_action = dcc_cluster_shard_recovery_action;
    dcc_package_cluster_recovery_action_string_fn cluster_recovery_action_string =
        dcc_cluster_recovery_action_string;
    dcc_package_cluster_remediate_fn cluster_remediate = dcc_cluster_remediate;
    dcc_package_cluster_supervisor_action_init_fn cluster_supervisor_action_init =
        dcc_cluster_supervisor_action_init;
    dcc_package_cluster_supervisor_decision_string_fn cluster_supervisor_decision_string =
        dcc_cluster_supervisor_decision_string;
    dcc_package_cluster_supervisor_status_fn cluster_supervisor_status = dcc_cluster_supervisor_status;
    dcc_package_cluster_remediation_loop_options_init_fn cluster_loop_options_init =
        dcc_cluster_remediation_loop_options_init;
    dcc_package_cluster_remediate_loop_fn cluster_remediate_loop = dcc_cluster_remediate_loop;
    dcc_package_cluster_summary_json_fn cluster_summary_json = dcc_cluster_health_summary_json;
    dcc_package_cluster_plan_json_fn cluster_plan_json = dcc_cluster_recovery_plan_json;
    dcc_package_cluster_remediation_json_fn cluster_remediation_json =
        dcc_cluster_remediation_result_json;
    dcc_package_cluster_remediation_loop_json_fn cluster_remediation_loop_json =
        dcc_cluster_remediation_loop_result_json;
    dcc_package_cluster_operation_json_fn cluster_operation_json =
        dcc_cluster_operation_status_json;
    dcc_package_cluster_status_json_fn cluster_status_json = dcc_cluster_status_json;
    dcc_package_cluster_add_status_route_fn cluster_add_status_route =
        dcc_cluster_add_status_route;
    dcc_cluster_supervisor_action_t package_supervisor_action;
    memset(&package_supervisor_action, 0x5A, sizeof(package_supervisor_action));
    cluster_supervisor_action_init(&package_supervisor_action);
    dcc_cluster_supervisor_status_t package_supervisor_status = { .size = sizeof(package_supervisor_status) };
    dcc_cluster_remediation_loop_options_t package_cluster_loop_options;
    cluster_loop_options_init(&package_cluster_loop_options);
    dcc_cluster_health_wait_options_t package_cluster_wait_options;
    cluster_health_wait_options_init(&package_cluster_wait_options);
    dcc_cluster_operation_status_t package_cluster_operation = {
        .size = sizeof(package_cluster_operation),
    };

    return cluster_rearm_one != NULL &&
           cluster_rearm_all != NULL &&
           cluster_recovery_plan != NULL &&
           cluster_wait_ready != NULL &&
           cluster_wait_ready(NULL, 1, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_health_wait_options_init != NULL &&
           cluster_wait_health != NULL &&
           package_cluster_wait_options.size == sizeof(package_cluster_wait_options) &&
           package_cluster_wait_options.target_health == DCC_CLUSTER_HEALTH_HEALTHY &&
           package_cluster_wait_options.timeout_ms == 30000U &&
           package_cluster_wait_options.interval_ms == 10U &&
           package_cluster_wait_options.fail_on_critical == 1U &&
           cluster_wait_health(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_health_state != NULL &&
           cluster_health_state_string != NULL &&
           cluster_recovery_action != NULL &&
           cluster_recovery_action_string != NULL &&
           DCC_CLUSTER_NO_SHARD_INDEX == UINT32_MAX &&
           cluster_recovery_plan(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_health_state(NULL) == DCC_CLUSTER_HEALTH_UNKNOWN &&
           cluster_health_state_string(DCC_CLUSTER_HEALTH_DEGRADED) != NULL &&
           cluster_recovery_action(NULL) == DCC_CLUSTER_RECOVERY_UNKNOWN &&
           cluster_recovery_action_string(DCC_CLUSTER_RECOVERY_RECONNECT) != NULL &&
           cluster_remediate != NULL &&
           cluster_remediate(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_supervisor_action_init != NULL &&
           cluster_supervisor_decision_string != NULL &&
           package_supervisor_action.size == sizeof(package_supervisor_action) &&
           package_supervisor_action.decision == DCC_CLUSTER_SUPERVISOR_DEFAULT &&
           package_supervisor_action.delay_ms == 0U &&
           package_supervisor_action.reserved[0] == 0U &&
           cluster_supervisor_decision_string(DCC_CLUSTER_SUPERVISOR_RESTART) != NULL &&
           cluster_supervisor_status != NULL &&
           cluster_supervisor_status(NULL, &package_supervisor_status) == DCC_ERR_INVALID_ARG &&
           dcc_cluster_set_supervisor_decision_callback(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
           dcc_cluster_get_supervisor_decision_callback(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
           dcc_cluster_supervisor_decision_event_json(NULL, NULL, 0, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_loop_options_init != NULL &&
           cluster_remediate_loop != NULL &&
           package_cluster_loop_options.size == sizeof(package_cluster_loop_options) &&
           package_cluster_loop_options.remediation.size == sizeof(package_cluster_loop_options.remediation) &&
           package_cluster_loop_options.remediation.allow_reconnect == 1U &&
           package_cluster_loop_options.remediation.resume == 1U &&
           package_cluster_loop_options.max_attempts == 3U &&
           package_cluster_loop_options.interval_ms == 1000U &&
           package_cluster_loop_options.stop_on_healthy == 1U &&
           package_cluster_loop_options.stop_on_no_action == 1U &&
           cluster_remediate_loop(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_summary_json != NULL &&
           cluster_summary_json(NULL, NULL, 0, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_plan_json != NULL &&
           cluster_plan_json(NULL, NULL, 0, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_remediation_json != NULL &&
           cluster_remediation_json(NULL, NULL, 0, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_remediation_loop_json != NULL &&
           cluster_remediation_loop_json(NULL, NULL, 0, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_operation_json != NULL &&
           cluster_operation_json(NULL, NULL, 0, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_operation_json(&package_cluster_operation, NULL, 0, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_status_json != NULL &&
           cluster_status_json(NULL, NULL, 0, NULL) == DCC_ERR_INVALID_ARG &&
           cluster_add_status_route != NULL &&
           cluster_add_status_route(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG;
}
