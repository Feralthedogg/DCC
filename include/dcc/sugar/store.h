#ifndef DCC_SUGAR_STORE_H
#define DCC_SUGAR_STORE_H

#include <dcc/app.h>
#include <dcc/sugar/status.h>
#include <dcc/store.h>
#include <stdint.h>

static inline dcc_store_t *dcc_sugar_app_store(dcc_app_t *app) {
    return app != NULL ? dcc_app_store(app) : NULL;
}

static inline dcc_store_t *dcc_sugar_ctx_store(const dcc_ctx_t *ctx) {
    return ctx != NULL ? dcc_sugar_app_store(dcc_ctx_app(ctx)) : NULL;
}

static inline uint8_t dcc_sugar_app_has_store(dcc_app_t *app) {
    return dcc_sugar_app_store(app) != NULL ? 1U : 0U;
}

static inline uint8_t dcc_sugar_ctx_has_store(const dcc_ctx_t *ctx) {
    return dcc_sugar_ctx_store(ctx) != NULL ? 1U : 0U;
}

static inline dcc_status_t dcc_sugar_app_store_get(
    dcc_app_t *app,
    const char *key,
    char *out_value,
    size_t out_value_size,
    size_t *out_value_len
) {
    dcc_store_t *store = dcc_sugar_app_store(app);
    return store != NULL
        ? dcc_store_get(store, key, out_value, out_value_size, out_value_len)
        : DCC_ERR_STATE;
}

static inline dcc_status_t dcc_sugar_ctx_store_get(
    const dcc_ctx_t *ctx,
    const char *key,
    char *out_value,
    size_t out_value_size,
    size_t *out_value_len
) {
    dcc_store_t *store = dcc_sugar_ctx_store(ctx);
    return store != NULL
        ? dcc_store_get(store, key, out_value, out_value_size, out_value_len)
        : DCC_ERR_STATE;
}

static inline dcc_status_t dcc_sugar_app_store_set(
    dcc_app_t *app,
    const char *key,
    const char *value
) {
    dcc_store_t *store = dcc_sugar_app_store(app);
    return store != NULL ? dcc_store_set(store, key, value) : DCC_ERR_STATE;
}

static inline dcc_status_t dcc_sugar_ctx_store_set(
    const dcc_ctx_t *ctx,
    const char *key,
    const char *value
) {
    dcc_store_t *store = dcc_sugar_ctx_store(ctx);
    return store != NULL ? dcc_store_set(store, key, value) : DCC_ERR_STATE;
}

static inline dcc_status_t dcc_sugar_app_store_delete(dcc_app_t *app, const char *key) {
    dcc_store_t *store = dcc_sugar_app_store(app);
    return store != NULL ? dcc_store_delete(store, key) : DCC_ERR_STATE;
}

static inline dcc_status_t dcc_sugar_ctx_store_delete(const dcc_ctx_t *ctx, const char *key) {
    dcc_store_t *store = dcc_sugar_ctx_store(ctx);
    return store != NULL ? dcc_store_delete(store, key) : DCC_ERR_STATE;
}

static inline dcc_status_t dcc_sugar_app_store_get_u64(
    dcc_app_t *app,
    const char *key,
    uint64_t *out_value
) {
    dcc_store_t *store = dcc_sugar_app_store(app);
    return store != NULL ? dcc_store_get_u64(store, key, out_value) : DCC_ERR_STATE;
}

static inline dcc_status_t dcc_sugar_ctx_store_get_u64(
    const dcc_ctx_t *ctx,
    const char *key,
    uint64_t *out_value
) {
    dcc_store_t *store = dcc_sugar_ctx_store(ctx);
    return store != NULL ? dcc_store_get_u64(store, key, out_value) : DCC_ERR_STATE;
}

static inline dcc_status_t dcc_sugar_app_store_set_u64(
    dcc_app_t *app,
    const char *key,
    uint64_t value
) {
    dcc_store_t *store = dcc_sugar_app_store(app);
    return store != NULL ? dcc_store_set_u64(store, key, value) : DCC_ERR_STATE;
}

