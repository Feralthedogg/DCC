#ifndef DCC_HOT_RELOAD_MODULE_DESCRIPTOR_H
#define DCC_HOT_RELOAD_MODULE_DESCRIPTOR_H

#include <dcc/hot_reload/module/context.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int dcc_bot_module_descriptor_has(
    const dcc_bot_module_descriptor_t *descriptor,
    size_t offset,
    size_t field_size
) {
    return descriptor != NULL &&
           descriptor->size >= offset &&
           descriptor->size - offset >= field_size;
}

static inline dcc_status_t dcc_bot_module_init_from_descriptor(
    dcc_bot_module_ctx_t *ctx,
    const dcc_bot_module_descriptor_t *descriptor
) {
    if (ctx == NULL || descriptor == NULL || descriptor->size < sizeof(descriptor->size)) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t state_size = dcc_bot_module_descriptor_has(
        descriptor,
        offsetof(dcc_bot_module_descriptor_t, state_size),
        sizeof(descriptor->state_size)
    ) ? descriptor->state_size : 0U;
    size_t state_alignment = dcc_bot_module_descriptor_has(
        descriptor,
        offsetof(dcc_bot_module_descriptor_t, state_alignment),
        sizeof(descriptor->state_alignment)
    ) ? descriptor->state_alignment : 0U;
    const dcc_bot_module_handler_t *handlers = dcc_bot_module_descriptor_has(
        descriptor,
        offsetof(dcc_bot_module_descriptor_t, handlers),
        sizeof(descriptor->handlers)
    ) ? descriptor->handlers : NULL;
    size_t handler_count = dcc_bot_module_descriptor_has(
        descriptor,
        offsetof(dcc_bot_module_descriptor_t, handler_count),
        sizeof(descriptor->handler_count)
    ) ? descriptor->handler_count : 0U;
    dcc_bot_module_load_fn load = dcc_bot_module_descriptor_has(
        descriptor,
        offsetof(dcc_bot_module_descriptor_t, load),
        sizeof(descriptor->load)
    ) ? descriptor->load : NULL;

    if (handler_count != 0U && handlers == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    void *state = NULL;
    if (state_size != 0U) {
        state = dcc_bot_module_aligned_state(ctx, state_size, state_alignment);
        if (state == NULL) {
            return DCC_ERR_STATE;
        }
    }

    dcc_status_t status = load != NULL ? load(ctx, state) : DCC_OK;
    for (size_t i = 0; status == DCC_OK && i < handler_count; ++i) {
        status = dcc_bot_module_on(ctx, handlers[i].type, handlers[i].cb, state);
    }
    return status;
}

static inline void dcc_bot_module_deinit_from_descriptor(
    dcc_bot_module_ctx_t *ctx,
    const dcc_bot_module_descriptor_t *descriptor
) {
    if (ctx == NULL || descriptor == NULL || descriptor->size < sizeof(descriptor->size)) {
        return;
    }

    dcc_bot_module_unload_fn unload = dcc_bot_module_descriptor_has(
        descriptor,
        offsetof(dcc_bot_module_descriptor_t, unload),
        sizeof(descriptor->unload)
    ) ? descriptor->unload : NULL;
    if (unload == NULL) {
        return;
    }

    size_t state_alignment = dcc_bot_module_descriptor_has(
        descriptor,
        offsetof(dcc_bot_module_descriptor_t, state_alignment),
        sizeof(descriptor->state_alignment)
    ) ? descriptor->state_alignment : 0U;
    void *state = dcc_bot_module_aligned_state(ctx, 0U, state_alignment);
    unload(ctx, state);
}

#ifdef __cplusplus
}
#endif

#endif
