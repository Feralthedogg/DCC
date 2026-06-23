#include <dcc/message_link.h>

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static dcc_status_t dcc_message_link_format_done(
    char *buffer,
    size_t buffer_size,
    int written
) {
    if (buffer == NULL || buffer_size == 0U || written < 0 || (size_t)written >= buffer_size) {
        if (buffer != NULL && buffer_size != 0U) {
            buffer[0] = '\0';
        }
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_message_link_format(
    char *buffer,
    size_t buffer_size,
    const dcc_message_link_t *link
) {
    if (buffer == NULL || buffer_size == 0U || link == NULL ||
        link->channel_id == 0U || link->message_id == 0U) {
        return dcc_message_link_format_done(buffer, buffer_size, -1);
    }

    if (link->is_dm != 0U) {
        if (link->guild_id != 0U) {
            return dcc_message_link_format_done(buffer, buffer_size, -1);
        }
        return dcc_message_link_format_done(
            buffer,
            buffer_size,
            snprintf(
                buffer,
                buffer_size,
                "https://discord.com/channels/@me/%" PRIu64 "/%" PRIu64,
                (uint64_t)link->channel_id,
                (uint64_t)link->message_id
            )
        );
    }

    if (link->guild_id == 0U) {
        return dcc_message_link_format_done(buffer, buffer_size, -1);
    }

    return dcc_message_link_format_done(
        buffer,
        buffer_size,
        snprintf(
            buffer,
            buffer_size,
            "https://discord.com/channels/%" PRIu64 "/%" PRIu64 "/%" PRIu64,
            (uint64_t)link->guild_id,
            (uint64_t)link->channel_id,
            (uint64_t)link->message_id
        )
    );
}

static const char *dcc_message_link_after_prefix(const char *text) {
    static const char *const prefixes[] = {
        "https://discord.com/channels/",
        "http://discord.com/channels/",
        "https://canary.discord.com/channels/",
        "http://canary.discord.com/channels/",
        "https://ptb.discord.com/channels/",
        "http://ptb.discord.com/channels/",
        "/channels/",
        "channels/"
    };

    for (size_t i = 0U; i < sizeof(prefixes) / sizeof(prefixes[0]); ++i) {
        size_t prefix_len = strlen(prefixes[i]);
        if (strncmp(text, prefixes[i], prefix_len) == 0) {
            return text + prefix_len;
        }
    }
    return NULL;
}

static dcc_status_t dcc_message_link_parse_segments(
    const char *segments,
    dcc_message_link_t *out
) {
    const char *first_slash;
    const char *second_slash;
    const char *third_slash;
    dcc_message_link_t parsed;

    first_slash = strchr(segments, '/');
    if (first_slash == NULL || first_slash == segments) {
        return DCC_ERR_INVALID_ARG;
    }
    second_slash = strchr(first_slash + 1, '/');
    if (second_slash == NULL || second_slash == first_slash + 1) {
        return DCC_ERR_INVALID_ARG;
    }
    third_slash = strchr(second_slash + 1, '/');
    if (third_slash != NULL || second_slash[1] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    memset(&parsed, 0, sizeof(parsed));
    if ((size_t)(first_slash - segments) == 3U && strncmp(segments, "@me", 3U) == 0) {
        parsed.is_dm = 1U;
    } else if (dcc_snowflake_parse_len(
        segments,
        (size_t)(first_slash - segments),
        &parsed.guild_id
    ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }

    if (dcc_snowflake_parse_len(
        first_slash + 1,
        (size_t)(second_slash - first_slash - 1),
        &parsed.channel_id
    ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_snowflake_parse(second_slash + 1, &parsed.message_id) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }

    *out = parsed;
    return DCC_OK;
}

dcc_status_t dcc_message_link_parse(
    const char *text,
    dcc_message_link_t *out
) {
    const char *segments;

    if (text == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    segments = dcc_message_link_after_prefix(text);
    if (segments == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_message_link_parse_segments(segments, out);
}
