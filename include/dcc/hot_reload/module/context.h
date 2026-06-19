#ifndef DCC_HOT_RELOAD_MODULE_CONTEXT_H
#define DCC_HOT_RELOAD_MODULE_CONTEXT_H

#include <dcc/hot_reload/module/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dcc_bot_module_ctx {
    size_t size;
    dcc_client_t *client;
    dcc_hot_reload_t *hot_reload;
    uint64_t generation;
    void *user_data;
    void *internal;
    dcc_status_t (*on)(
        dcc_bot_module_ctx_t *ctx,
        dcc_event_type_t type,
        dcc_event_cb cb,
        void *handler_user_data
    );
    void *(*state)(dcc_bot_module_ctx_t *ctx, size_t size, size_t alignment);
};

static inline dcc_client_t *dcc_bot_module_client(const dcc_bot_module_ctx_t *ctx) {
    return ctx != NULL ? ctx->client : NULL;
}

static inline void *dcc_bot_module_user_data(const dcc_bot_module_ctx_t *ctx) {
    return ctx != NULL ? ctx->user_data : NULL;
}

static inline uint64_t dcc_bot_module_generation(const dcc_bot_module_ctx_t *ctx) {
    return ctx != NULL ? ctx->generation : 0;
}

static inline dcc_status_t dcc_bot_module_on(
    dcc_bot_module_ctx_t *ctx,
    dcc_event_type_t type,
    dcc_event_cb cb,
    void *handler_user_data
) {
    return ctx != NULL && ctx->on != NULL
        ? ctx->on(ctx, type, cb, handler_user_data)
        : DCC_ERR_INVALID_ARG;
}

static inline void *dcc_bot_module_aligned_state(
    dcc_bot_module_ctx_t *ctx,
    size_t size,
    size_t alignment
) {
    return ctx != NULL && ctx->state != NULL ? ctx->state(ctx, size, alignment) : NULL;
}

static inline void *dcc_bot_module_state(dcc_bot_module_ctx_t *ctx, size_t size) {
    return dcc_bot_module_aligned_state(ctx, size, 0);
}

#ifdef __cplusplus
}
#endif

#endif
