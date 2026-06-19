#include "internal/client/dcc_cluster_health_json_internal.h"

static int dcc_cluster_json_append_nullable_u32(
    dcc_cluster_json_buffer_t *json,
    uint32_t value
) {
    return value == DCC_CLUSTER_NO_SHARD_INDEX
        ? dcc_cluster_json_append_cstr(json, "null")
        : dcc_cluster_json_append_u64(json, value);
}

int dcc_cluster_json_append_recovery_plan(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_recovery_plan_t *plan
) {
    if (dcc_cluster_json_append_cstr(json, "{\"health\":") != 0 ||
        dcc_cluster_json_append_string(json, dcc_cluster_health_state_string(plan->health)) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"action\":") != 0 ||
        dcc_cluster_json_append_string(json, dcc_cluster_recovery_action_string(plan->action)) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"shard_index\":") != 0 ||
        dcc_cluster_json_append_nullable_u32(json, plan->shard_index) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"shard_id\":") != 0 ||
        dcc_cluster_json_append_nullable_u32(json, plan->shard_id) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"shard_status\":") != 0 ||
        dcc_cluster_json_append_string(json, dcc_cluster_shard_status_string(plan->shard_status)) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"reason\":") != 0 ||
        dcc_cluster_json_append_string(json, plan->reason) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"summary\":") != 0 ||
        dcc_cluster_json_append_health_summary(json, &plan->summary) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"shard\":") != 0) {
        return -1;
    }

    if (plan->shard_index == DCC_CLUSTER_NO_SHARD_INDEX) {
        if (dcc_cluster_json_append_cstr(json, "null") != 0) {
            return -1;
        }
    } else if (dcc_cluster_json_append_shard_info(json, &plan->shard) != 0) {
        return -1;
    }

    return dcc_cluster_json_append_cstr(json, "}");
}

dcc_status_t dcc_cluster_recovery_plan_json(
    const dcc_cluster_recovery_plan_t *plan,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (plan == NULL || plan->size < sizeof(*plan) || out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_json_buffer_t json = {
        .data = out,
        .len = 0,
        .cap = out_size,
        .overflow = 0,
    };
    if (dcc_cluster_json_append_recovery_plan(&json, plan) != 0) {
        return dcc_cluster_json_finish(&json, out_len);
    }
    return dcc_cluster_json_finish(&json, out_len);
}
