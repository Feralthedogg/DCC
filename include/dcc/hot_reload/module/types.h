#ifndef DCC_HOT_RELOAD_MODULE_TYPES_H
#define DCC_HOT_RELOAD_MODULE_TYPES_H

#include <dcc/hot_reload/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_bot_module_handler {
    dcc_event_type_t type;
    dcc_event_cb cb;
} dcc_bot_module_handler_t;

typedef struct dcc_bot_module_descriptor {
    size_t size;
    size_t state_size;
    size_t state_alignment;
    const dcc_bot_module_handler_t *handlers;
    size_t handler_count;
    dcc_bot_module_load_fn load;
    dcc_bot_module_unload_fn unload;
} dcc_bot_module_descriptor_t;

#ifdef __cplusplus
}
#endif

#endif
