#include "internal/app/dcc_app_internal.h"

static const char *dcc_app_default_error_description(dcc_status_t status, const char *message) {
    switch (status) {
        case DCC_ERR_NOT_FOUND:
            return "Required input was not found. Check the command or form fields and try again.";
        case DCC_ERR_INVALID_ARG:
            return "The request was invalid. Check the input and try again.";
        case DCC_ERR_STATE:
            return "This action cannot be used right now.";
        default:
            break;
    }
    return message != NULL && message[0] != '\0' ? message : "The action failed.";
}

static void dcc_app_default_error_handler(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const char *message,
    void *user_data
) {
    (void)user_data;
    (void)dcc_ctx_reply_error(
        ctx,
        "Action failed",
        dcc_app_default_error_description(status, message),
        NULL,
        NULL
    );
}

dcc_status_t dcc_app_on_error(
    dcc_app_t *app,
    dcc_app_error_fn handler,
    void *user_data
) {
    if (app == NULL || handler == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    app->error_handler = handler;
    app->error_user_data = user_data;
    return DCC_OK;
}

dcc_status_t dcc_app_use_default_error_responses(dcc_app_t *app) {
    return dcc_app_on_error(app, dcc_app_default_error_handler, NULL);
}

dcc_status_t dcc_ctx_handle_error(dcc_ctx_t *ctx, dcc_status_t status, const char *message) {
    if (status == DCC_OK) {
        return DCC_OK;
    }
    if (ctx == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (ctx->app != NULL && ctx->app->error_handler != NULL) {
        ctx->app->error_handler(ctx, status, message, ctx->app->error_user_data);
    } else {
        dcc_app_default_error_handler(ctx, status, message, NULL);
    }
    return status;
}
