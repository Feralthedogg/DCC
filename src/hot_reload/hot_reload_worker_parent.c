#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdlib.h>
#include <stdint.h>

dcc_status_t dcc_hot_reload_worker_reload(dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_hot_reload_lock(hot_reload);
    if (hot_reload->reloading) {
        dcc_hot_reload_unlock(hot_reload);
        return DCC_ERR_STATE;
    }
    if (hot_reload->generation == UINT64_MAX) {
        dcc_hot_reload_set_error(
            hot_reload,
            DCC_ERR_STATE,
            "hot reload generation counter overflow"
        );
        dcc_hot_reload_broadcast(hot_reload);
        dcc_hot_reload_unlock(hot_reload);
        return DCC_ERR_STATE;
    }
    hot_reload->reloading = 1U;
    uint64_t next_generation = hot_reload->generation + 1U;
    dcc_hot_reload_unlock(hot_reload);

    dcc_hot_reload_worker_process_t *candidate =
        (dcc_hot_reload_worker_process_t *)calloc(1, sizeof(*candidate));
    if (candidate == NULL) {
        dcc_hot_reload_lock(hot_reload);
        hot_reload->reloading = 0U;
        hot_reload->last_status = DCC_ERR_NOMEM;
        dcc_hot_reload_broadcast(hot_reload);
        dcc_hot_reload_unlock(hot_reload);
        return DCC_ERR_NOMEM;
    }
    dcc_event_type_t handler_types[DCC_EVENT_MAX];
    size_t handler_count = 0U;
    dcc_status_t status = dcc_hot_reload_worker_process_spawn(
        hot_reload,
        next_generation,
        candidate,
        handler_types,
        &handler_count,
        DCC_EVENT_MAX
    );
    if (status != DCC_OK) {
        char last_error[sizeof(candidate->last_error)];
        dcc_hot_reload_copy_text(
            last_error,
            sizeof(last_error),
            candidate->last_error[0] != '\0' ? candidate->last_error : dcc_status_string(status)
        );
        free(candidate);
        dcc_hot_reload_lock(hot_reload);
        hot_reload->reloading = 0U;
        hot_reload->last_status = status;
        dcc_hot_reload_copy_text(hot_reload->last_error, sizeof(hot_reload->last_error), last_error);
        dcc_set_error(hot_reload->client, hot_reload->last_error);
        dcc_hot_reload_broadcast(hot_reload);
        dcc_hot_reload_unlock(hot_reload);
        return status;
    }

    for (size_t i = 0; i < handler_count; ++i) {
        status = dcc_hot_reload_install_trampoline(hot_reload, handler_types[i]);
        if (status != DCC_OK) {
            dcc_hot_reload_worker_process_stop(candidate, hot_reload->worker_drain_timeout_ms);
            free(candidate);
            dcc_hot_reload_lock(hot_reload);
            hot_reload->reloading = 0U;
            hot_reload->last_status = status;
            dcc_hot_reload_broadcast(hot_reload);
            dcc_hot_reload_unlock(hot_reload);
            return status;
        }
    }

    dcc_hot_reload_file_sig_t sig;
    uint8_t has_sig = dcc_hot_reload_file_sig(hot_reload->path, &sig) == DCC_OK ? 1U : 0U;

    dcc_hot_reload_lock(hot_reload);
    dcc_hot_reload_worker_process_t *old_last_good = hot_reload->last_good_worker;
    dcc_hot_reload_worker_process_t *old_candidate = hot_reload->candidate_worker;
    uint8_t start_canary = hot_reload->worker_canary_enabled &&
        hot_reload->active_worker != NULL ? 1U : 0U;
    if (start_canary) {
        hot_reload->candidate_worker = candidate;
        hot_reload->last_good_worker = NULL;
        (void)dcc_hot_reload_canary_start(
            &hot_reload->worker_canary,
            &hot_reload->worker_canary_options,
            next_generation,
            dcc_hot_reload_now_ms()
        );
    } else {
        hot_reload->last_good_worker = hot_reload->active_worker;
        hot_reload->active_worker = candidate;
        hot_reload->candidate_worker = NULL;
        dcc_hot_reload_canary_stop(&hot_reload->worker_canary);
    }
    hot_reload->generation = next_generation;
    if (has_sig) {
        hot_reload->file_sig = sig;
        hot_reload->has_file_sig = 1U;
    }
    hot_reload->has_pending_file_sig = 0U;
    hot_reload->pending_since_ms = 0;
    hot_reload->last_status = DCC_OK;
    hot_reload->last_error[0] = '\0';
    hot_reload->reloading = 0U;
    while (hot_reload->active_calls > 0) {
        dcc_hot_reload_wait_locked(hot_reload);
    }
    dcc_hot_reload_broadcast(hot_reload);
    dcc_hot_reload_unlock(hot_reload);

    dcc_hot_reload_worker_process_stop(old_last_good, hot_reload->worker_drain_timeout_ms);
    dcc_hot_reload_worker_process_stop(old_candidate, hot_reload->worker_drain_timeout_ms);
    free(old_last_good);
    free(old_candidate);
    dcc_hot_reload_log(
        hot_reload,
        DCC_LOG_INFO,
        start_canary ? "hot reload worker candidate loaded for canary" : "hot reload worker loaded"
    );
    return DCC_OK;
}
