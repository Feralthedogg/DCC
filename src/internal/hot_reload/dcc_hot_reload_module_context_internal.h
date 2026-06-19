#ifndef DCC_HOT_RELOAD_MODULE_CONTEXT_INTERNAL_H
#define DCC_HOT_RELOAD_MODULE_CONTEXT_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_module_state_internal.h"
#include "internal/hot_reload/dcc_hot_reload_state_internal.h"

#include <dcc/hot_reload.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_bot_module_ctx_t dcc_hot_reload_ctx(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_module_t *module
);

#ifdef __cplusplus
}
#endif

#endif
