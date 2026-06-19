#ifndef DCC_HOT_RELOAD_MODULE_INTERNAL_H
#define DCC_HOT_RELOAD_MODULE_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_module_state_internal.h"
#include "internal/hot_reload/dcc_hot_reload_state_internal.h"

#include <dcc/hot_reload.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_hot_reload_module_deinit(dcc_hot_reload_t *hot_reload, dcc_hot_reload_module_t *module);
dcc_status_t dcc_hot_reload_load_next(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    dcc_hot_reload_module_t *out
);

#ifdef __cplusplus
}
#endif

#endif
