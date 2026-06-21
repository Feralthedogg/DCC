#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdint.h>

dcc_status_t dcc_hot_reload_poll(dcc_hot_reload_t *hot_reload) {
    if (hot_reload == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_hot_reload_file_sig_t sig;
    dcc_status_t status = dcc_hot_reload_file_sig(hot_reload->path, &sig);
    if (status != DCC_OK) {
        dcc_hot_reload_lock(hot_reload);
        dcc_hot_reload_set_error(hot_reload, status, "hot reload module path is not readable");
        hot_reload->has_pending_file_sig = 0U;
        hot_reload->pending_since_ms = 0U;
        dcc_hot_reload_broadcast(hot_reload);
        dcc_hot_reload_unlock(hot_reload);
        return status;
    }

    uint64_t now_ms = dcc_hot_reload_now_ms();
    dcc_hot_reload_lock(hot_reload);
    int changed = !hot_reload->has_file_sig || !dcc_hot_reload_same_sig(&hot_reload->file_sig, &sig);
    int loaded = hot_reload->backend == DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER
        ? dcc_hot_reload_worker_loaded(hot_reload)
        : hot_reload->module.handle != NULL;
    int first_load = hot_reload->generation == 0 && !loaded;
    if (!changed) {
        hot_reload->has_pending_file_sig = 0U;
        hot_reload->pending_since_ms = 0;
        dcc_hot_reload_unlock(hot_reload);
        return DCC_OK;
    }

    if (!first_load && hot_reload->settle_interval_ms != 0) {
        if (!hot_reload->has_pending_file_sig ||
            !dcc_hot_reload_same_sig(&hot_reload->pending_file_sig, &sig)) {
            hot_reload->pending_file_sig = sig;
            hot_reload->has_pending_file_sig = 1U;
            hot_reload->pending_since_ms = now_ms;
            dcc_hot_reload_unlock(hot_reload);
            return DCC_OK;
        }
        uint64_t elapsed_ms = now_ms >= hot_reload->pending_since_ms ? now_ms - hot_reload->pending_since_ms : 0;
        if (elapsed_ms < hot_reload->settle_interval_ms) {
            dcc_hot_reload_unlock(hot_reload);
            return DCC_OK;
        }
    }
    dcc_hot_reload_unlock(hot_reload);
    return dcc_hot_reload_reload(hot_reload);
}
