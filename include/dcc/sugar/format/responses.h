#ifndef DCC_SUGAR_FORMAT_RESPONSES_H
#define DCC_SUGAR_FORMAT_RESPONSES_H

#include <dcc/sugar/format/responses_v.h>

static inline dcc_status_t dcc_sugar_ctx_replyf(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_replyfv(ctx, 0U, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_reply_privatef(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_replyfv(ctx, DCC_MESSAGE_FLAG_EPHEMERAL, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_reply_silentf(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status =
        dcc_sugar_ctx_replyfv(ctx, DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_reply_no_embedsf(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status =
        dcc_sugar_ctx_replyfv(ctx, DCC_MESSAGE_FLAG_SUPPRESS_EMBEDS, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_followupf(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_followupfv(ctx, 0U, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_followup_privatef(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_followupfv(ctx, DCC_MESSAGE_FLAG_EPHEMERAL, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_followup_silentf(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status =
        dcc_sugar_ctx_followupfv(ctx, DCC_MESSAGE_FLAG_SUPPRESS_NOTIFICATIONS, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_sendf(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_sendfv(ctx, 0U, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_updatef(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_updatefv(ctx, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_editf(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_editfv(ctx, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_okf(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_okfv(ctx, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_failf(
    dcc_ctx_t *ctx,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_failfv(ctx, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_successf(
    dcc_ctx_t *ctx,
    const char *title,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_successfv(ctx, title, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_ctx_errorf(
    dcc_ctx_t *ctx,
    const char *title,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_ctx_errorfv(ctx, title, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_app_sendf(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_app_sendfv(app, channel_id, 0U, format, args);
    va_end(args);
    return status;
}

static inline dcc_status_t dcc_sugar_app_editf(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_app_editfv(app, channel_id, message_id, format, args);
    va_end(args);
    return status;
}

#endif /* DCC_SUGAR_FORMAT_RESPONSES_H */
