#include "internal/hot_reload/dcc_hot_reload_module_trampoline_internal.h"
#include "internal/hot_reload/dcc_hot_reload_runtime_internal.h"
#include "internal/hot_reload/dcc_hot_reload_sync_internal.h"
#include "internal/hot_reload/dcc_hot_reload_worker_internal.h"

#include <stdlib.h>

static void dcc_hot_reload_trampoline(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    dcc_hot_reload_t *hot_reload = (dcc_hot_reload_t *)user_data;
    if (hot_reload == NULL || event == NULL) {
        return;
    }

    dcc_event_type_t type = dcc_event_type(event);
    if (type < 0 || type >= DCC_EVENT_MAX) {
        return;
    }

    if (hot_reload->backend == DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER) {
        dcc_hot_reload_lock(hot_reload);
        hot_reload->active_calls++;
        dcc_hot_reload_unlock(hot_reload);
        (void)dcc_hot_reload_worker_dispatch_event(hot_reload, event);
        dcc_hot_reload_lock(hot_reload);
        if (hot_reload->active_calls > 0) {
            hot_reload->active_calls--;
        }
        dcc_hot_reload_broadcast(hot_reload);
        dcc_hot_reload_unlock(hot_reload);
        return;
    }

    dcc_hot_reload_handler_t *snapshot = NULL;
    size_t snapshot_len = 0;

    dcc_hot_reload_lock(hot_reload);
    dcc_hot_reload_module_t *module = &hot_reload->module;
    for (size_t i = 0; i < module->handler_count; ++i) {
        if (module->handlers[i].type == type) {
            snapshot_len++;
        }
    }
    if (snapshot_len == 0) {
        dcc_hot_reload_unlock(hot_reload);
        return;
    }
    snapshot = (dcc_hot_reload_handler_t *)malloc(snapshot_len * sizeof(snapshot[0]));
    if (snapshot == NULL) {
        dcc_hot_reload_set_error(hot_reload, DCC_ERR_NOMEM, "hot reload handler snapshot allocation failed");
        dcc_hot_reload_unlock(hot_reload);
        return;
    }
    size_t cursor = 0;
    for (size_t i = 0; i < module->handler_count; ++i) {
        if (module->handlers[i].type == type) {
            snapshot[cursor++] = module->handlers[i];
        }
    }
    hot_reload->active_calls++;
    dcc_hot_reload_unlock(hot_reload);

    for (size_t i = 0; i < snapshot_len; ++i) {
        snapshot[i].cb(hot_reload->client, event, snapshot[i].user_data);
        if (dcc_event_is_cancelled(event)) {
            break;
        }
    }
    free(snapshot);

    dcc_hot_reload_lock(hot_reload);
    if (hot_reload->active_calls > 0) {
        hot_reload->active_calls--;
    }
    dcc_hot_reload_broadcast(hot_reload);
    dcc_hot_reload_unlock(hot_reload);
}

dcc_status_t dcc_hot_reload_install_trampoline(dcc_hot_reload_t *hot_reload, dcc_event_type_t type) {
    if (hot_reload == NULL || type < 0 || type >= DCC_EVENT_MAX) {
        return DCC_ERR_INVALID_ARG;
    }
    if (hot_reload->listener_installed[type]) {
        return DCC_OK;
    }

    dcc_listener_id_t id = 0;
    dcc_status_t status = dcc_client_on(hot_reload->client, type, dcc_hot_reload_trampoline, hot_reload, &id);
    if (status != DCC_OK) {
        return status;
    }
    hot_reload->listener_ids[type] = id;
    hot_reload->listener_installed[type] = 1U;
    return DCC_OK;
}
