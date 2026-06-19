#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stddef.h>
#include <stdint.h>

uint8_t dcc_hot_reload_options_has_field(
    const dcc_hot_reload_options_t *options,
    size_t offset,
    size_t field_size
) {
    if (options == NULL || offset > SIZE_MAX - field_size) {
        return 0U;
    }
    return options->size >= offset + field_size ? 1U : 0U;
}

static const char *dcc_hot_reload_options_init_symbol(
    const dcc_hot_reload_options_t *options
) {
    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, init_symbol),
            sizeof(options->init_symbol)
        ) && options->init_symbol != NULL) {
        return options->init_symbol;
    }
    return DCC_BOT_MODULE_INIT_SYMBOL;
}

static const char *dcc_hot_reload_options_deinit_symbol(
    const dcc_hot_reload_options_t *options
) {
    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, deinit_symbol),
            sizeof(options->deinit_symbol)
        ) && options->deinit_symbol != NULL) {
        return options->deinit_symbol;
    }
    return DCC_BOT_MODULE_DEINIT_SYMBOL;
}

static void dcc_hot_reload_apply_timing_options(
    dcc_hot_reload_t *hot_reload,
    const dcc_hot_reload_options_t *options
) {
    hot_reload->poll_interval_ms = 250U;
    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, poll_interval_ms),
            sizeof(options->poll_interval_ms)
        ) && options->poll_interval_ms != 0U) {
        hot_reload->poll_interval_ms = options->poll_interval_ms;
    }

    hot_reload->settle_interval_ms = 100U;
    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, settle_interval_ms),
            sizeof(options->settle_interval_ms)
        ) && options->settle_interval_ms != 0U) {
        hot_reload->settle_interval_ms = options->settle_interval_ms == DCC_HOT_RELOAD_NO_SETTLE
            ? 0U
            : options->settle_interval_ms;
    }
}

dcc_status_t dcc_hot_reload_apply_create_options(
    dcc_hot_reload_t *hot_reload,
    const char *library_path,
    const dcc_hot_reload_options_t *options
) {
    hot_reload->path = dcc_strdup(library_path);
    hot_reload->init_symbol = dcc_strdup(dcc_hot_reload_options_init_symbol(options));
    hot_reload->deinit_symbol = dcc_strdup(dcc_hot_reload_options_deinit_symbol(options));
    if (hot_reload->path == NULL || hot_reload->init_symbol == NULL || hot_reload->deinit_symbol == NULL) {
        return DCC_ERR_NOMEM;
    }

    dcc_hot_reload_apply_timing_options(hot_reload, options);
    if (dcc_hot_reload_options_has_field(
            options,
            offsetof(dcc_hot_reload_options_t, user_data),
            sizeof(options->user_data)
        )) {
        hot_reload->user_data = options->user_data;
    }

    return dcc_hot_reload_apply_worker_options(hot_reload, options);
}
