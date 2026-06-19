#include "cluster_smoke_support.h"

#include <stdio.h>
#include <string.h>

int cluster_smoke_check_json_contracts(
    dcc_cluster_t *cluster,
    dcc_cluster_recovery_plan_t *plan,
    const dcc_cluster_remediation_result_t *remediation,
    const dcc_cluster_remediation_loop_result_t *loop_result
) {
    char summary_json[2048];
    char plan_json[4096];
    char remediation_json[4096];
    char loop_json[8192];
    char status_json[12288] = {0};
    char tiny_json[8];
    size_t json_len = 0;
    dcc_cluster_health_summary_t invalid_json_summary = {
        .size = 0,
    };
    dcc_cluster_recovery_plan_t invalid_json_plan = {
        .size = 0,
    };
    dcc_cluster_remediation_result_t invalid_json_remediation = {
        .size = 0,
    };
    dcc_cluster_remediation_loop_result_t invalid_json_loop = {
        .size = 0,
    };
    if (dcc_cluster_health_summary_json(NULL, summary_json, sizeof(summary_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_health_summary_json(&invalid_json_summary, summary_json, sizeof(summary_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_health_summary_json(&plan->summary, NULL, sizeof(summary_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_health_summary_json(&plan->summary, tiny_json, sizeof(tiny_json), NULL) !=
            DCC_ERR_NOMEM ||
        dcc_cluster_health_summary_json(&plan->summary, summary_json, sizeof(summary_json), &json_len) !=
            DCC_OK ||
        json_len == 0U ||
        strstr(summary_json, "\"health\":\"starting\"") == NULL ||
        strstr(summary_json, "\"total_shards\":3") == NULL ||
        strstr(summary_json, "\"created_shards\":3") == NULL ||
        dcc_cluster_recovery_plan_json(NULL, plan_json, sizeof(plan_json), NULL) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_recovery_plan_json(&invalid_json_plan, plan_json, sizeof(plan_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_recovery_plan_json(plan, plan_json, sizeof(plan_json), NULL) != DCC_OK ||
        strstr(plan_json, "\"action\":\"wait\"") == NULL ||
        strstr(plan_json, "\"shard_status\":\"created\"") == NULL ||
        strstr(plan_json, "\"summary\":{") == NULL ||
        strstr(plan_json, "\"shard\":{") == NULL ||
        dcc_cluster_remediation_result_json(NULL, remediation_json, sizeof(remediation_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediation_result_json(
            &invalid_json_remediation,
            remediation_json,
            sizeof(remediation_json),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediation_result_json(remediation, remediation_json, sizeof(remediation_json), NULL) !=
            DCC_OK ||
        strstr(remediation_json, "\"action_status\":\"ok\"") == NULL ||
        strstr(remediation_json, "\"action_attempted\":false") == NULL ||
        strstr(remediation_json, "\"plan\":{") == NULL ||
        dcc_cluster_remediation_loop_result_json(NULL, loop_json, sizeof(loop_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediation_loop_result_json(
            &invalid_json_loop,
            loop_json,
            sizeof(loop_json),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_remediation_loop_result_json(loop_result, tiny_json, sizeof(tiny_json), NULL) !=
            DCC_ERR_NOMEM ||
        dcc_cluster_remediation_loop_result_json(loop_result, loop_json, sizeof(loop_json), NULL) !=
            DCC_OK ||
        strstr(loop_json, "\"last_status\":\"ok\"") == NULL ||
        strstr(loop_json, "\"attempts\":1") == NULL ||
        strstr(loop_json, "\"actions_attempted\":0") == NULL ||
        strstr(loop_json, "\"final_health\":\"starting\"") == NULL ||
        strstr(loop_json, "\"final_action\":\"wait\"") == NULL ||
        strstr(loop_json, "\"stopped_no_action\":true") == NULL ||
        strstr(loop_json, "\"last\":{") == NULL ||
        dcc_cluster_status_json(NULL, status_json, sizeof(status_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_status_json(cluster, NULL, sizeof(status_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_status_json(cluster, tiny_json, sizeof(tiny_json), NULL) !=
            DCC_ERR_NOMEM ||
        dcc_cluster_status_json(cluster, status_json, sizeof(status_json), &json_len) != DCC_OK ||
        json_len == 0U ||
        strstr(status_json, "\"recovery\":{") == NULL ||
        strstr(status_json, "\"operation\":{") == NULL ||
        strstr(status_json, "\"supervisor\":{") == NULL ||
        strstr(status_json, "\"health\":\"starting\"") == NULL ||
        strstr(status_json, "\"type\":\"none\"") == NULL ||
        strstr(status_json, "\"last_status\":\"ok\"") == NULL ||
        strstr(status_json, "\"decisions\":0") == NULL ||
        strstr(status_json, "\"last_decision\":\"default\"") == NULL) {
        fprintf(
            stderr,
            "cluster JSON contracts failed: summary=%s plan=%s remediation=%s loop=%s status=%s\n",
            summary_json,
            plan_json,
            remediation_json,
            loop_json,
            status_json
        );
        return 1;
    }

    snprintf(plan->reason, sizeof(plan->reason), "%s", "quote\"slash\\line\n");
    if (dcc_cluster_recovery_plan_json(plan, plan_json, sizeof(plan_json), NULL) != DCC_OK ||
        strstr(plan_json, "quote\\\"slash\\\\line\\n") == NULL) {
        fprintf(stderr, "cluster JSON escaping failed: %s\n", plan_json);
        return 1;
    }

    return 0;
}

int cluster_smoke_check_health_classifiers(void) {
    dcc_cluster_health_summary_t synthetic_summary = {
        .size = sizeof(synthetic_summary),
        .total_shards = 2,
        .ready_shards = 2,
    };
    dcc_cluster_shard_info_t synthetic_shard = {
        .size = sizeof(synthetic_shard),
        .status = DCC_CLUSTER_SHARD_READY,
    };
    dcc_cluster_health_summary_t invalid_summary = {
        .size = 0,
    };
    dcc_cluster_shard_info_t invalid_shard = {
        .size = 0,
    };
    if (dcc_cluster_health_summary_state(NULL) != DCC_CLUSTER_HEALTH_UNKNOWN ||
        dcc_cluster_health_summary_state(&invalid_summary) != DCC_CLUSTER_HEALTH_UNKNOWN ||
        dcc_cluster_health_summary_state(&synthetic_summary) != DCC_CLUSTER_HEALTH_HEALTHY ||
        strcmp(dcc_cluster_health_state_string(DCC_CLUSTER_HEALTH_HEALTHY), "healthy") != 0 ||
        strcmp(dcc_cluster_health_state_string((dcc_cluster_health_state_t)999), "unknown") != 0 ||
        dcc_cluster_shard_recovery_action(NULL) != DCC_CLUSTER_RECOVERY_UNKNOWN ||
        dcc_cluster_shard_recovery_action(&invalid_shard) != DCC_CLUSTER_RECOVERY_UNKNOWN ||
        dcc_cluster_shard_recovery_action(&synthetic_shard) != DCC_CLUSTER_RECOVERY_NONE ||
        strcmp(dcc_cluster_recovery_action_string(DCC_CLUSTER_RECOVERY_REARM_RESTART), "rearm_restart") != 0 ||
        strcmp(dcc_cluster_recovery_action_string((dcc_cluster_recovery_action_t)999), "unknown") != 0) {
        fprintf(stderr, "cluster health classifier base contracts failed\n");
        return 1;
    }

    synthetic_summary.socket_closes = 1;
    if (dcc_cluster_health_summary_state(&synthetic_summary) != DCC_CLUSTER_HEALTH_DEGRADED) {
        fprintf(stderr, "cluster degraded classifier failed\n");
        return 1;
    }
    synthetic_summary.socket_closes = 0;
    synthetic_summary.ready_shards = 1;
    synthetic_summary.starting_shards = 1;
    if (dcc_cluster_health_summary_state(&synthetic_summary) != DCC_CLUSTER_HEALTH_STARTING) {
        fprintf(stderr, "cluster starting classifier failed\n");
        return 1;
    }
    synthetic_summary.starting_shards = 0;
    synthetic_summary.ready_shards = 0;
    synthetic_summary.stopped_shards = 2;
    if (dcc_cluster_health_summary_state(&synthetic_summary) != DCC_CLUSTER_HEALTH_DRAINING) {
        fprintf(stderr, "cluster draining classifier failed\n");
        return 1;
    }
    synthetic_summary.stopped_shards = 0;
    synthetic_summary.error_shards = 1;
    if (dcc_cluster_health_summary_state(&synthetic_summary) != DCC_CLUSTER_HEALTH_CRITICAL) {
        fprintf(stderr, "cluster critical classifier failed\n");
        return 1;
    }

    synthetic_shard.status = DCC_CLUSTER_SHARD_STARTING;
    if (dcc_cluster_shard_recovery_action(&synthetic_shard) != DCC_CLUSTER_RECOVERY_WAIT) {
        fprintf(stderr, "cluster wait action classifier failed\n");
        return 1;
    }
    synthetic_shard.status = DCC_CLUSTER_SHARD_STOPPED;
    synthetic_shard.socket_closes = 1;
    if (dcc_cluster_shard_recovery_action(&synthetic_shard) != DCC_CLUSTER_RECOVERY_RECONNECT) {
        fprintf(stderr, "cluster reconnect action classifier failed\n");
        return 1;
    }
    synthetic_shard.socket_closes = 0;
    synthetic_shard.status = DCC_CLUSTER_SHARD_ERROR;
    if (dcc_cluster_shard_recovery_action(&synthetic_shard) != DCC_CLUSTER_RECOVERY_INVESTIGATE) {
        fprintf(stderr, "cluster investigate action classifier failed\n");
        return 1;
    }
    synthetic_shard.restart_suppressed = 1;
    if (dcc_cluster_shard_recovery_action(&synthetic_shard) != DCC_CLUSTER_RECOVERY_REARM_RESTART) {
        fprintf(stderr, "cluster rearm action classifier failed\n");
        return 1;
    }

    return 0;
}
