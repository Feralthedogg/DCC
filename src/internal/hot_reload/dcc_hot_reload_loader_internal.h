#ifndef DCC_HOT_RELOAD_LOADER_INTERNAL_H
#define DCC_HOT_RELOAD_LOADER_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_module_state_internal.h"

#include <dcc/hot_reload.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*dcc_hot_reload_symbol_fn)(void);

dcc_status_t dcc_hot_reload_open_library(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    dcc_hot_reload_module_t *module
);
dcc_hot_reload_symbol_fn dcc_hot_reload_symbol(dcc_hot_reload_module_t *module, const char *name);
void dcc_hot_reload_close_library(dcc_hot_reload_module_t *module);

#ifdef __cplusplus
}
#endif

#endif
