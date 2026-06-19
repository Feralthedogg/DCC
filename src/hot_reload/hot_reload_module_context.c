#include "internal/hot_reload/dcc_hot_reload_module_context_internal.h"
#include "internal/hot_reload/dcc_hot_reload_module_trampoline_internal.h"
#include "internal/hot_reload/dcc_hot_reload_state_api_internal.h"

#include <stdint.h>
#include <stdlib.h>

static dcc_status_t dcc_hot_reload_ctx_on(
    dcc_bot_module_ctx_t *ctx,
    dcc_event_type_t type,
    dcc_event_cb cb,
    void *handler_user_data
) {
    if (ctx == NULL || ctx->hot_reload == NULL || ctx->internal == NULL || cb == NULL ||
        type < 0 || type >= DCC_EVENT_MAX) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_hot_reload_t *hot_reload = ctx->hot_reload;
    dcc_hot_reload_module_t *module = (dcc_hot_reload_module_t *)ctx->internal;
    if (module->handler_count == module->handler_cap) {
        size_t next_cap = 4U;
        if (module->handler_cap != 0U) {
            if (module->handler_cap > SIZE_MAX / 2U) {
                return DCC_ERR_NOMEM;
            }
            next_cap = module->handler_cap * 2U;
        }
        if (next_cap > SIZE_MAX / sizeof(module->handlers[0])) {
            return DCC_ERR_NOMEM;
        }
        dcc_hot_reload_handler_t *next =
            (dcc_hot_reload_handler_t *)realloc(module->handlers, next_cap * sizeof(module->handlers[0]));
        if (next == NULL) {
            return DCC_ERR_NOMEM;
        }
        module->handlers = next;
        module->handler_cap = next_cap;
    }

    dcc_status_t status = dcc_hot_reload_install_trampoline(hot_reload, type);
    if (status != DCC_OK) {
        return status;
    }

    module->handlers[module->handler_count++] = (dcc_hot_reload_handler_t){
        .type = type,
        .cb = cb,
        .user_data = handler_user_data,
    };
    return DCC_OK;
}

static void *dcc_hot_reload_ctx_state(dcc_bot_module_ctx_t *ctx, size_t size, size_t alignment) {
    if (ctx == NULL || ctx->hot_reload == NULL) {
        return NULL;
    }
    return dcc_hot_reload_state_reserve(ctx->hot_reload, size, alignment);
}

dcc_bot_module_ctx_t dcc_hot_reload_ctx(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_module_t *module
) {
    return (dcc_bot_module_ctx_t){
        .size = sizeof(dcc_bot_module_ctx_t),
        .client = hot_reload->client,
        .hot_reload = hot_reload,
        .generation = module != NULL ? module->generation : hot_reload->generation,
        .user_data = hot_reload->user_data,
        .internal = module,
        .on = dcc_hot_reload_ctx_on,
        .state = dcc_hot_reload_ctx_state,
    };
}
