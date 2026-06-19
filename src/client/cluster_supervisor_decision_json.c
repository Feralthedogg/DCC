#include "internal/client/dcc_cluster_health_json_internal.h"

static int dcc_cluster_supervisor_json_append_bool(
    dcc_cluster_json_buffer_t *json,
    uint8_t value
) {
    return dcc_cluster_json_append_cstr(json, value != 0U ? "true" : "false");
}

static int dcc_cluster_supervisor_json_append_nullable_u32(
    dcc_cluster_json_buffer_t *json,
    uint32_t value
) {
    return value == DCC_CLUSTER_NO_SHARD_INDEX
        ? dcc_cluster_json_append_cstr(json, "null")
        : dcc_cluster_json_append_u64(json, value);
}

static int dcc_cluster_supervisor_json_append_restart_policy(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_restart_policy_t *policy
) {
    return dcc_cluster_json_append_cstr(json, "{\"max_restarts\":") != 0 ||
            dcc_cluster_json_append_u64(json, policy->max_restarts) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"base_delay_ms\":") != 0 ||
            dcc_cluster_json_append_u64(json, policy->base_delay_ms) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"max_delay_ms\":") != 0 ||
            dcc_cluster_json_append_u64(json, policy->max_delay_ms) != 0 ||
            dcc_cluster_json_append_cstr(json, "}") != 0;
}

static int dcc_cluster_supervisor_json_append_action(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_supervisor_action_t *action
) {
    return dcc_cluster_json_append_cstr(json, "{\"decision\":") != 0 ||
            dcc_cluster_json_append_string(
                json,
                dcc_cluster_supervisor_decision_string(action->decision)
            ) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"delay_ms\":") != 0 ||
            dcc_cluster_json_append_u64(json, action->delay_ms) != 0 ||
            dcc_cluster_json_append_cstr(json, "}") != 0;
}

dcc_status_t dcc_cluster_supervisor_decision_event_json(
    const dcc_cluster_supervisor_decision_event_t *event,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (event == NULL || event->size < sizeof(*event) || out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_json_buffer_t json = {
        .data = out,
        .len = 0,
        .cap = out_size,
        .overflow = 0,
    };
    if (dcc_cluster_json_append_cstr(&json, "{\"shard_index\":") != 0 ||
        dcc_cluster_supervisor_json_append_nullable_u32(&json, event->shard_index) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"shard_id\":") != 0 ||
        dcc_cluster_supervisor_json_append_nullable_u32(&json, event->shard_id) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"gateway_status\":") != 0 ||
        dcc_cluster_json_append_string(&json, dcc_status_string(event->gateway_status)) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"action_status\":") != 0 ||
        dcc_cluster_json_append_string(&json, dcc_status_string(event->action_status)) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"restart_scheduled\":") != 0 ||
        dcc_cluster_supervisor_json_append_bool(&json, event->restart_scheduled) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"restart_suppressed\":") != 0 ||
        dcc_cluster_supervisor_json_append_bool(&json, event->restart_suppressed) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"action\":") != 0 ||
        dcc_cluster_supervisor_json_append_action(&json, &event->action) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"restart_policy\":") != 0 ||
        dcc_cluster_supervisor_json_append_restart_policy(&json, &event->restart_policy) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"status\":") != 0 ||
        dcc_cluster_json_append_supervisor_status(&json, &event->status) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"shard\":") != 0 ||
        dcc_cluster_json_append_shard_info(&json, &event->shard) != 0 ||
        dcc_cluster_json_append_cstr(&json, "}") != 0) {
        return dcc_cluster_json_finish(&json, out_len);
    }
    return dcc_cluster_json_finish(&json, out_len);
}
