#include "internal/hot_reload/dcc_hot_reload_internal.h"
#include "internal/hot_reload/dcc_hot_reload_health_json_internal.h"

static const char *dcc_hot_reload_backend_json_string(dcc_hot_reload_backend_t backend) {
    switch (backend) {
        case DCC_HOT_RELOAD_BACKEND_IN_PROCESS:
            return "in_process";
        case DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER:
            return "isolated_worker";
        default:
            return "unknown";
    }
}

dcc_status_t dcc_hot_reload_health_snapshot_json(
    const dcc_hot_reload_health_snapshot_t *snapshot,
    char *out,
    size_t out_size,
    size_t *out_len
) {
    if (snapshot == NULL || snapshot->size < sizeof(*snapshot) || out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_hot_reload_json_buffer_t json = {
        .data = out,
        .len = 0,
        .cap = out_size,
        .overflow = 0,
    };
    if (dcc_hot_reload_json_append_cstr(&json, "{\"ok\":") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, snapshot->ok != 0U ? "true" : "false") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"backend\":") != 0 ||
        dcc_hot_reload_json_append_string(&json, dcc_hot_reload_backend_json_string(snapshot->backend)) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"health\":") != 0 ||
        dcc_hot_reload_json_append_string(&json, dcc_hot_reload_health_string(snapshot->health)) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"running\":") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, snapshot->running != 0U ? "true" : "false") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"loaded\":") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, snapshot->loaded != 0U ? "true" : "false") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"reloading\":") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, snapshot->reloading != 0U ? "true" : "false") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"pending_reload\":") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, snapshot->pending_reload != 0U ? "true" : "false") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"generation\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->generation) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"active_calls\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->active_calls) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"poll_interval_ms\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->poll_interval_ms) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"settle_interval_ms\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->settle_interval_ms) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"worker_supervisor_running\":") != 0 ||
        dcc_hot_reload_json_append_cstr(
            &json,
            snapshot->worker_supervisor_running != 0U ? "true" : "false"
        ) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"worker_health_timeout_ms\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_health_timeout_ms) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"worker_drain_timeout_ms\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_drain_timeout_ms) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"worker_events\":{\"active_failures\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_active_failures) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"last_good_promotions\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_last_good_promotions) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"dispatch_failures\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_dispatch_failures) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"temp_error_responses\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_temp_error_responses) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, "}") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"canary\":{\"enabled\":") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, snapshot->worker_canary_enabled != 0U ? "true" : "false") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"active\":") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, snapshot->worker_canary_active != 0U ? "true" : "false") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"percent\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_canary_percent) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"routed\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_canary_routed) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"successes\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_canary_successes) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"failures\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_canary_failures) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"promotions\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_canary_promotions) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"rollbacks\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->worker_canary_rollbacks) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"rollback_reason\":") != 0 ||
        dcc_hot_reload_json_append_string(
            &json,
            dcc_hot_reload_canary_rollback_reason_string(snapshot->worker_canary_rollback_reason)
        ) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, "}") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"active_worker\":{\"loaded\":") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, snapshot->active_worker_loaded != 0U ? "true" : "false") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"generation\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->active_worker_generation) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"pid\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->active_worker_pid) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, "},\"last_good_worker\":{\"loaded\":") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, snapshot->last_good_worker_loaded != 0U ? "true" : "false") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"generation\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->last_good_worker_generation) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"pid\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->last_good_worker_pid) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, "},\"candidate_worker\":{\"loaded\":") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, snapshot->candidate_worker_loaded != 0U ? "true" : "false") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"generation\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->candidate_worker_generation) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"pid\":") != 0 ||
        dcc_hot_reload_json_append_u64(&json, snapshot->candidate_worker_pid) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, "}") != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"last_status\":") != 0 ||
        dcc_hot_reload_json_append_string(&json, dcc_status_string(snapshot->last_status)) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"path\":") != 0 ||
        dcc_hot_reload_json_append_string(&json, snapshot->path) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"last_error\":") != 0 ||
        dcc_hot_reload_json_append_string(&json, snapshot->last_error) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, ",\"reason\":") != 0 ||
        dcc_hot_reload_json_append_string(&json, snapshot->reason) != 0 ||
        dcc_hot_reload_json_append_cstr(&json, "}") != 0) {
        return dcc_hot_reload_json_finish(&json, out_len);
    }
    return dcc_hot_reload_json_finish(&json, out_len);
}
