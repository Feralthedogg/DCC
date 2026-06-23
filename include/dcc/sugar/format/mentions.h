#ifndef DCC_SUGAR_FORMAT_MENTIONS_H
#define DCC_SUGAR_FORMAT_MENTIONS_H

#include <dcc/sugar/format/validation.h>

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

#endif /* DCC_SUGAR_FORMAT_MENTIONS_H */
