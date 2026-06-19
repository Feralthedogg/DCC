#include "cluster_smoke_support.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int cluster_smoke_check_operation_and_admission_contracts(dcc_cluster_t *cluster) {
    uint8_t admission_open = 0;
    dcc_cluster_rolling_reconnect_options_t rolling = {
        .size = sizeof(rolling),
        .first_index = 0,
        .shard_count = 3,
        .batch_size = 2,
        .delay_ms = 1,
        .resume = 0,
    };
    dcc_cluster_rolling_reconnect_options_t bad_rolling = {
        .size = 0,
    };
    dcc_cluster_rolling_reconnect_options_t runtime_rolling = {
        .size = sizeof(runtime_rolling),
        .first_index = 1,
        .shard_count = 2,
        .batch_size = 2,
        .delay_ms = 0,
        .resume = 0,
    };
    dcc_cluster_operation_status_t operation = {
        .size = sizeof(operation),
    };
    dcc_cluster_operation_status_t invalid_json_operation = {
        .size = 0,
    };
    char operation_json[2048] = {0};
    char tiny_json[8];
    size_t operation_json_len = 0;
    if (dcc_cluster_operation_status(cluster, &operation) != DCC_OK ||
        operation.type != DCC_CLUSTER_OPERATION_NONE ||
        operation.active != 0 ||
        operation.completed != 0 ||
        operation.generation != 0 ||
        dcc_cluster_operation_status(NULL, &operation) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_operation_status(cluster, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_operation_status_json(NULL, operation_json, sizeof(operation_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_operation_status_json(
            &invalid_json_operation,
            operation_json,
            sizeof(operation_json),
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_operation_status_json(&operation, NULL, sizeof(operation_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_cluster_operation_status_json(&operation, tiny_json, sizeof(tiny_json), NULL) !=
            DCC_ERR_NOMEM ||
        dcc_cluster_operation_status_json(
            &operation,
            operation_json,
            sizeof(operation_json),
            &operation_json_len
        ) != DCC_OK ||
        operation_json_len == 0U ||
        strstr(operation_json, "\"type\":\"none\"") == NULL ||
        strstr(operation_json, "\"generation\":0") == NULL ||
        strstr(operation_json, "\"active\":false") == NULL ||
        strstr(operation_json, "\"last_status\":\"ok\"") == NULL ||
        dcc_cluster_run_planned_operation(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_run_planned_operation(cluster) != DCC_ERR_STATE ||
        dcc_cluster_clear_operation(NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "cluster initial operation contracts failed: %s\n", operation_json);
        return 1;
    }

    if (dcc_cluster_gateway_admission(cluster, &admission_open) != DCC_OK ||
        admission_open != 1U ||
        dcc_cluster_set_gateway_admission(cluster, 0) != DCC_OK ||
        dcc_cluster_gateway_admission(cluster, &admission_open) != DCC_OK ||
        admission_open != 0U ||
        dcc_cluster_start(cluster) != DCC_ERR_STATE ||
        dcc_cluster_rolling_reconnect(cluster, &rolling) != DCC_ERR_STATE ||
        dcc_cluster_rolling_reconnect(cluster, &bad_rolling) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_gateway_admission(NULL, &admission_open) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_gateway_admission(cluster, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_set_gateway_admission(NULL, 1) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_set_gateway_admission(cluster, 1) != DCC_OK ||
        dcc_cluster_gateway_admission(cluster, &admission_open) != DCC_OK ||
        admission_open != 1U) {
        fprintf(stderr, "cluster admission and rolling reconnect contracts failed\n");
        return 1;
    }

    memset(&operation, 0, sizeof(operation));
    operation.size = sizeof(operation);
    if (dcc_cluster_plan_rolling_reconnect(NULL, &rolling) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_plan_rolling_reconnect(cluster, &bad_rolling) != DCC_ERR_INVALID_ARG ||
        dcc_cluster_plan_rolling_reconnect(cluster, &rolling) != DCC_OK ||
        dcc_cluster_operation_status(cluster, &operation) != DCC_OK ||
        operation.type != DCC_CLUSTER_OPERATION_ROLLING_RECONNECT ||
        operation.generation != 1U ||
        operation.active != 0 ||
        operation.completed != 0 ||
        operation.resume != 0 ||
        operation.first_index != 0 ||
        operation.shard_count != 3 ||
        operation.batch_size != 2 ||
        operation.delay_ms != 1 ||
        operation.next_index != 0 ||
        operation.completed_shards != 0 ||
        operation.failed_shards != 0 ||
        operation.total_batches != 2 ||
        operation.last_status != DCC_OK ||
        dcc_cluster_operation_status_json(&operation, operation_json, sizeof(operation_json), NULL) !=
            DCC_OK ||
        strstr(operation_json, "\"type\":\"rolling_reconnect\"") == NULL ||
        strstr(operation_json, "\"generation\":1") == NULL ||
        strstr(operation_json, "\"active\":false") == NULL ||
        strstr(operation_json, "\"resume\":false") == NULL ||
        strstr(operation_json, "\"first_index\":0") == NULL ||
        strstr(operation_json, "\"shard_count\":3") == NULL ||
        strstr(operation_json, "\"batch_size\":2") == NULL ||
        strstr(operation_json, "\"delay_ms\":1") == NULL ||
        strstr(operation_json, "\"total_batches\":2") == NULL ||
        dcc_cluster_clear_operation(cluster) != DCC_OK) {
        fprintf(stderr, "cluster rolling operation plan contracts failed: %s\n", operation_json);
        return 1;
    }

    memset(&operation, 0, sizeof(operation));
    operation.size = sizeof(operation);
    if (dcc_cluster_operation_status(cluster, &operation) != DCC_OK ||
        operation.type != DCC_CLUSTER_OPERATION_NONE ||
        operation.generation != 1U ||
        dcc_cluster_plan_rolling_reconnect(cluster, &runtime_rolling) != DCC_OK ||
        dcc_cluster_run_planned_operation(cluster) != DCC_ERR_STATE) {
        fprintf(stderr, "cluster rolling operation clear/runtime contracts failed\n");
        return 1;
    }

    if (dcc_cluster_drain_shard(cluster, 0) != DCC_ERR_STATE ||
        dcc_cluster_resume_shard(cluster, 0) != DCC_ERR_STATE ||
        dcc_cluster_reconnect_shard(cluster, 0, 0) != DCC_ERR_STATE ||
        dcc_cluster_reconnect_all(cluster, 0) != DCC_ERR_STATE ||
        dcc_cluster_drain_shard(cluster, 3) != DCC_ERR_INVALID_ARG ||
        dcc_client_gateway_start(dcc_cluster_shard(cluster, 0)) != DCC_ERR_STATE ||
        dcc_client_gateway_reconnect(dcc_cluster_shard(cluster, 0), 0) != DCC_ERR_STATE ||
        dcc_client_gateway_stop(NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "gateway control pre-start contracts failed\n");
        return 1;
    }

    dcc_cluster_restart_policy_t policy = {
        .size = sizeof(policy),
        .max_restarts = 0,
        .base_delay_ms = 20,
        .max_delay_ms = 40,
    };
    dcc_status_t status = dcc_cluster_set_restart_policy(cluster, &policy);
    memset(&policy, 0, sizeof(policy));
    policy.size = sizeof(policy);
    if (status != DCC_OK ||
        dcc_cluster_get_restart_policy(cluster, &policy) != DCC_OK ||
        policy.max_restarts != 0 ||
        policy.base_delay_ms != 20 ||
        policy.max_delay_ms != 40) {
        fprintf(stderr, "cluster restart policy helpers failed\n");
        return 1;
    }

    return 0;
}

int cluster_smoke_check_initial_shards(dcc_cluster_t *cluster) {
    for (uint32_t i = 0; i < 3; ++i) {
        dcc_cluster_shard_info_t shard_info = {
            .size = sizeof(shard_info),
        };
        dcc_status_t status = dcc_cluster_shard_info(cluster, i, &shard_info);
        if (status != DCC_OK ||
            shard_info.shard_id != i ||
            shard_info.shard_count != 3 ||
            shard_info.status != DCC_CLUSTER_SHARD_CREATED ||
            shard_info.starts != 0 ||
            strcmp(dcc_cluster_shard_status_string(shard_info.status), "created") != 0) {
            fprintf(stderr, "cluster initial shard info failed for shard %u\n", (unsigned)i);
            return 1;
        }

        dcc_gateway_info_t gateway_info = {
            .size = sizeof(gateway_info),
        };
        status = dcc_client_gateway_info(dcc_cluster_shard(cluster, i), &gateway_info);
        if (status != DCC_OK ||
            gateway_info.recommended_shards != 3 ||
            gateway_info.max_concurrency != 2) {
            fprintf(stderr, "cluster gateway info failed for shard %u\n", (unsigned)i);
            return 1;
        }
    }

    return 0;
}
