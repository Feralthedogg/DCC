#ifndef DCC_HOT_RELOAD_MODULE_MACROS_H
#define DCC_HOT_RELOAD_MODULE_MACROS_H

#include <dcc/hot_reload/module/descriptor.h>

#if defined(__cplusplus)
#  define DCC_BOT_MODULE_ALIGNOF(type) alignof(type)
#else
#  define DCC_BOT_MODULE_ALIGNOF(type) _Alignof(type)
#endif

#define DCC_BOT_MODULE_ON(event_type, callback) \
    { (event_type), (callback) }

#define DCC_BOT_MODULE_EXPORT(descriptor_name) \
    DCC_MODULE_API dcc_status_t dcc_bot_module_init(dcc_bot_module_ctx_t *ctx) { \
        return dcc_bot_module_init_from_descriptor(ctx, &(descriptor_name)); \
    } \
    DCC_MODULE_API void dcc_bot_module_deinit(dcc_bot_module_ctx_t *ctx) { \
        dcc_bot_module_deinit_from_descriptor(ctx, &(descriptor_name)); \
    }

#define DCC_BOT_MODULE_WITH_STATE(state_type, handler_array, load_fn, unload_fn) \
    static const dcc_bot_module_descriptor_t dcc_bot_module_descriptor = { \
        sizeof(dcc_bot_module_descriptor_t), \
        sizeof(state_type), \
        DCC_BOT_MODULE_ALIGNOF(state_type), \
        (handler_array), \
        sizeof(handler_array) / sizeof((handler_array)[0]), \
        (load_fn), \
        (unload_fn), \
    }; \
    DCC_BOT_MODULE_EXPORT(dcc_bot_module_descriptor)

#define DCC_BOT_MODULE_STATE(state_type, handler_array) \
    DCC_BOT_MODULE_WITH_STATE(state_type, (handler_array), NULL, NULL)

#define DCC_BOT_MODULE_STATELESS(handler_array, load_fn, unload_fn) \
    static const dcc_bot_module_descriptor_t dcc_bot_module_descriptor = { \
        sizeof(dcc_bot_module_descriptor_t), \
        0U, \
        0U, \
        (handler_array), \
        sizeof(handler_array) / sizeof((handler_array)[0]), \
        (load_fn), \
        (unload_fn), \
    }; \
    DCC_BOT_MODULE_EXPORT(dcc_bot_module_descriptor)

#define DCC_BOT_MODULE(handler_array) \
    DCC_BOT_MODULE_STATELESS((handler_array), NULL, NULL)

#endif
