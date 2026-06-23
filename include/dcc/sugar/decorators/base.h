#ifndef DCC_SUGAR_DECORATORS_BASE_H
#define DCC_SUGAR_DECORATORS_BASE_H

#include <dcc/sugar/app_params.h>
#include <dcc/sugar/events.h>

#define DCC_DECORATED_ROUTE_NAME_(name_) name_ ## _dcc_route
#define DCC_DECORATED_ROUTE_NAME(name_) DCC_DECORATED_ROUTE_NAME_(name_)
#define DCC_ROUTE(name_) DCC_DECORATED_ROUTE_NAME(name_)
#define DCC_TYPED_ROUTE(name_) DCC_ROUTE(name_)

#define DCC_DECORATE_TYPED_HANDLER_IMPL(route_type_, handler_, args_type_, arg_, route_init_) \
    static void handler_(dcc_ctx_t *ctx, void *args, void *user_data); \
    static void handler_ ## _typed(dcc_ctx_t *ctx, const args_type_ *arg_, void *user_data); \
    static const route_type_ DCC_DECORATED_ROUTE_NAME(handler_) = route_init_; \
    static void handler_(dcc_ctx_t *ctx, void *args, void *user_data) { \
        if (args == NULL) { \
            return; \
        } \
        handler_ ## _typed((ctx), (const args_type_ *)args, (user_data)); \
    } \
    static void handler_ ## _typed(dcc_ctx_t *ctx, const args_type_ *arg_, void *user_data)

#endif /* DCC_SUGAR_DECORATORS_BASE_H */
