#ifndef DCC_SUGAR_FORMAT_RESPONSES_V_H
#define DCC_SUGAR_FORMAT_RESPONSES_V_H

#include <dcc/sugar/format/base.h>

static inline dcc_status_t dcc_sugar_ctx_replyfv(
    dcc_ctx_t *ctx,
    uint64_t flags,
    const char *format,
    va_list args
) {
    if (ctx == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    dcc_message_builder_t message = {
        .content = content,
        .flags = flags,
        .has_content = 1U,
        .has_flags = flags != 0U ? 1U : 0U
    };
    status = dcc_ctx_reply(ctx, &message, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_followupfv(
    dcc_ctx_t *ctx,
    uint64_t flags,
    const char *format,
    va_list args
) {
    if (ctx == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    dcc_message_builder_t message = {
        .content = content,
        .flags = flags,
        .has_content = 1U,
        .has_flags = flags != 0U ? 1U : 0U
    };
    status = dcc_ctx_followup(ctx, &message, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_sendfv(
    dcc_ctx_t *ctx,
    uint64_t flags,
    const char *format,
    va_list args
) {
    if (ctx == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    dcc_message_builder_t message = {
        .content = content,
        .flags = flags,
        .has_content = 1U,
        .has_flags = flags != 0U ? 1U : 0U
    };
    status = dcc_ctx_send(ctx, &message, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_updatefv(
    dcc_ctx_t *ctx,
    const char *format,
    va_list args
) {
    if (ctx == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U
    };
    status = dcc_ctx_update_message(ctx, &message, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_editfv(
    dcc_ctx_t *ctx,
    const char *format,
    va_list args
) {
    if (ctx == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U
    };
    status = dcc_ctx_edit_original(ctx, &message, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_okfv(
    dcc_ctx_t *ctx,
    const char *format,
    va_list args
) {
    if (ctx == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_ctx_ok(ctx, content, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_failfv(
    dcc_ctx_t *ctx,
    const char *format,
    va_list args
) {
    if (ctx == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_ctx_fail(ctx, content, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_successfv(
    dcc_ctx_t *ctx,
    const char *title,
    const char *format,
    va_list args
) {
    if (ctx == NULL || title == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_ctx_reply_success(ctx, title, content, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_errorfv(
    dcc_ctx_t *ctx,
    const char *title,
    const char *format,
    va_list args
) {
    if (ctx == NULL || title == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_ctx_reply_error(ctx, title, content, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

static inline dcc_status_t dcc_sugar_app_sendfv(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    uint64_t flags,
    const char *format,
    va_list args
) {
    if (app == NULL || channel_id == 0U || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    dcc_message_builder_t message = {
        .content = content,
        .flags = flags,
        .has_content = 1U,
        .has_flags = flags != 0U ? 1U : 0U
    };
    status = dcc_app_send(app, channel_id, &message, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

static inline dcc_status_t dcc_sugar_app_editfv(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *format,
    va_list args
) {
    if (app == NULL || channel_id == 0U || message_id == 0U || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *content = NULL;
    dcc_status_t status = dcc_sugar_format_alloc_v(&content, format, args);
    if (status != DCC_OK) {
        return status;
    }

    dcc_message_builder_t message = {
        .content = content,
        .has_content = 1U
    };
    status = dcc_app_edit_message(app, channel_id, message_id, &message, NULL, NULL);
    dcc_sugar_format_free(content);
    return status;
}

#endif /* DCC_SUGAR_FORMAT_RESPONSES_V_H */
