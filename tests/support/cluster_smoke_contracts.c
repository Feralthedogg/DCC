#include "cluster_smoke_support.h"

#include <stdio.h>
#include <string.h>

int cluster_smoke_check_supervisor_helpers(
    dcc_cluster_t *cluster,
    dcc_cluster_supervisor_policy_fn expected_policy,
    void *expected_policy_user_data,
    dcc_cluster_supervisor_decision_fn expected_decision_callback,
    void *expected_decision_user_data
) {
    dcc_cluster_supervisor_policy_fn supervisor_policy = NULL;
    dcc_cluster_supervisor_decision_fn supervisor_decision_callback = NULL;
    void *supervisor_policy_user_data = NULL;
    void *supervisor_decision_user_data = NULL;
    dcc_cluster_supervisor_action_t supervisor_action;
    memset(&supervisor_action, 0x5A, sizeof(supervisor_action));
    dcc_cluster_supervisor_action_init(&supervisor_action);
    dcc_cluster_supervisor_status_t supervisor_status = {
        .size = sizeof(supervisor_status),
    };
    dcc_cluster_supervisor_status_t invalid_supervisor_status = {
        .size = sizeof(invalid_supervisor_status) - 1U,
    };
    if (dcc_cluster_get_supervisor_policy(NULL, &supervisor_policy, &supervisor_policy_user_data) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_get_supervisor_policy(cluster, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_get_supervisor_policy(cluster, &supervisor_policy, &supervisor_policy_user_data) !=
            DCC_OK ||
        supervisor_policy != expected_policy ||
        supervisor_policy_user_data != expected_policy_user_data ||
        dcc_cluster_get_supervisor_decision_callback(
            NULL,
            &supervisor_decision_callback,
            &supervisor_decision_user_data
        ) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_get_supervisor_decision_callback(cluster, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_get_supervisor_decision_callback(
            cluster,
            &supervisor_decision_callback,
            &supervisor_decision_user_data
        ) != DCC_OK ||
        supervisor_decision_callback != expected_decision_callback ||
        supervisor_decision_user_data != expected_decision_user_data ||
        supervisor_action.size != sizeof(supervisor_action) ||
        supervisor_action.decision != DCC_CLUSTER_SUPERVISOR_DEFAULT ||
        supervisor_action.delay_ms != 0U ||
        supervisor_action.reserved[0] != 0U ||
        strcmp(dcc_cluster_supervisor_decision_string(DCC_CLUSTER_SUPERVISOR_DEFAULT), "default") != 0 ||
        strcmp(dcc_cluster_supervisor_decision_string(DCC_CLUSTER_SUPERVISOR_RESTART), "restart") != 0 ||
        strcmp(dcc_cluster_supervisor_decision_string(DCC_CLUSTER_SUPERVISOR_SUPPRESS), "suppress") != 0 ||
        strcmp(dcc_cluster_supervisor_decision_string((dcc_cluster_supervisor_decision_t)999), "unknown") != 0 ||
        dcc_cluster_supervisor_status(NULL, &supervisor_status) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_supervisor_status(cluster, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_supervisor_status(cluster, &invalid_supervisor_status) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_supervisor_status(cluster, &supervisor_status) != DCC_OK ||
        supervisor_status.size != sizeof(supervisor_status) ||
        supervisor_status.decisions != 0U ||
        supervisor_status.restart_scheduled != 0U ||
        supervisor_status.restart_suppressed != 0U ||
        supervisor_status.policy_errors != 0U ||
        supervisor_status.last_shard_index != DCC_CLUSTER_NO_SHARD_INDEX ||
        supervisor_status.last_shard_id != DCC_CLUSTER_NO_SHARD_INDEX ||
        supervisor_status.last_decision != DCC_CLUSTER_SUPERVISOR_DEFAULT ||
        supervisor_status.last_restart_scheduled != 0U ||
        dcc_cluster_set_supervisor_policy(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_set_supervisor_policy(cluster, NULL, NULL) != DCC_OK ||
        dcc_cluster_set_supervisor_decision_callback(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_set_supervisor_decision_callback(cluster, NULL, NULL) != DCC_OK ||
        dcc_cluster_get_supervisor_policy(cluster, &supervisor_policy, &supervisor_policy_user_data) !=
            DCC_OK ||
        supervisor_policy != NULL ||
        supervisor_policy_user_data != NULL ||
        dcc_cluster_get_supervisor_decision_callback(
            cluster,
            &supervisor_decision_callback,
            &supervisor_decision_user_data
        ) != DCC_OK ||
        supervisor_decision_callback != NULL ||
        supervisor_decision_user_data != NULL) {
        fprintf(stderr, "cluster supervisor policy helpers failed\n");
        return 1;
    }

    return 0;
}

int cluster_smoke_check_shard_accessors(dcc_cluster_t *cluster) {
    if (dcc_cluster_shard_count(cluster) != 3 ||
        dcc_cluster_shard(cluster, 0) == NULL ||
        dcc_cluster_shard(cluster, 1) == NULL ||
        dcc_cluster_shard(cluster, 2) == NULL ||
        dcc_cluster_shard(cluster, 3) != NULL ||
        dcc_cluster_shard_const(cluster, 1) == NULL) {
        fprintf(stderr, "cluster shard accessors failed\n");
        return 1;
    }

    return 0;
}

int cluster_smoke_check_initial_recovery(
    dcc_cluster_t *cluster,
    dcc_cluster_recovery_plan_t *plan,
    dcc_cluster_remediation_result_t *remediation,
    dcc_cluster_remediation_loop_result_t *loop_result
) {
    *plan = (dcc_cluster_recovery_plan_t){
        .size = sizeof(*plan),
    };
    dcc_cluster_recovery_plan_t invalid_plan = {
        .size = 0,
    };
    dcc_cluster_health_summary_t wait_summary = {
        .size = sizeof(wait_summary),
    };
    dcc_cluster_health_summary_t invalid_wait_summary = {
        .size = 0,
    };
    dcc_cluster_health_wait_options_t health_wait_options;
    dcc_cluster_health_wait_options_init(&health_wait_options);
    health_wait_options.timeout_ms = 1U;
    health_wait_options.interval_ms = 0U;
    dcc_cluster_health_wait_options_t invalid_health_wait_options = {
        .size = 0,
    };
    dcc_cluster_health_wait_options_t bad_health_wait_options;
    dcc_cluster_health_wait_options_init(&bad_health_wait_options);
    bad_health_wait_options.target_health = DCC_CLUSTER_HEALTH_UNKNOWN;
    dcc_cluster_recovery_plan_t health_wait_plan = {
        .size = sizeof(health_wait_plan),
    };
    dcc_cluster_recovery_plan_t invalid_health_wait_plan = {
        .size = 0,
    };
    dcc_cluster_remediation_options_t invalid_remediation_options = {
        .size = 0,
    };
    *remediation = (dcc_cluster_remediation_result_t){
        .size = sizeof(*remediation),
    };
    dcc_cluster_remediation_result_t invalid_remediation = {
        .size = 0,
    };
    dcc_cluster_remediation_loop_options_t loop_options;
    dcc_cluster_remediation_loop_options_init(&loop_options);
    loop_options.interval_ms = 0;
    dcc_cluster_remediation_loop_options_t invalid_loop_options = {
        .size = 0,
    };
    dcc_cluster_remediation_loop_options_t zero_attempt_loop_options;
    dcc_cluster_remediation_loop_options_init(&zero_attempt_loop_options);
    zero_attempt_loop_options.max_attempts = 0;
    *loop_result = (dcc_cluster_remediation_loop_result_t){
        .size = sizeof(*loop_result),
    };
    dcc_cluster_remediation_loop_result_t invalid_loop_result = {
        .size = 0,
    };
    if (dcc_cluster_recovery_plan(NULL, plan) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_recovery_plan(cluster, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_recovery_plan(cluster, &invalid_plan) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_recovery_plan(cluster, plan) != DCC_OK ||
        dcc_cluster_wait_until_ready(NULL, 1U, &wait_summary) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_wait_until_ready(cluster, 1U, &invalid_wait_summary) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_wait_until_ready(cluster, 1U, &wait_summary) != DCC_ERR_STATE ||
        dcc_cluster_wait_until_health(NULL, NULL, &health_wait_plan) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_wait_until_health(cluster, &invalid_health_wait_options, &health_wait_plan) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_wait_until_health(cluster, &bad_health_wait_options, &health_wait_plan) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_wait_until_health(cluster, &health_wait_options, &invalid_health_wait_plan) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_wait_until_health(cluster, &health_wait_options, &health_wait_plan) !=
            DCC_ERR_STATE ||
        dcc_cluster_remediate(NULL, NULL, remediation) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediate(cluster, &invalid_remediation_options, remediation) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediate(cluster, NULL, &invalid_remediation) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediate(cluster, NULL, remediation) != DCC_OK ||
        dcc_cluster_remediate_loop(NULL, NULL, loop_result) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediate_loop(cluster, &invalid_loop_options, loop_result) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediate_loop(cluster, &zero_attempt_loop_options, loop_result) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediate_loop(cluster, NULL, &invalid_loop_result) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediate_loop(cluster, &loop_options, loop_result) != DCC_OK ||
        plan->health != DCC_CLUSTER_HEALTH_STARTING ||
        plan->action != DCC_CLUSTER_RECOVERY_WAIT ||
        plan->shard_index != 0 ||
        plan->shard_id != 0 ||
        plan->shard_status != DCC_CLUSTER_SHARD_CREATED ||
        plan->summary.total_shards != 3 ||
        plan->summary.created_shards != 3 ||
        plan->shard.status != DCC_CLUSTER_SHARD_CREATED ||
        health_wait_options.size != sizeof(health_wait_options) ||
        health_wait_options.target_health != DCC_CLUSTER_HEALTH_HEALTHY ||
        health_wait_options.timeout_ms != 1U ||
        health_wait_options.interval_ms != 0U ||
        health_wait_options.fail_on_critical != 1U ||
        remediation->plan.health != DCC_CLUSTER_HEALTH_STARTING ||
        remediation->plan.action != DCC_CLUSTER_RECOVERY_WAIT ||
        remediation->action_status != DCC_OK ||
        remediation->action_attempted != 0U ||
        remediation->action_allowed != 0U ||
        loop_options.size != sizeof(loop_options) ||
        loop_options.remediation.size != sizeof(loop_options.remediation) ||
        loop_options.remediation.allow_reconnect != 1U ||
        loop_options.remediation.resume != 1U ||
        loop_options.max_attempts != 3U ||
        loop_options.stop_on_healthy != 1U ||
        loop_options.stop_on_no_action != 1U ||
        loop_result->attempts != 1U ||
        loop_result->actions_attempted != 0U ||
        loop_result->last.plan.action != DCC_CLUSTER_RECOVERY_WAIT ||
        loop_result->last_status != DCC_OK ||
        loop_result->final_health != DCC_CLUSTER_HEALTH_STARTING ||
        loop_result->final_action != DCC_CLUSTER_RECOVERY_WAIT ||
        loop_result->stopped_no_action != 1U ||
        loop_result->exhausted_attempts != 0U ||
        loop_result->stopped_error != 0U ||
        plan->reason[0] == '\0') {
        fprintf(stderr, "cluster initial recovery plan failed\n");
        return 1;
    }

    return 0;
}
