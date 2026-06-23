#ifndef DCC_SUGAR_FORMAT_H
#define DCC_SUGAR_FORMAT_H

#include <dcc/app.h>
#include <dcc/error.h>
#include <dcc/sugar/flags.h>
#include <dcc/snowflake.h>

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DCC_FORMAT_MENTION_MAX 25U
#define DCC_FORMAT_EMOJI_MAX 64U
#define DCC_FORMAT_COMMAND_MENTION_MAX 128U
#define DCC_FORMAT_TIMESTAMP_MAX 32U

typedef struct dcc_sugar_custom_emoji {
    const char *name;
    size_t name_len;
    dcc_snowflake_t id;
    uint8_t animated;
} dcc_sugar_custom_emoji_t;

static inline dcc_status_t dcc_sugar_format_done(char *buffer, size_t buffer_size, int written) {
    if (buffer == NULL || buffer_size == 0U || written < 0 || (size_t)written >= buffer_size) {
        if (buffer != NULL && buffer_size != 0U) {
            buffer[0] = '\0';
        }
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static inline dcc_status_t dcc_sugar_format_alloc_v(
    char **out_text,
    const char *format,
    va_list args
) {
    if (out_text == NULL || format == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_text = NULL;

    va_list count_args;
    va_copy(count_args, args);
    int needed = vsnprintf(NULL, 0U, format, count_args);
    va_end(count_args);
    if (needed < 0) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t size = (size_t)needed + 1U;
    if (size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    char *text = (char *)malloc(size);
    if (text == NULL) {
        return DCC_ERR_NOMEM;
    }

    va_list write_args;
    va_copy(write_args, args);
    int written = vsnprintf(text, size, format, write_args);
    va_end(write_args);
    if (written < 0 || written != needed) {
        free(text);
        return DCC_ERR_INVALID_ARG;
    }

    *out_text = text;
    return DCC_OK;
}

static inline dcc_status_t dcc_sugar_format_alloc(
    char **out_text,
    const char *format,
    ...
) {
    va_list args;
    va_start(args, format);
    dcc_status_t status = dcc_sugar_format_alloc_v(out_text, format, args);
    va_end(args);
    return status;
}

static inline void dcc_sugar_format_free(char *text) {
    free(text);
}

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

static inline uint8_t dcc_sugar_format_timestamp_style_valid(char style) {
    return style == '\0' ||
        style == 't' ||
        style == 'T' ||
        style == 'd' ||
        style == 'D' ||
        style == 'f' ||
        style == 'F' ||
        style == 'R'
            ? 1U
            : 0U;
}

static inline uint8_t dcc_sugar_format_emoji_name_valid_len(
    const char *name,
    size_t name_len
) {
    if (name == NULL || name_len < 2U || name_len > 32U) {
        return 0U;
    }

    for (size_t i = 0U; i < name_len; ++i) {
        unsigned char ch = (unsigned char)name[i];
        if (!((ch >= 'A' && ch <= 'Z') ||
              (ch >= 'a' && ch <= 'z') ||
              (ch >= '0' && ch <= '9') ||
              ch == '_')) {
            return 0U;
        }
    }
    return 1U;
}

static inline uint8_t dcc_sugar_format_emoji_name_valid(const char *name) {
    return name != NULL ? dcc_sugar_format_emoji_name_valid_len(name, strlen(name)) : 0U;
}

static inline uint8_t dcc_sugar_format_command_name_valid(const char *name) {
    if (name == NULL || name[0] == '\0') {
        return 0U;
    }

    size_t len = 0U;
    for (; name[len] != '\0'; ++len) {
        unsigned char ch = (unsigned char)name[len];
        if (len >= 32U || ch <= 0x20U || ch == ':' || ch == '<' || ch == '>') {
            return 0U;
        }
    }
    return len <= 32U ? 1U : 0U;
}

static inline dcc_status_t dcc_sugar_format_user_mention(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t user_id
) {
    if (buffer == NULL || buffer_size == 0U || user_id == 0U) {
        return dcc_sugar_format_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_format_done(
        buffer,
        buffer_size,
        snprintf(buffer, buffer_size, "<@%" PRIu64 ">", (uint64_t)user_id)
    );
}

static inline dcc_status_t dcc_sugar_format_member_mention(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t user_id
) {
    if (buffer == NULL || buffer_size == 0U || user_id == 0U) {
        return dcc_sugar_format_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_format_done(
        buffer,
        buffer_size,
        snprintf(buffer, buffer_size, "<@!%" PRIu64 ">", (uint64_t)user_id)
    );
}

static inline dcc_status_t dcc_sugar_format_role_mention(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t role_id
) {
    if (buffer == NULL || buffer_size == 0U || role_id == 0U) {
        return dcc_sugar_format_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_format_done(
        buffer,
        buffer_size,
        snprintf(buffer, buffer_size, "<@&%" PRIu64 ">", (uint64_t)role_id)
    );
}

static inline dcc_status_t dcc_sugar_format_channel_mention(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t channel_id
) {
    if (buffer == NULL || buffer_size == 0U || channel_id == 0U) {
        return dcc_sugar_format_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_format_done(
        buffer,
        buffer_size,
        snprintf(buffer, buffer_size, "<#%" PRIu64 ">", (uint64_t)channel_id)
    );
}

static inline dcc_status_t dcc_sugar_format_command_mention(
    char *buffer,
    size_t buffer_size,
    const char *name,
    dcc_snowflake_t command_id
) {
    if (buffer == NULL ||
        buffer_size == 0U ||
        !dcc_sugar_format_command_name_valid(name) ||
        command_id == 0U) {
        return dcc_sugar_format_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_format_done(
        buffer,
        buffer_size,
        snprintf(buffer, buffer_size, "</%s:%" PRIu64 ">", name, (uint64_t)command_id)
    );
}

static inline dcc_status_t dcc_sugar_format_subcommand_mention(
    char *buffer,
    size_t buffer_size,
    const char *name,
    const char *subcommand,
    dcc_snowflake_t command_id
) {
    if (buffer == NULL ||
        buffer_size == 0U ||
        !dcc_sugar_format_command_name_valid(name) ||
        !dcc_sugar_format_command_name_valid(subcommand) ||
        command_id == 0U) {
        return dcc_sugar_format_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_format_done(
        buffer,
        buffer_size,
        snprintf(buffer, buffer_size, "</%s %s:%" PRIu64 ">", name, subcommand, (uint64_t)command_id)
    );
}

static inline dcc_status_t dcc_sugar_format_subcommand_group_mention(
    char *buffer,
    size_t buffer_size,
    const char *name,
    const char *group,
    const char *subcommand,
    dcc_snowflake_t command_id
) {
    if (buffer == NULL ||
        buffer_size == 0U ||
        !dcc_sugar_format_command_name_valid(name) ||
        !dcc_sugar_format_command_name_valid(group) ||
        !dcc_sugar_format_command_name_valid(subcommand) ||
        command_id == 0U) {
        return dcc_sugar_format_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_format_done(
        buffer,
        buffer_size,
        snprintf(
            buffer,
            buffer_size,
            "</%s %s %s:%" PRIu64 ">",
            name,
            group,
            subcommand,
            (uint64_t)command_id
        )
    );
}

static inline dcc_status_t dcc_sugar_format_custom_emoji(
    char *buffer,
    size_t buffer_size,
    const char *name,
    dcc_snowflake_t emoji_id,
    uint8_t animated
) {
    if (buffer == NULL ||
        buffer_size == 0U ||
        !dcc_sugar_format_emoji_name_valid(name) ||
        emoji_id == 0U) {
        return dcc_sugar_format_done(buffer, buffer_size, -1);
    }
    return dcc_sugar_format_done(
        buffer,
        buffer_size,
        snprintf(
            buffer,
            buffer_size,
            animated ? "<a:%s:%" PRIu64 ">" : "<:%s:%" PRIu64 ">",
            name,
            (uint64_t)emoji_id
        )
    );
}

static inline dcc_status_t dcc_sugar_format_timestamp(
    char *buffer,
    size_t buffer_size,
    int64_t unix_seconds,
    char style
) {
    if (buffer == NULL || buffer_size == 0U || !dcc_sugar_format_timestamp_style_valid(style)) {
        return dcc_sugar_format_done(buffer, buffer_size, -1);
    }
    if (style == '\0') {
        return dcc_sugar_format_done(
            buffer,
            buffer_size,
            snprintf(buffer, buffer_size, "<t:%" PRId64 ">", unix_seconds)
        );
    }
    return dcc_sugar_format_done(
        buffer,
        buffer_size,
        snprintf(buffer, buffer_size, "<t:%" PRId64 ":%c>", unix_seconds, style)
    );
}

static inline const char *dcc_sugar_user_mention(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t user_id
) {
    return dcc_sugar_format_user_mention(buffer, buffer_size, user_id) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_member_mention(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t user_id
) {
    return dcc_sugar_format_member_mention(buffer, buffer_size, user_id) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_role_mention(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t role_id
) {
    return dcc_sugar_format_role_mention(buffer, buffer_size, role_id) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_channel_mention(
    char *buffer,
    size_t buffer_size,
    dcc_snowflake_t channel_id
) {
    return dcc_sugar_format_channel_mention(buffer, buffer_size, channel_id) == DCC_OK ? buffer : "";
}

static inline const char *dcc_sugar_command_mention(
    char *buffer,
    size_t buffer_size,
    const char *name,
    dcc_snowflake_t command_id
) {
    return dcc_sugar_format_command_mention(buffer, buffer_size, name, command_id) == DCC_OK
        ? buffer
        : "";
}

static inline const char *dcc_sugar_subcommand_mention(
    char *buffer,
    size_t buffer_size,
    const char *name,
    const char *subcommand,
    dcc_snowflake_t command_id
) {
    return dcc_sugar_format_subcommand_mention(
        buffer,
        buffer_size,
        name,
        subcommand,
        command_id
    ) == DCC_OK
        ? buffer
        : "";
}

static inline const char *dcc_sugar_subcommand_group_mention(
    char *buffer,
    size_t buffer_size,
    const char *name,
    const char *group,
    const char *subcommand,
    dcc_snowflake_t command_id
) {
    return dcc_sugar_format_subcommand_group_mention(
        buffer,
        buffer_size,
        name,
        group,
        subcommand,
        command_id
    ) == DCC_OK
        ? buffer
        : "";
}

static inline const char *dcc_sugar_custom_emoji(
    char *buffer,
    size_t buffer_size,
    const char *name,
    dcc_snowflake_t emoji_id,
    uint8_t animated
) {
    return dcc_sugar_format_custom_emoji(buffer, buffer_size, name, emoji_id, animated) == DCC_OK
        ? buffer
        : "";
}

static inline const char *dcc_sugar_timestamp(
    char *buffer,
    size_t buffer_size,
    int64_t unix_seconds,
    char style
) {
    return dcc_sugar_format_timestamp(buffer, buffer_size, unix_seconds, style) == DCC_OK ? buffer : "";
}

static inline dcc_status_t dcc_sugar_parse_custom_emoji_len(
    const char *text,
    size_t text_len,
    dcc_sugar_custom_emoji_t *out
) {
    if (text == NULL || out == NULL || text_len < 7U || text[0] != '<' || text[text_len - 1U] != '>') {
        return DCC_ERR_INVALID_ARG;
    }

    size_t name_start = 2U;
    uint8_t animated = 0U;
    if (text[1] == 'a') {
        if (text_len < 8U || text[2] != ':') {
            return DCC_ERR_INVALID_ARG;
        }
        animated = 1U;
        name_start = 3U;
    } else if (text[1] != ':') {
        return DCC_ERR_INVALID_ARG;
    }

    size_t name_end = name_start;
    while (name_end + 1U < text_len && text[name_end] != ':') {
        ++name_end;
    }
    if (name_end + 1U >= text_len || text[name_end] != ':') {
        return DCC_ERR_INVALID_ARG;
    }

    size_t name_len = name_end - name_start;
    size_t id_start = name_end + 1U;
    size_t id_len = text_len - id_start - 1U;
    dcc_snowflake_t id = 0U;
    if (!dcc_sugar_format_emoji_name_valid_len(text + name_start, name_len) ||
        dcc_snowflake_parse_len(text + id_start, id_len, &id) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }

    out->name = text + name_start;
    out->name_len = name_len;
    out->id = id;
    out->animated = animated;
    return DCC_OK;
}

static inline dcc_status_t dcc_sugar_parse_custom_emoji(
    const char *text,
    dcc_sugar_custom_emoji_t *out
) {
    return text != NULL
        ? dcc_sugar_parse_custom_emoji_len(text, strlen(text), out)
        : DCC_ERR_INVALID_ARG;
}

#define DCC_MENTION_USER(user_id_) \
    dcc_sugar_user_mention((char[DCC_FORMAT_MENTION_MAX]){ 0 }, DCC_FORMAT_MENTION_MAX, (user_id_))

#define DCC_MENTION_MEMBER(user_id_) \
    dcc_sugar_member_mention((char[DCC_FORMAT_MENTION_MAX]){ 0 }, DCC_FORMAT_MENTION_MAX, (user_id_))

#define DCC_MENTION_ROLE(role_id_) \
    dcc_sugar_role_mention((char[DCC_FORMAT_MENTION_MAX]){ 0 }, DCC_FORMAT_MENTION_MAX, (role_id_))

#define DCC_MENTION_CHANNEL(channel_id_) \
    dcc_sugar_channel_mention((char[DCC_FORMAT_MENTION_MAX]){ 0 }, DCC_FORMAT_MENTION_MAX, (channel_id_))

#define DCC_MENTION_COMMAND(name_, command_id_) \
    dcc_sugar_command_mention( \
        (char[DCC_FORMAT_COMMAND_MENTION_MAX]){ 0 }, \
        DCC_FORMAT_COMMAND_MENTION_MAX, \
        (name_), \
        (command_id_) \
    )

#define DCC_MENTION_SUBCOMMAND(name_, subcommand_, command_id_) \
    dcc_sugar_subcommand_mention( \
        (char[DCC_FORMAT_COMMAND_MENTION_MAX]){ 0 }, \
        DCC_FORMAT_COMMAND_MENTION_MAX, \
        (name_), \
        (subcommand_), \
        (command_id_) \
    )

#define DCC_MENTION_SUBCOMMAND_GROUP(name_, group_, subcommand_, command_id_) \
    dcc_sugar_subcommand_group_mention( \
        (char[DCC_FORMAT_COMMAND_MENTION_MAX]){ 0 }, \
        DCC_FORMAT_COMMAND_MENTION_MAX, \
        (name_), \
        (group_), \
        (subcommand_), \
        (command_id_) \
    )

#define DCC_CUSTOM_EMOJI(name_, emoji_id_, animated_) \
    dcc_sugar_custom_emoji( \
        (char[DCC_FORMAT_EMOJI_MAX]){ 0 }, \
        DCC_FORMAT_EMOJI_MAX, \
        (name_), \
        (emoji_id_), \
        (animated_) \
    )

#define DCC_EMOJI(name_, emoji_id_) DCC_CUSTOM_EMOJI((name_), (emoji_id_), 0U)
#define DCC_ANIMATED_EMOJI(name_, emoji_id_) DCC_CUSTOM_EMOJI((name_), (emoji_id_), 1U)

#define DCC_TIMESTAMP(unix_seconds_, style_) \
    dcc_sugar_timestamp( \
        (char[DCC_FORMAT_TIMESTAMP_MAX]){ 0 }, \
        DCC_FORMAT_TIMESTAMP_MAX, \
        (unix_seconds_), \
        (style_) \
    )

#define DCC_TIMESTAMP_SHORT_TIME(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 't')
#define DCC_TIMESTAMP_LONG_TIME(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'T')
#define DCC_TIMESTAMP_SHORT_DATE(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'd')
#define DCC_TIMESTAMP_LONG_DATE(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'D')
#define DCC_TIMESTAMP_SHORT(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'f')
#define DCC_TIMESTAMP_LONG(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'F')
#define DCC_TIMESTAMP_RELATIVE(unix_seconds_) DCC_TIMESTAMP((unix_seconds_), 'R')

#define DCC_FORMAT_ALLOC(out_text_, ...) dcc_sugar_format_alloc((out_text_), __VA_ARGS__)
#define DCC_FORMAT_FREE(text_) dcc_sugar_format_free((text_))

#define DCC_REPLYF(ctx_, ...) dcc_sugar_ctx_replyf((ctx_), __VA_ARGS__)
#define DCC_REPLY_PRIVATEF(ctx_, ...) dcc_sugar_ctx_reply_privatef((ctx_), __VA_ARGS__)
#define DCC_REPLY_SILENTF(ctx_, ...) dcc_sugar_ctx_reply_silentf((ctx_), __VA_ARGS__)
#define DCC_REPLY_NO_EMBEDSF(ctx_, ...) dcc_sugar_ctx_reply_no_embedsf((ctx_), __VA_ARGS__)
#define DCC_PRIVATEF(ctx_, ...) DCC_REPLY_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_RESPONDF(ctx_, ...) DCC_REPLYF((ctx_), __VA_ARGS__)
#define DCC_RESPOND_PRIVATEF(ctx_, ...) DCC_REPLY_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_RESPOND_SILENTF(ctx_, ...) DCC_REPLY_SILENTF((ctx_), __VA_ARGS__)
#define DCC_RESPOND_NO_EMBEDSF(ctx_, ...) DCC_REPLY_NO_EMBEDSF((ctx_), __VA_ARGS__)

#define DCC_FOLLOWUPF(ctx_, ...) dcc_sugar_ctx_followupf((ctx_), __VA_ARGS__)
#define DCC_FOLLOWUP_PRIVATEF(ctx_, ...) dcc_sugar_ctx_followup_privatef((ctx_), __VA_ARGS__)
#define DCC_FOLLOWUP_SILENTF(ctx_, ...) dcc_sugar_ctx_followup_silentf((ctx_), __VA_ARGS__)
#define DCC_SENDF(ctx_, ...) dcc_sugar_ctx_sendf((ctx_), __VA_ARGS__)
#define DCC_UPDATEF(ctx_, ...) dcc_sugar_ctx_updatef((ctx_), __VA_ARGS__)
#define DCC_UPDATE_MESSAGEF(ctx_, ...) DCC_UPDATEF((ctx_), __VA_ARGS__)
#define DCC_EDITF(ctx_, ...) dcc_sugar_ctx_editf((ctx_), __VA_ARGS__)
#define DCC_EDIT_ORIGINALF(ctx_, ...) DCC_EDITF((ctx_), __VA_ARGS__)
#define DCC_DONEF(ctx_, ...) dcc_sugar_ctx_okf((ctx_), __VA_ARGS__)
#define DCC_ACKF(ctx_, ...) DCC_DONEF((ctx_), __VA_ARGS__)
#define DCC_SUCCESSF(ctx_, title_, ...) dcc_sugar_ctx_successf((ctx_), (title_), __VA_ARGS__)
#define DCC_FAILF(ctx_, ...) dcc_sugar_ctx_failf((ctx_), __VA_ARGS__)
#define DCC_ERRORF(ctx_, title_, ...) dcc_sugar_ctx_errorf((ctx_), (title_), __VA_ARGS__)
#define DCC_DENYF(ctx_, ...) DCC_ERRORF((ctx_), "Permission denied", __VA_ARGS__)
#define DCC_BAD_INPUTF(ctx_, ...) DCC_ERRORF((ctx_), "Invalid input", __VA_ARGS__)
#define DCC_NOT_FOUNDF(ctx_, ...) DCC_ERRORF((ctx_), "Not found", __VA_ARGS__)
#define DCC_BUSYF(ctx_, ...) DCC_ERRORF((ctx_), "Please wait", __VA_ARGS__)

#define DCC_CTX_REPLYF(ctx_, ...) DCC_REPLYF((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_F(ctx_, ...) DCC_REPLYF((ctx_), __VA_ARGS__)
#define DCC_CTX_REPLY_TEXT_F(ctx_, ...) DCC_REPLYF((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPONDF(ctx_, ...) DCC_RESPONDF((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_F(ctx_, ...) DCC_RESPONDF((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_TEXT_F(ctx_, ...) DCC_RESPONDF((ctx_), __VA_ARGS__)
#define DCC_CTX_PRIVATEF(ctx_, ...) DCC_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_PRIVATE_F(ctx_, ...) DCC_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_EPHEMERAL_F(ctx_, ...) DCC_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_PRIVATEF(ctx_, ...) DCC_RESPOND_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_PRIVATE_F(ctx_, ...) DCC_RESPOND_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_EPHEMERAL_F(ctx_, ...) DCC_RESPOND_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_SILENTF(ctx_, ...) DCC_REPLY_SILENTF((ctx_), __VA_ARGS__)
#define DCC_CTX_SILENT_F(ctx_, ...) DCC_REPLY_SILENTF((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_SILENTF(ctx_, ...) DCC_RESPOND_SILENTF((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_SILENT_F(ctx_, ...) DCC_RESPOND_SILENTF((ctx_), __VA_ARGS__)
#define DCC_CTX_NO_EMBEDSF(ctx_, ...) DCC_REPLY_NO_EMBEDSF((ctx_), __VA_ARGS__)
#define DCC_CTX_NO_EMBEDS_F(ctx_, ...) DCC_REPLY_NO_EMBEDSF((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_NO_EMBEDSF(ctx_, ...) DCC_RESPOND_NO_EMBEDSF((ctx_), __VA_ARGS__)
#define DCC_CTX_RESPOND_NO_EMBEDS_F(ctx_, ...) DCC_RESPOND_NO_EMBEDSF((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUPF(ctx_, ...) DCC_FOLLOWUPF((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_F(ctx_, ...) DCC_FOLLOWUPF((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_TEXT_F(ctx_, ...) DCC_FOLLOWUPF((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_PRIVATEF(ctx_, ...) DCC_FOLLOWUP_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_PRIVATE_F(ctx_, ...) DCC_FOLLOWUP_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_EPHEMERAL_F(ctx_, ...) DCC_FOLLOWUP_PRIVATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_SILENTF(ctx_, ...) DCC_FOLLOWUP_SILENTF((ctx_), __VA_ARGS__)
#define DCC_CTX_FOLLOWUP_SILENT_F(ctx_, ...) DCC_FOLLOWUP_SILENTF((ctx_), __VA_ARGS__)
#define DCC_CTX_SENDF(ctx_, ...) DCC_SENDF((ctx_), __VA_ARGS__)
#define DCC_CTX_SEND_F(ctx_, ...) DCC_SENDF((ctx_), __VA_ARGS__)
#define DCC_CTX_SEND_TEXT_F(ctx_, ...) DCC_SENDF((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATEF(ctx_, ...) DCC_UPDATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATE_F(ctx_, ...) DCC_UPDATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATE_TEXT_F(ctx_, ...) DCC_UPDATEF((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATE_MESSAGEF(ctx_, ...) DCC_UPDATE_MESSAGEF((ctx_), __VA_ARGS__)
#define DCC_CTX_UPDATE_MESSAGE_F(ctx_, ...) DCC_UPDATE_MESSAGEF((ctx_), __VA_ARGS__)
#define DCC_CTX_EDITF(ctx_, ...) DCC_EDITF((ctx_), __VA_ARGS__)
#define DCC_CTX_EDIT_F(ctx_, ...) DCC_EDITF((ctx_), __VA_ARGS__)
#define DCC_CTX_EDIT_TEXT_F(ctx_, ...) DCC_EDITF((ctx_), __VA_ARGS__)
#define DCC_CTX_EDIT_ORIGINALF(ctx_, ...) DCC_EDIT_ORIGINALF((ctx_), __VA_ARGS__)
#define DCC_CTX_EDIT_ORIGINAL_F(ctx_, ...) DCC_EDIT_ORIGINALF((ctx_), __VA_ARGS__)
#define DCC_CTX_DONEF(ctx_, ...) DCC_DONEF((ctx_), __VA_ARGS__)
#define DCC_CTX_DONE_F(ctx_, ...) DCC_DONEF((ctx_), __VA_ARGS__)
#define DCC_CTX_ACKF(ctx_, ...) DCC_ACKF((ctx_), __VA_ARGS__)
#define DCC_CTX_ACK_F(ctx_, ...) DCC_ACKF((ctx_), __VA_ARGS__)
#define DCC_CTX_SUCCESSF(ctx_, title_, ...) DCC_SUCCESSF((ctx_), (title_), __VA_ARGS__)
#define DCC_CTX_SUCCESS_F(ctx_, title_, ...) DCC_SUCCESSF((ctx_), (title_), __VA_ARGS__)
#define DCC_CTX_FAILF(ctx_, ...) DCC_FAILF((ctx_), __VA_ARGS__)
#define DCC_CTX_FAIL_F(ctx_, ...) DCC_FAILF((ctx_), __VA_ARGS__)
#define DCC_CTX_ERRORF(ctx_, title_, ...) DCC_ERRORF((ctx_), (title_), __VA_ARGS__)
#define DCC_CTX_ERROR_F(ctx_, title_, ...) DCC_ERRORF((ctx_), (title_), __VA_ARGS__)
#define DCC_CTX_DENYF(ctx_, ...) DCC_DENYF((ctx_), __VA_ARGS__)
#define DCC_CTX_DENY_F(ctx_, ...) DCC_DENYF((ctx_), __VA_ARGS__)
#define DCC_CTX_BAD_INPUTF(ctx_, ...) DCC_BAD_INPUTF((ctx_), __VA_ARGS__)
#define DCC_CTX_BAD_INPUT_F(ctx_, ...) DCC_BAD_INPUTF((ctx_), __VA_ARGS__)
#define DCC_CTX_NOT_FOUNDF(ctx_, ...) DCC_NOT_FOUNDF((ctx_), __VA_ARGS__)
#define DCC_CTX_NOT_FOUND_F(ctx_, ...) DCC_NOT_FOUNDF((ctx_), __VA_ARGS__)
#define DCC_CTX_BUSYF(ctx_, ...) DCC_BUSYF((ctx_), __VA_ARGS__)
#define DCC_CTX_BUSY_F(ctx_, ...) DCC_BUSYF((ctx_), __VA_ARGS__)

#define DCC_APP_SENDF(app_, channel_id_, ...) dcc_sugar_app_sendf((app_), (channel_id_), __VA_ARGS__)
#define DCC_APP_SEND_F(app_, channel_id_, ...) DCC_APP_SENDF((app_), (channel_id_), __VA_ARGS__)
#define DCC_APP_SEND_TEXT_F(app_, channel_id_, ...) DCC_APP_SENDF((app_), (channel_id_), __VA_ARGS__)
#define DCC_CHANNEL_SENDF(app_, channel_id_, ...) DCC_APP_SENDF((app_), (channel_id_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_F(app_, channel_id_, ...) DCC_CHANNEL_SENDF((app_), (channel_id_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_TEXT_F(app_, channel_id_, ...) \
    DCC_CHANNEL_SENDF((app_), (channel_id_), __VA_ARGS__)
#define DCC_APP_EDITF(app_, channel_id_, message_id_, ...) \
    dcc_sugar_app_editf((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_APP_EDIT_F(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDITF((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_APP_EDIT_TEXT_F(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDITF((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_MESSAGE_EDITF(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDITF((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_MESSAGE_EDIT_F(app_, channel_id_, message_id_, ...) \
    DCC_MESSAGE_EDITF((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_MESSAGE_EDIT_TEXT_F(app_, channel_id_, message_id_, ...) \
    DCC_MESSAGE_EDITF((app_), (channel_id_), (message_id_), __VA_ARGS__)

#endif
