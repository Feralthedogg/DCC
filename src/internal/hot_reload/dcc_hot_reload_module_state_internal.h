#ifndef DCC_HOT_RELOAD_MODULE_STATE_INTERNAL_H
#define DCC_HOT_RELOAD_MODULE_STATE_INTERNAL_H

#include "internal/dcc_windows_internal.h"

#include <dcc/hot_reload.h>

#include <stddef.h>
#include <stdint.h>

typedef struct dcc_hot_reload_handler {
    dcc_event_type_t type;
    dcc_event_cb cb;
    void *user_data;
} dcc_hot_reload_handler_t;

typedef struct dcc_hot_reload_module {
#if defined(_WIN32)
    HMODULE handle;
#else
    void *handle;
#endif
    dcc_bot_module_deinit_fn deinit;
    dcc_hot_reload_handler_t *handlers;
    size_t handler_count;
    size_t handler_cap;
    char *loaded_path;
    uint64_t generation;
} dcc_hot_reload_module_t;

#endif
