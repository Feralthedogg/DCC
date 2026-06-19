#include "internal/client/dcc_cluster_health_json_internal.h"

static int dcc_cluster_json_append_shard_counters(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_shard_info_t *shard
) {
    return dcc_cluster_json_append_cstr(json, ",\"starts\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->starts) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"restart_attempts\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->restart_attempts) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"restarts\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->restarts) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"restart_suppressed\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->restart_suppressed) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"restart_budget_used\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->restart_budget_used) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"ready_events\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->ready_events) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"resumed_events\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->resumed_events) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"socket_closes\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->socket_closes) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"log_errors\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->log_errors) != 0;
}

int dcc_cluster_json_append_shard_info(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_shard_info_t *shard
) {
    return dcc_cluster_json_append_cstr(json, "{\"shard_id\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->shard_id) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"shard_count\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->shard_count) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"status\":") != 0 ||
            dcc_cluster_json_append_string(json, dcc_cluster_shard_status_string(shard->status)) != 0 ||
            dcc_cluster_json_append_shard_counters(json, shard) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"last_close_code\":") != 0 ||
            dcc_cluster_json_append_u64(json, shard->last_close_code) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"last_close_reason\":") != 0 ||
            dcc_cluster_json_append_string(json, shard->last_close_reason) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"last_error\":") != 0 ||
            dcc_cluster_json_append_string(json, shard->last_error) != 0 ||
            dcc_cluster_json_append_cstr(json, "}") != 0;
}
