#include "internal/app/dcc_app_internal.h"

#include <stdio.h>

dcc_status_t dcc_ctx_send(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || message == NULL || ctx->app == NULL || dcc_ctx_channel_id(ctx) == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_send(ctx->app, dcc_ctx_channel_id(ctx), message, cb, user_data);
}

dcc_status_t dcc_ctx_send_with_id(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_app_message_id_cb cb,
    void *user_data
) {
    if (ctx == NULL || message == NULL || ctx->app == NULL || dcc_ctx_channel_id(ctx) == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_send_with_id(ctx->app, dcc_ctx_channel_id(ctx), message, cb, user_data);
}

dcc_status_t dcc_ctx_send_with_thread(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    const dcc_thread_params_t *thread,
    dcc_app_message_thread_cb cb,
    void *user_data
) {
    if (ctx == NULL || message == NULL || thread == NULL || ctx->app == NULL ||
        dcc_ctx_channel_id(ctx) == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_app_send_with_thread(ctx->app, dcc_ctx_channel_id(ctx), message, thread, cb, user_data);
}

dcc_status_t dcc_ctx_send_with_thread_name(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    const char *thread_name,
    dcc_app_message_thread_cb cb,
    void *user_data
) {
    if (ctx == NULL || message == NULL || thread_name == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_ctx_send_with_thread(
        ctx,
        message,
        &((dcc_thread_params_t[]){ {
            .size = sizeof(dcc_thread_params_t),
            .name = thread_name
        } })[0],
        cb,
        user_data
    );
}

dcc_status_t dcc_ctx_send_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
) {
    if (ctx == NULL || content == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U,
    };
    return dcc_ctx_send(ctx, &message, cb, user_data);
}

dcc_status_t dcc_ctx_send_text_with_id(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_app_message_id_cb cb,
    void *user_data
) {
    if (ctx == NULL || content == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U,
    };
    return dcc_ctx_send_with_id(ctx, &message, cb, user_data);
}

dcc_status_t dcc_ctx_send_text_with_thread(
    dcc_ctx_t *ctx,
    const char *content,
    const char *thread_name,
    dcc_app_message_thread_cb cb,
    void *user_data
) {
    if (ctx == NULL || content == NULL || thread_name == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U,
    };
    return dcc_ctx_send_with_thread_name(ctx, &message, thread_name, cb, user_data);
}

dcc_status_t dcc_ctx_ok(
    dcc_ctx_t *ctx,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_ctx_reply_success(ctx, "Done", description, cb, user_data);
}

dcc_status_t dcc_ctx_fail(
    dcc_ctx_t *ctx,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_ctx_reply_error(ctx, "Error", description, cb, user_data);
}

dcc_status_t dcc_ctx_reply_validation_error(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    dcc_rest_cb cb,
    void *user_data
) {
    const char *description = "Check the command or form input and try again.";
    if (status == DCC_ERR_NOT_FOUND) {
        description = "A required input is missing. Fill it in and try again.";
    } else if (status == DCC_ERR_INVALID_ARG) {
        description = "One of the inputs is invalid. Check the values and try again.";
    }
    return dcc_ctx_reply_error(ctx, "Invalid input", description, cb, user_data);
}

dcc_status_t dcc_ctx_reply_validation_result(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const dcc_ctx_validation_error_t *error,
    dcc_rest_cb cb,
    void *user_data
) {
    if (error == NULL || error->status == DCC_OK ||
        error->field == NULL || error->field[0] == '\0') {
        return dcc_ctx_reply_validation_error(ctx, status, cb, user_data);
    }

    const char *message = error->message != NULL && error->message[0] != '\0'
        ? error->message
        : "Check this input and try again.";
    char description[256];
    (void)snprintf(
        description,
        sizeof(description),
        "`%s`: %s",
        error->field,
        message
    );
    return dcc_ctx_reply_error(ctx, "Invalid input", description, cb, user_data);
}
