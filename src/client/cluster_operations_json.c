#include "internal/client/dcc_cluster_health_json_internal.h"

static const char *dcc_cluster_operation_type_name(dcc_cluster_operation_type_t type) {
    switch (type) {
        case DCC_CLUSTER_OPERATION_NONE:
            return "none";
        case DCC_CLUSTER_OPERATION_ROLLING_RECONNECT:
            return "rolling_reconnect";
        default:
            return "unknown";
    }
}

static int dcc_cluster_json_append_bool(dcc_cluster_json_buffer_t *json, uint8_t value) {
    return dcc_cluster_json_append_cstr(json, value != 0U ? "true" : "false");
}

int dcc_cluster_json_append_operation_status(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_operation_status_t *status
) {
    return dcc_cluster_json_append_cstr(json, "{\"type\":") != 0 ||
            dcc_cluster_json_append_string(json, dcc_cluster_operation_type_name(status->type)) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"generation\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->generation) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"active\":") != 0 ||
            dcc_cluster_json_append_bool(json, status->active) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"completed\":") != 0 ||
            dcc_cluster_json_append_bool(json, status->completed) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"resume\":") != 0 ||
            dcc_cluster_json_append_bool(json, status->resume) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"first_index\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->first_index) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"shard_count\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->shard_count) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"batch_size\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->batch_size) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"delay_ms\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->delay_ms) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"next_index\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->next_index) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"completed_shards\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->completed_shards) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"failed_shards\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->failed_shards) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"current_batch\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->current_batch) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"total_batches\":") != 0 ||
            dcc_cluster_json_append_u64(json, status->total_batches) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"last_status\":") != 0 ||
            dcc_cluster_json_append_string(json, dcc_status_string(status->last_status)) != 0 ||
            dcc_cluster_json_append_cstr(json, "}") != 0;
}

dcc_status_t dcc_cluster_operation_status_json(
    const dcc_cluster_operation_status_t *status,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (status == NULL || status->size < sizeof(*status) || out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_json_buffer_t json = {
        .data = out,
        .len = 0,
        .cap = out_size,
        .overflow = 0,
    };
    if (dcc_cluster_json_append_operation_status(&json, status) != 0) {
        return dcc_cluster_json_finish(&json, out_len);
    }
    return dcc_cluster_json_finish(&json, out_len);
}
