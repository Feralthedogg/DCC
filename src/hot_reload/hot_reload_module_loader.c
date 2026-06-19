#include "internal/hot_reload/dcc_hot_reload_internal.h"
#include "internal/hot_reload/dcc_hot_reload_module_context_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dcc_hot_reload_module_deinit(dcc_hot_reload_t *hot_reload, dcc_hot_reload_module_t *module) {
    if (module == NULL) {
        return;
    }
    if (module->deinit != NULL) {
        dcc_bot_module_ctx_t ctx = dcc_hot_reload_ctx(hot_reload, module);
        module->deinit(&ctx);
    }
    dcc_hot_reload_close_library(module);
    free(module->handlers);
    free(module->loaded_path);
    memset(module, 0, sizeof(*module));
}

dcc_status_t dcc_hot_reload_load_next(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    dcc_hot_reload_module_t *out
) {
    memset(out, 0, sizeof(*out));
    out->generation = generation;
    dcc_status_t status = dcc_hot_reload_open_library(hot_reload, generation, out);
    if (status != DCC_OK) {
        return status;
    }

    union {
        dcc_hot_reload_symbol_fn symbol;
        dcc_bot_module_init_fn init;
    } init_cast;
    init_cast.symbol = dcc_hot_reload_symbol(out, hot_reload->init_symbol);
    if (init_cast.init == NULL) {
        snprintf(
            hot_reload->last_error,
            sizeof(hot_reload->last_error),
            "hot reload module is missing symbol %s",
            hot_reload->init_symbol
        );
        dcc_hot_reload_module_deinit(hot_reload, out);
        return DCC_ERR_STATE;
    }

    union {
        dcc_hot_reload_symbol_fn symbol;
        dcc_bot_module_deinit_fn deinit;
    } deinit_cast;
    deinit_cast.symbol = dcc_hot_reload_symbol(out, hot_reload->deinit_symbol);
    out->deinit = deinit_cast.deinit;

    dcc_bot_module_ctx_t ctx = dcc_hot_reload_ctx(hot_reload, out);
    status = init_cast.init(&ctx);
    if (status != DCC_OK) {
        snprintf(
            hot_reload->last_error,
            sizeof(hot_reload->last_error),
            "hot reload module init failed: %s",
            dcc_status_string(status)
        );
        dcc_hot_reload_module_deinit(hot_reload, out);
        return status;
    }
    return DCC_OK;
}
