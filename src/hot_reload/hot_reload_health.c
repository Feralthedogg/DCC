#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <string.h>

const char *dcc_hot_reload_health_string(dcc_hot_reload_health_t health) {
    switch (health) {
        case DCC_HOT_RELOAD_HEALTH_STOPPED:
            return "stopped";
        case DCC_HOT_RELOAD_HEALTH_WATCHING:
            return "watching";
        case DCC_HOT_RELOAD_HEALTH_RELOADING:
            return "reloading";
        case DCC_HOT_RELOAD_HEALTH_DEGRADED:
            return "degraded";
        case DCC_HOT_RELOAD_HEALTH_UNKNOWN:
        default:
            return "unknown";
    }
}

dcc_status_t dcc_hot_reload_health_snapshot(
    const dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_health_snapshot_t *out
) {
    if (hot_reload == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_hot_reload_t *mutable_hot_reload = (dcc_hot_reload_t *)hot_reload;
    dcc_hot_reload_health_snapshot_t snapshot;
    memset(&snapshot, 0, sizeof(snapshot));
    snapshot.size = sizeof(snapshot);

    dcc_hot_reload_lock(mutable_hot_reload);
    snapshot.running = atomic_load_explicit(&mutable_hot_reload->watch_running, memory_order_acquire) ? 1U : 0U;
    snapshot.backend = mutable_hot_reload->backend;
    snapshot.loaded = mutable_hot_reload->backend == DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER
        ? dcc_hot_reload_worker_loaded(mutable_hot_reload)
        : (mutable_hot_reload->module.handle != NULL ? 1U : 0U);
    snapshot.reloading = mutable_hot_reload->reloading;
    snapshot.pending_reload = mutable_hot_reload->has_pending_file_sig;
    snapshot.worker_supervisor_running =
        atomic_load_explicit(&mutable_hot_reload->worker_supervisor_running, memory_order_acquire) ? 1U : 0U;
    snapshot.active_worker_loaded =
        mutable_hot_reload->active_worker != NULL && mutable_hot_reload->active_worker->loaded != 0U ? 1U : 0U;
    snapshot.last_good_worker_loaded =
        mutable_hot_reload->last_good_worker != NULL && mutable_hot_reload->last_good_worker->loaded != 0U ? 1U : 0U;
    snapshot.candidate_worker_loaded =
        mutable_hot_reload->candidate_worker != NULL && mutable_hot_reload->candidate_worker->loaded != 0U ? 1U : 0U;
    snapshot.worker_canary_enabled = mutable_hot_reload->worker_canary_enabled;
    snapshot.worker_canary_active = mutable_hot_reload->worker_canary.active;
    snapshot.worker_canary_percent = mutable_hot_reload->worker_canary.active != 0U
        ? mutable_hot_reload->worker_canary.options.canary_percent
        : mutable_hot_reload->worker_canary_options.canary_percent;
    snapshot.active_calls = mutable_hot_reload->active_calls;
    snapshot.poll_interval_ms = mutable_hot_reload->poll_interval_ms;
    snapshot.settle_interval_ms = mutable_hot_reload->settle_interval_ms;
    snapshot.worker_health_timeout_ms = mutable_hot_reload->worker_health_timeout_ms;
    snapshot.worker_drain_timeout_ms = mutable_hot_reload->worker_drain_timeout_ms;
    snapshot.generation = mutable_hot_reload->generation;
    snapshot.active_worker_generation =
        mutable_hot_reload->active_worker != NULL ? mutable_hot_reload->active_worker->generation : 0U;
    snapshot.last_good_worker_generation =
        mutable_hot_reload->last_good_worker != NULL ? mutable_hot_reload->last_good_worker->generation : 0U;
    snapshot.candidate_worker_generation =
        mutable_hot_reload->candidate_worker != NULL ? mutable_hot_reload->candidate_worker->generation : 0U;
    snapshot.active_worker_pid =
        mutable_hot_reload->active_worker != NULL ? mutable_hot_reload->active_worker->process_id : 0U;
    snapshot.last_good_worker_pid =
        mutable_hot_reload->last_good_worker != NULL ? mutable_hot_reload->last_good_worker->process_id : 0U;
    snapshot.candidate_worker_pid =
        mutable_hot_reload->candidate_worker != NULL ? mutable_hot_reload->candidate_worker->process_id : 0U;
    snapshot.worker_active_failures = mutable_hot_reload->worker_active_failures;
    snapshot.worker_last_good_promotions = mutable_hot_reload->worker_last_good_promotions;
    snapshot.worker_dispatch_failures = mutable_hot_reload->worker_dispatch_failures;
    snapshot.worker_temp_error_responses = mutable_hot_reload->worker_temp_error_responses;
    snapshot.worker_canary_routed = mutable_hot_reload->worker_canary.routed;
    snapshot.worker_canary_successes = mutable_hot_reload->worker_canary.successes;
    snapshot.worker_canary_failures = mutable_hot_reload->worker_canary.failures;
    snapshot.worker_canary_promotions = mutable_hot_reload->worker_canary_promotions;
    snapshot.worker_canary_rollbacks = mutable_hot_reload->worker_canary_rollbacks;
    snapshot.worker_canary_rollback_reason = mutable_hot_reload->worker_canary.rollback_reason;
    snapshot.last_status = mutable_hot_reload->last_status;
    dcc_hot_reload_copy_text(snapshot.path, sizeof(snapshot.path), mutable_hot_reload->path);
    dcc_hot_reload_copy_text(snapshot.last_error, sizeof(snapshot.last_error), mutable_hot_reload->last_error);
    dcc_hot_reload_unlock(mutable_hot_reload);

    snapshot.health = dcc_hot_reload_classify_health(
        snapshot.loaded,
        snapshot.running,
        snapshot.reloading,
        snapshot.last_status
    );
    snapshot.ok = snapshot.health == DCC_HOT_RELOAD_HEALTH_WATCHING ? 1U : 0U;
    dcc_hot_reload_copy_text(
        snapshot.reason,
        sizeof(snapshot.reason),
        dcc_hot_reload_health_reason(snapshot.health, snapshot.loaded, snapshot.pending_reload)
    );
    *out = snapshot;
    return DCC_OK;
}