static inline dcc_status_t dcc_sugar_ctx_store_set_u64(
    const dcc_ctx_t *ctx,
    const char *key,
    uint64_t value
) {
    dcc_store_t *store = dcc_sugar_ctx_store(ctx);
    return store != NULL ? dcc_store_set_u64(store, key, value) : DCC_ERR_STATE;
}

static inline uint64_t dcc_sugar_store_u64_or(
    const dcc_store_t *store,
    const char *key,
    uint64_t fallback
) {
    uint64_t value = fallback;
    if (dcc_store_get_u64(store, key, &value) != DCC_OK) {
        return fallback;
    }
    return value;
}

static inline uint64_t dcc_sugar_app_store_u64_or(
    dcc_app_t *app,
    const char *key,
    uint64_t fallback
) {
    dcc_store_t *store = dcc_sugar_app_store(app);
    return store != NULL ? dcc_sugar_store_u64_or(store, key, fallback) : fallback;
}

static inline uint64_t dcc_sugar_ctx_store_u64_or(
    const dcc_ctx_t *ctx,
    const char *key,
    uint64_t fallback
) {
    dcc_store_t *store = dcc_sugar_ctx_store(ctx);
    return store != NULL ? dcc_sugar_store_u64_or(store, key, fallback) : fallback;
}

#define DCC_CTX_STORE(ctx_) dcc_sugar_ctx_store((ctx_))
#define DCC_APP_HAS_STORE(app_) dcc_sugar_app_has_store((app_))
#define DCC_CTX_HAS_STORE(ctx_) dcc_sugar_ctx_has_store((ctx_))
#define DCC_APP_STORE_GET(app_, key_, out_value_, out_value_size_, out_value_len_) \
    dcc_sugar_app_store_get((app_), (key_), (out_value_), (out_value_size_), (out_value_len_))
#define DCC_CTX_STORE_GET(ctx_, key_, out_value_, out_value_size_, out_value_len_) \
    dcc_sugar_ctx_store_get((ctx_), (key_), (out_value_), (out_value_size_), (out_value_len_))
#define DCC_APP_STORE_SET(app_, key_, value_) \
    dcc_sugar_app_store_set((app_), (key_), (value_))
#define DCC_CTX_STORE_SET(ctx_, key_, value_) \
    dcc_sugar_ctx_store_set((ctx_), (key_), (value_))
#define DCC_APP_STORE_DELETE(app_, key_) dcc_sugar_app_store_delete((app_), (key_))
#define DCC_CTX_STORE_DELETE(ctx_, key_) dcc_sugar_ctx_store_delete((ctx_), (key_))
#define DCC_APP_STORE_GET_U64(app_, key_, out_) \
    dcc_sugar_app_store_get_u64((app_), (key_), (out_))
#define DCC_CTX_STORE_GET_U64(ctx_, key_, out_) \
    dcc_sugar_ctx_store_get_u64((ctx_), (key_), (out_))
#define DCC_APP_STORE_SET_U64(app_, key_, value_) \
    dcc_sugar_app_store_set_u64((app_), (key_), (value_))
#define DCC_CTX_STORE_SET_U64(ctx_, key_, value_) \
    dcc_sugar_ctx_store_set_u64((ctx_), (key_), (value_))
#define DCC_STORE_U64_OR(store_, key_, fallback_) \
    dcc_sugar_store_u64_or((store_), (key_), (fallback_))
#define DCC_APP_STORE_U64_OR(app_, key_, fallback_) \
    dcc_sugar_app_store_u64_or((app_), (key_), (fallback_))
#define DCC_CTX_STORE_U64_OR(ctx_, key_, fallback_) \
    dcc_sugar_ctx_store_u64_or((ctx_), (key_), (fallback_))
#define DCC_CTX_REQUIRE_STORE(ctx_, message_) \
    do { \
        if (!DCC_CTX_HAS_STORE((ctx_))) { \
            (void)DCC_CTX_WARN((ctx_), (message_)); \
            return; \
        } \
    } while (0)
#define DCC_REQUIRE_STORE(ctx_, message_) DCC_CTX_REQUIRE_STORE((ctx_), (message_))

#endif
