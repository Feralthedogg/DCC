#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stddef.h>
#include <string.h>

static void dcc_hot_reload_run_resolve_hot_reload_options(
    const dcc_hot_reload_run_options_t *options,
    dcc_hot_reload_options_t *storage,
    const dcc_hot_reload_options_t **out
) {
    memset(storage, 0, sizeof(*storage));
    *out = NULL;
    if (!dcc_hot_reload_run_options_has(
            options,
            offsetof(dcc_hot_reload_run_options_t, hot_reload_options),
            sizeof(options->hot_reload_options)
        )) {
        return;
    }

    *storage = options->hot_reload_options;
    storage->size = sizeof(*storage);
    if (storage->backend == DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER &&
        (storage->worker_path == NULL || storage->worker_path[0] == '\0')) {
        storage->worker_path = dcc_hot_reload_run_worker_path_or_default(NULL);
    }
    *out = storage;
}

dcc_status_t dcc_hot_reload_run_resolve_options(
    const dcc_hot_reload_run_options_t *options,
    dcc_client_options_t *client_options,
    const char **library_path,
    dcc_hot_reload_options_t *hot_reload_options,
    const dcc_hot_reload_options_t **hot_reload_options_ptr
) {
    dcc_status_t status = dcc_hot_reload_run_resolve_client_options(options, client_options);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_hot_reload_run_resolve_library_path(options, library_path);
    if (status != DCC_OK) {
        return status;
    }

    dcc_hot_reload_run_resolve_hot_reload_options(
        options,
        hot_reload_options,
        hot_reload_options_ptr
    );
    if (*hot_reload_options_ptr != NULL &&
        (*hot_reload_options_ptr)->backend == DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER &&
        ((*hot_reload_options_ptr)->worker_path == NULL ||
         (*hot_reload_options_ptr)->worker_path[0] == '\0')) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}
