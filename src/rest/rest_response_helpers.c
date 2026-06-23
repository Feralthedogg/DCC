#include <dcc/rest/response_helpers.h>

#include <stdint.h>
#include <string.h>

static const char *dcc_rest_response_skip_ws(const char *p, const char *end) {
    while (p < end && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t')) {
        p++;
    }
    return p;
}

static const char *dcc_rest_response_scan_string(
    const char *p,
    const char *end,
    const char **out_start,
    size_t *out_len,
    int *out_escaped
) {
    if (p >= end || *p != '"') {
        return NULL;
    }

    p++;
    const char *start = p;
    int escaped = 0;
    while (p < end) {
        if (*p == '\\') {
            escaped = 1;
            p++;
            if (p >= end) {
                return NULL;
            }
            p++;
            continue;
        }
        if (*p == '"') {
            if (out_start != NULL) {
                *out_start = start;
            }
            if (out_len != NULL) {
                *out_len = (size_t)(p - start);
            }
            if (out_escaped != NULL) {
                *out_escaped = escaped;
            }
            return p + 1;
        }
        p++;
    }
    return NULL;
}

static int dcc_rest_response_field_matches(
    const char *field,
    size_t field_len,
    const char *name
) {
    return name != NULL && strlen(name) == field_len && memcmp(field, name, field_len) == 0;
}

static dcc_status_t dcc_rest_response_parse_snowflake_digits(
    const char *digits,
    size_t digits_len,
    dcc_snowflake_t *out
) {
    if (digits == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (digits_len == 0) {
        return DCC_ERR_JSON;
    }

    uint64_t value = 0;
    for (size_t i = 0; i < digits_len; i++) {
        unsigned char c = (unsigned char)digits[i];
        if (c < '0' || c > '9') {
            return DCC_ERR_JSON;
        }
        uint64_t digit = (uint64_t)(c - '0');
        if (value > (UINT64_MAX - digit) / 10U) {
            return DCC_ERR_JSON;
        }
        value = value * 10U + digit;
    }
    *out = (dcc_snowflake_t)value;
    return DCC_OK;
}

dcc_status_t dcc_rest_response_snowflake_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    dcc_snowflake_t *out
) {
    if (response == NULL || field_name == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (response->error != DCC_OK) {
        return response->error;
    }
    if (response->body == NULL || response->body_len == 0) {
        return DCC_ERR_NOT_FOUND;
    }

    const char *p = dcc_rest_response_skip_ws(response->body, response->body + response->body_len);
    const char *end = response->body + response->body_len;
    if (p >= end) {
        return DCC_ERR_NOT_FOUND;
    }
    if (*p != '{') {
        return DCC_ERR_NOT_FOUND;
    }

    int depth = 0;
    while (p < end) {
        if (*p == '{' || *p == '[') {
            depth++;
            p++;
            continue;
        }
        if (*p == '}' || *p == ']') {
            if (depth == 0) {
                return DCC_ERR_JSON;
            }
            depth--;
            p++;
            if (depth == 0) {
                break;
            }
            continue;
        }
        if (*p != '"') {
            p++;
            continue;
        }

        const char *key = NULL;
        size_t key_len = 0;
        int escaped = 0;
        const char *after_key = dcc_rest_response_scan_string(p, end, &key, &key_len, &escaped);
        if (after_key == NULL) {
            return DCC_ERR_JSON;
        }

        const char *colon = dcc_rest_response_skip_ws(after_key, end);
        if (depth == 1 && !escaped && colon < end && *colon == ':' &&
            dcc_rest_response_field_matches(key, key_len, field_name)) {
            const char *value = dcc_rest_response_skip_ws(colon + 1, end);
            if (value >= end) {
                return DCC_ERR_JSON;
            }
            if (*value == '"') {
                const char *digits = NULL;
                size_t digits_len = 0;
                int value_escaped = 0;
                const char *after_value =
                    dcc_rest_response_scan_string(value, end, &digits, &digits_len, &value_escaped);
                if (after_value == NULL || value_escaped) {
                    return DCC_ERR_JSON;
                }
                (void)after_value;
                return dcc_rest_response_parse_snowflake_digits(digits, digits_len, out);
            }
            const char *digits_end = value;
            while (digits_end < end && *digits_end >= '0' && *digits_end <= '9') {
                digits_end++;
            }
            return dcc_rest_response_parse_snowflake_digits(
                value,
                (size_t)(digits_end - value),
                out
            );
        }

        p = after_key;
    }

    return DCC_ERR_NOT_FOUND;
}

dcc_status_t dcc_rest_response_message_id(
    const dcc_rest_response_t *response,
    dcc_snowflake_t *out_message_id
) {
    return dcc_rest_response_snowflake_field(response, "id", out_message_id);
}

dcc_status_t dcc_rest_response_channel_id(
    const dcc_rest_response_t *response,
    dcc_snowflake_t *out_channel_id
) {
    return dcc_rest_response_snowflake_field(response, "channel_id", out_channel_id);
}

dcc_status_t dcc_rest_response_guild_id(
    const dcc_rest_response_t *response,
    dcc_snowflake_t *out_guild_id
) {
    return dcc_rest_response_snowflake_field(response, "guild_id", out_guild_id);
}
