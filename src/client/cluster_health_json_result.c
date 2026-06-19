#include "internal/client/dcc_cluster_health_json_internal.h"

int dcc_cluster_json_append_remediation_result(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_remediation_result_t *result
) {
    if (dcc_cluster_json_append_cstr(json, "{\"action_status\":") != 0 ||
        dcc_cluster_json_append_string(json, dcc_status_string(result->action_status)) != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"action_attempted\":") != 0 ||
        dcc_cluster_json_append_cstr(json, result->action_attempted != 0U ? "true" : "false") != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"action_allowed\":") != 0 ||
        dcc_cluster_json_append_cstr(json, result->action_allowed != 0U ? "true" : "false") != 0 ||
        dcc_cluster_json_append_cstr(json, ",\"plan\":") != 0 ||
        dcc_cluster_json_append_recovery_plan(json, &result->plan) != 0 ||
        dcc_cluster_json_append_cstr(json, "}") != 0) {
        return -1;
    }
    return 0;
}

dcc_status_t dcc_cluster_remediation_result_json(
    const dcc_cluster_remediation_result_t *result,
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
    if (dcc_cluster_json_append_remediation_result(&json, result) != 0) {
        return dcc_cluster_json_finish(&json, out_len);
    }
    return dcc_cluster_json_finish(&json, out_len);
}
