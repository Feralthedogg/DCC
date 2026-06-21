#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdint.h>

dcc_status_t dcc_hot_reload_reload(dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (hot_reload->backend == DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER) {
        return dcc_hot_reload_worker_reload(hot_reload);
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

    dcc_hot_reload_file_sig_t attempted_sig;
    uint8_t has_attempted_sig = dcc_hot_reload_file_sig(hot_reload->path, &attempted_sig) == DCC_OK ? 1U : 0U;

    dcc_hot_reload_module_t next;
    dcc_status_t status = dcc_hot_reload_load_next(hot_reload, next_generation, &next);
    if (status != DCC_OK) {
        dcc_hot_reload_rollback_result_t rollback =
            dcc_hot_reload_restore_previous_module_on_disk(hot_reload, next_generation);

        dcc_hot_reload_lock(hot_reload);
        hot_reload->reloading = 0U;
        hot_reload->has_pending_file_sig = 0U;
        hot_reload->pending_since_ms = 0;
        if (rollback.has_file_sig) {
            hot_reload->file_sig = rollback.file_sig;
            hot_reload->has_file_sig = 1U;
        } else if (has_attempted_sig) {
            hot_reload->file_sig = attempted_sig;
            hot_reload->has_file_sig = 1U;
        }
        if (hot_reload->last_error[0] != '\0') {
            dcc_set_error(hot_reload->client, hot_reload->last_error);
        }
        hot_reload->last_status = status;
        dcc_hot_reload_broadcast(hot_reload);
        dcc_hot_reload_unlock(hot_reload);
        if (rollback.has_file_sig) {
            dcc_hot_reload_log(
                hot_reload,
                DCC_LOG_WARN,
                "hot reload failed; restored previous module on disk and kept previous generation running"
            );
        } else if (rollback.attempted) {
            dcc_hot_reload_log(
                hot_reload,
                DCC_LOG_WARN,
                "hot reload failed; previous generation is still running but disk rollback failed"
            );
        }
        return status;
    }

    dcc_hot_reload_file_sig_t sig;
    uint8_t has_sig = dcc_hot_reload_file_sig(hot_reload->path, &sig) == DCC_OK ? 1U : 0U;

    dcc_hot_reload_module_t old;
    dcc_hot_reload_lock(hot_reload);
    old = hot_reload->module;
    hot_reload->module = next;
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

    dcc_hot_reload_module_deinit(hot_reload, &old);
    dcc_hot_reload_log(hot_reload, DCC_LOG_INFO, "hot reload module loaded");
    return DCC_OK;
}
