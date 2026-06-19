#include "internal/client/dcc_cluster_health_json_internal.h"

static int dcc_cluster_status_json_append_bool(dcc_cluster_json_buffer_t *json, uint8_t value) {
    return dcc_cluster_json_append_cstr(json, value ? "true" : "false");
}

int dcc_cluster_json_append_supervisor_status(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_supervisor_status_t *status
) {
    if (dcc_cluster_json_append_cstr(json, "{\"decisions\":") != 0 ||
        dcc_cluster_json_append_u64(json, status->decisions) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"restart_scheduled\":") != 0 ||
        dcc_cluster_json_append_u64(json, status->restart_scheduled) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"restart_suppressed\":") != 0 ||
        dcc_cluster_json_append_u64(json, status->restart_suppressed) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"policy_errors\":") != 0 ||
        dcc_cluster_json_append_u64(json, status->policy_errors) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"last_shard_index\":") != 0) {
        return -1;
    }
    if (status->last_shard_index == DCC_CLUSTER_NO_SHARD_INDEX) {
        if (dcc_cluster_json_append_cstr(json, "null") != 0) {
            return -1;
        }
    } else if (dcc_cluster_json_append_u64(json, status->last_shard_index) != 0) {
        return -1;
    }
    if (dcc_cluster_json_append_cstr(json, ",\"last_shard_id\":") != 0) {
        return -1;
    }
    if (status->last_shard_id == DCC_CLUSTER_NO_SHARD_INDEX) {
        if (dcc_cluster_json_append_cstr(json, "null") != 0) {
            return -1;
        }
    } else if (dcc_cluster_json_append_u64(json, status->last_shard_id) != 0) {
        return -1;
    }
    return dcc_cluster_json_append_cstr(json, ",\"last_gateway_status\":") != 0 ||
            dcc_cluster_json_append_string(json, dcc_status_string(status->last_gateway_status)) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"last_action_status\":") != 0 ||
            dcc_cluster_json_append_string(json, dcc_status_string(status->last_action_status)) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"last_decision\":") != 0 ||
            dcc_cluster_json_append_string(json, dcc_cluster_supervisor_decision_string(status->last_decision)) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"last_delay_ms\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->last_delay_ms) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"last_restart_scheduled\":") != 0 ||
            dcc_cluster_status_json_append_bool(json, status->last_restart_scheduled) != 0 ||
            dcc_cluster_json_append_cstr(json, "}") != 0;
}

dcc_status_t dcc_cluster_status_json(
    const dcc_cluster_t *cluster,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (cluster == NULL || out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_recovery_plan_t plan = {
        .size = sizeof(plan),
    };
    dcc_status_t status = dcc_cluster_recovery_plan(cluster, &plan);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cluster_operation_status_t operation = {
        .size = sizeof(operation),
    };
    status = dcc_cluster_operation_status(cluster, &operation);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cluster_supervisor_status_t supervisor = {
        .size = sizeof(supervisor),
    };
    status = dcc_cluster_supervisor_status(cluster, &supervisor);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cluster_json_buffer_t json = {
        .data = out,
        .len = 0,
        .cap = out_size,
        .overflow = 0,
    };
    if (dcc_cluster_json_append_cstr(&json, "{\"recovery\":") != 0 ||
        dcc_cluster_json_append_recovery_plan(&json, &plan) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"operation\":") != 0 ||
        dcc_cluster_json_append_operation_status(&json, &operation) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"supervisor\":") != 0 ||
        dcc_cluster_json_append_supervisor_status(&json, &supervisor) != 0 ||
        dcc_cluster_json_append_cstr(&json, "}") != 0) {
        return dcc_cluster_json_finish(&json, out_len);
    }
    return dcc_cluster_json_finish(&json, out_len);
}
