#include <dcc/snowflake.h>

#include <stdint.h>
#include <string.h>

dcc_status_t dcc_snowflake_parse_len(
    const char *text,
    size_t text_len,
    dcc_snowflake_t *out
) {
    if (text == NULL || text_len == 0U || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    uint64_t value = 0U;
    for (size_t i = 0U; i < text_len; ++i) {
        unsigned char ch = (unsigned char)text[i];
        if (ch < '0' || ch > '9') {
            return DCC_ERR_INVALID_ARG;
        }
        uint64_t digit = (uint64_t)(ch - '0');
        if (value > (UINT64_MAX - digit) / 10U) {
            return DCC_ERR_INVALID_ARG;
        }
        value = value * 10U + digit;
    }
    if (value == 0U) {
        return DCC_ERR_INVALID_ARG;
    }

    *out = (dcc_snowflake_t)value;
    return DCC_OK;
}

dcc_status_t dcc_snowflake_parse(const char *text, dcc_snowflake_t *out) {
    return text != NULL ? dcc_snowflake_parse_len(text, strlen(text), out) : DCC_ERR_INVALID_ARG;
}

static dcc_status_t dcc_snowflake_parse_wrapped(
    const char *text,
    const char *prefix,
    const char *suffix,
    dcc_snowflake_t *out
) {
    if (text == NULL || prefix == NULL || suffix == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t text_len = strlen(text);
    size_t prefix_len = strlen(prefix);
    size_t suffix_len = strlen(suffix);
    if (text_len <= prefix_len + suffix_len ||
        strncmp(text, prefix, prefix_len) != 0 ||
        strcmp(text + text_len - suffix_len, suffix) != 0) {
        return DCC_ERR_INVALID_ARG;
    }

    return dcc_snowflake_parse_len(
        text + prefix_len,
        text_len - prefix_len - suffix_len,
        out
    );
}

dcc_status_t dcc_snowflake_parse_user_mention(
    const char *text,
    dcc_snowflake_t *out
) {
    dcc_status_t status = dcc_snowflake_parse_wrapped(text, "<@!", ">", out);
    return status == DCC_OK ? status : dcc_snowflake_parse_wrapped(text, "<@", ">", out);
}

dcc_status_t dcc_snowflake_parse_member_mention(
    const char *text,
    dcc_snowflake_t *out
) {
    return dcc_snowflake_parse_wrapped(text, "<@!", ">", out);
}

dcc_status_t dcc_snowflake_parse_role_mention(
    const char *text,
    dcc_snowflake_t *out
) {
    return dcc_snowflake_parse_wrapped(text, "<@&", ">", out);
}

dcc_status_t dcc_snowflake_parse_channel_mention(
    const char *text,
    dcc_snowflake_t *out
) {
    return dcc_snowflake_parse_wrapped(text, "<#", ">", out);
}

dcc_status_t dcc_snowflake_parse_any_mention(
    const char *text,
    dcc_snowflake_t *out
) {
    dcc_status_t status = dcc_snowflake_parse_channel_mention(text, out);
    if (status == DCC_OK) {
        return status;
    }
    status = dcc_snowflake_parse_role_mention(text, out);
    if (status == DCC_OK) {
        return status;
    }
    status = dcc_snowflake_parse_member_mention(text, out);
    if (status == DCC_OK) {
        return status;
    }
    return dcc_snowflake_parse_user_mention(text, out);
}

dcc_status_t dcc_snowflake_parse_any(
    const char *text,
    dcc_snowflake_t *out
) {
    dcc_status_t status = dcc_snowflake_parse(text, out);
    return status == DCC_OK ? status : dcc_snowflake_parse_any_mention(text, out);
}

dcc_status_t dcc_snowflake_created_at_ms(
    dcc_snowflake_t snowflake,
    uint64_t *out_unix_ms
) {
    if (snowflake == 0U || out_unix_ms == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_unix_ms = ((uint64_t)snowflake >> 22U) + DCC_SNOWFLAKE_DISCORD_EPOCH_MS;
    return DCC_OK;
}

dcc_status_t dcc_snowflake_created_at_unix_seconds(
    dcc_snowflake_t snowflake,
    int64_t *out_unix_seconds
) {
    uint64_t unix_ms = 0U;
    dcc_status_t status = dcc_snowflake_created_at_ms(snowflake, &unix_ms);
    if (status != DCC_OK || out_unix_seconds == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_unix_seconds = (int64_t)(unix_ms / 1000U);
    return DCC_OK;
}
