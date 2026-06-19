#include "internal/client/dcc_cluster_health_json_internal.h"

static int dcc_cluster_json_append_summary_counts(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_health_summary_t *summary
) {
    return dcc_cluster_json_append_cstr(json, ",\"created_shards\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->created_shards) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"starting_shards\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->starting_shards) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"ready_shards\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->ready_shards) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"stopping_shards\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->stopping_shards) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"stopped_shards\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->stopped_shards) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"error_shards\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->error_shards) != 0;
}

static int dcc_cluster_json_append_summary_counters(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_health_summary_t *summary
) {
    return dcc_cluster_json_append_cstr(json, ",\"starts\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->starts) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"restart_attempts\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->restart_attempts) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"restarts\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->restarts) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"restart_suppressed\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->restart_suppressed) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"restart_budget_used\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->restart_budget_used) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"ready_events\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->ready_events) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"resumed_events\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->resumed_events) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"socket_closes\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->socket_closes) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"log_errors\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->log_errors) != 0;
}

int dcc_cluster_json_append_health_summary(
    dcc_cluster_json_buffer_t *json,
    const dcc_cluster_health_summary_t *summary
) {
    return dcc_cluster_json_append_cstr(json, "{\"health\":") != 0 ||
            dcc_cluster_json_append_string(json, dcc_cluster_health_state_string(
                dcc_cluster_health_summary_state(summary)
            )) != 0 ||
            dcc_cluster_json_append_cstr(json, ",\"total_shards\":") != 0 ||
            dcc_cluster_json_append_u64(json, summary->total_shards) != 0 ||
            dcc_cluster_json_append_summary_counts(json, summary) != 0 ||
            dcc_cluster_json_append_summary_counters(json, summary) != 0 ||
            dcc_cluster_json_append_cstr(json, "}") != 0;
}

dcc_status_t dcc_cluster_health_summary_json(
    const dcc_cluster_health_summary_t *summary,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (summary == NULL || summary->size < sizeof(*summary) || out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_cluster_json_buffer_t json = {
        .data = out,
        .len = 0,
        .cap = out_size,
        .overflow = 0,
    };
    if (dcc_cluster_json_append_health_summary(&json, summary) != 0) {
        return dcc_cluster_json_finish(&json, out_len);
    }
    return dcc_cluster_json_finish(&json, out_len);
}
