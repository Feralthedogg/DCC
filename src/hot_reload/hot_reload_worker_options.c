#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stddef.h>

static dcc_status_t dcc_hot_reload_apply_backend_options(
    dcc_hot_reload_t *hot_reload,
    const dcc_hot_reload_options_t *options
) {
    hot_reload->backend = DCC_HOT_RELOAD_BACKEND_IN_PROCESS;
    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, backend),
            sizeof(options->backend)
        )) {
        hot_reload->backend = options->backend;
    }
    if (hot_reload->backend != DCC_HOT_RELOAD_BACKEND_IN_PROCESS &&
        hot_reload->backend != DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER) {
        return DCC_ERR_INVALID_ARG;
    }

    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, worker_path),
            sizeof(options->worker_path)
        ) && options->worker_path != NULL) {
        hot_reload->worker_path = dcc_strdup(options->worker_path);
        if (hot_reload->worker_path == NULL) {
            return DCC_ERR_NOMEM;
        }
    }
    if (hot_reload->backend == DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER &&
        (hot_reload->worker_path == NULL || hot_reload->worker_path[0] == '\0')) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_hot_reload_apply_worker_options(
    dcc_hot_reload_t *hot_reload,
    const dcc_hot_reload_options_t *options
) {
    dcc_status_t status = dcc_hot_reload_apply_backend_options(hot_reload, options);
    if (status != DCC_OK) {
        return status;
    }

    hot_reload->worker_health_timeout_ms = 2000U;
    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, worker_health_timeout_ms),
            sizeof(options->worker_health_timeout_ms)
        ) && options->worker_health_timeout_ms != 0U) {
        hot_reload->worker_health_timeout_ms = options->worker_health_timeout_ms;
    }
    hot_reload->worker_drain_timeout_ms = 500U;
    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, worker_drain_timeout_ms),
            sizeof(options->worker_drain_timeout_ms)
        )) {
        hot_reload->worker_drain_timeout_ms = options->worker_drain_timeout_ms;
    }

    hot_reload->worker_canary_enabled = 0U;
    dcc_hot_reload_canary_options_init(&hot_reload->worker_canary_options);
    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, worker_canary_enabled),
            sizeof(options->worker_canary_enabled)
        )) {
        hot_reload->worker_canary_enabled = options->worker_canary_enabled != 0U ? 1U : 0U;
    }
    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, worker_canary_options),
            sizeof(options->worker_canary_options)
        ) && options->worker_canary_options.size != 0U) {
        hot_reload->worker_canary_options = options->worker_canary_options;
    }
    hot_reload->worker_canary_options.size = sizeof(hot_reload->worker_canary_options);
    if (hot_reload->worker_canary_enabled &&
        dcc_hot_reload_canary_options_validate(&hot_reload->worker_canary_options) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }

    return DCC_OK;
}
