#include "internal/client/dcc_cluster_health_json_internal.h"

dcc_status_t dcc_cluster_remediation_loop_result_json(
    const dcc_cluster_remediation_loop_result_t *result,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (result == NULL || result->size < sizeof(*result) || out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_json_buffer_t json = {
        .data = out,
        .len = 0,
        .cap = out_size,
        .overflow = 0,
    };
    if (dcc_cluster_json_append_cstr(&json, "{\"last_status\":") != 0 ||
        dcc_cluster_json_append_string(&json, dcc_status_string(result->last_status)) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"attempts\":") != 0 ||
        dcc_cluster_json_append_u64(&json, result->attempts) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"actions_attempted\":") != 0 ||
        dcc_cluster_json_append_u64(&json, result->actions_attempted) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"final_health\":") != 0 ||
        dcc_cluster_json_append_string(&json, dcc_cluster_health_state_string(result->final_health)) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"final_action\":") != 0 ||
        dcc_cluster_json_append_string(&json, dcc_cluster_recovery_action_string(result->final_action)) != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"reached_healthy\":") != 0 ||
        dcc_cluster_json_append_cstr(&json, result->reached_healthy != 0U ? "true" : "false") != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"exhausted_attempts\":") != 0 ||
        dcc_cluster_json_append_cstr(&json, result->exhausted_attempts != 0U ? "true" : "false") != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"stopped_no_action\":") != 0 ||
        dcc_cluster_json_append_cstr(&json, result->stopped_no_action != 0U ? "true" : "false") != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"stopped_error\":") != 0 ||
        dcc_cluster_json_append_cstr(&json, result->stopped_error != 0U ? "true" : "false") != 0 ||
        dcc_cluster_json_append_cstr(&json, ",\"last\":") != 0 ||
        dcc_cluster_json_append_remediation_result(&json, &result->last) != 0 ||
        dcc_cluster_json_append_cstr(&json, "}") != 0) {
        return dcc_cluster_json_finish(&json, out_len);
    }
    return dcc_cluster_json_finish(&json, out_len);
}
