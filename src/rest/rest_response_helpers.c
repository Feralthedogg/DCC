#include <dcc/rest/response_helpers.h>

#include "internal/json/dcc_json_memory_string.h"

#include <limits.h>
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

static int dcc_rest_response_value_done(const char *p, const char *end) {
    p = dcc_rest_response_skip_ws(p, end);
    return p >= end || *p == ',' || *p == '}';
}

static dcc_status_t dcc_rest_response_find_top_level_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    const char **out_value,
    const char **out_end
) {
    if (response == NULL || field_name == NULL || out_value == NULL || out_end == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (response->error != DCC_OK) {
        return response->error;
    }
    if (response->body == NULL || response->body_len == 0) {
        return DCC_ERR_NOT_FOUND;
    }

    const char *end = response->body + response->body_len;
    const char *p = dcc_rest_response_skip_ws(response->body, end);
    if (p >= end || *p != '{') {
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
            *out_value = dcc_rest_response_skip_ws(colon + 1, end);
            *out_end = end;
            return *out_value < end ? DCC_OK : DCC_ERR_JSON;
        }

        p = after_key;
    }

    return DCC_ERR_NOT_FOUND;
}

static dcc_status_t dcc_rest_response_parse_u64_digits(
    const char *digits,
    size_t digits_len,
    uint64_t *out
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
    *out = value;
    return DCC_OK;
}

static dcc_status_t dcc_rest_response_parse_i64_digits(
    const char *digits,
    size_t digits_len,
    int64_t *out
) {
    if (digits == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (digits_len == 0) {
        return DCC_ERR_JSON;
    }

    size_t pos = 0;
    int negative = 0;
    if (digits[0] == '-') {
        negative = 1;
        pos = 1;
        if (pos == digits_len) {
            return DCC_ERR_JSON;
        }
    }

    const uint64_t limit = negative ? ((uint64_t)INT64_MAX + 1U) : (uint64_t)INT64_MAX;
    uint64_t value = 0;
    for (; pos < digits_len; pos++) {
        unsigned char c = (unsigned char)digits[pos];
        if (c < '0' || c > '9') {
            return DCC_ERR_JSON;
        }
        uint64_t digit = (uint64_t)(c - '0');
        if (value > (limit - digit) / 10U) {
            return DCC_ERR_JSON;
        }
        value = value * 10U + digit;
    }

    if (negative) {
        if (value == (uint64_t)INT64_MAX + 1U) {
            *out = INT64_MIN;
        } else {
            *out = -(int64_t)value;
        }
    } else {
        *out = (int64_t)value;
    }
    return DCC_OK;
}

static dcc_status_t dcc_rest_response_parse_u64_value(
    const char *value,
    const char *end,
    uint64_t *out
) {
    if (value == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (value >= end) {
        return DCC_ERR_JSON;
    }

    if (*value == '"') {
        const char *digits = NULL;
        size_t digits_len = 0;
        int escaped = 0;
        const char *after = dcc_rest_response_scan_string(value, end, &digits, &digits_len, &escaped);
        if (after == NULL || escaped || !dcc_rest_response_value_done(after, end)) {
            return DCC_ERR_JSON;
        }
        return dcc_rest_response_parse_u64_digits(digits, digits_len, out);
    }

    const char *digits_end = value;
    while (digits_end < end && *digits_end >= '0' && *digits_end <= '9') {
        digits_end++;
    }
    if (!dcc_rest_response_value_done(digits_end, end)) {
        return DCC_ERR_JSON;
    }
    return dcc_rest_response_parse_u64_digits(value, (size_t)(digits_end - value), out);
}

static dcc_status_t dcc_rest_response_parse_i64_value(
    const char *value,
    const char *end,
    int64_t *out
) {
    if (value == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (value >= end) {
        return DCC_ERR_JSON;
    }

    if (*value == '"') {
        const char *digits = NULL;
        size_t digits_len = 0;
        int escaped = 0;
        const char *after = dcc_rest_response_scan_string(value, end, &digits, &digits_len, &escaped);
        if (after == NULL || escaped || !dcc_rest_response_value_done(after, end)) {
            return DCC_ERR_JSON;
        }
        return dcc_rest_response_parse_i64_digits(digits, digits_len, out);
    }

    const char *digits_end = value;
    if (digits_end < end && *digits_end == '-') {
        digits_end++;
    }
    while (digits_end < end && *digits_end >= '0' && *digits_end <= '9') {
        digits_end++;
    }
    if (!dcc_rest_response_value_done(digits_end, end)) {
        return DCC_ERR_JSON;
    }
    return dcc_rest_response_parse_i64_digits(value, (size_t)(digits_end - value), out);
}

static int dcc_rest_response_utf8_len(uint32_t cp, size_t *out_len) {
    if (out_len == NULL || cp > 0x10ffffU) {
        return -1;
    }
    if (cp <= 0x7fU) {
        *out_len = 1U;
    } else if (cp <= 0x7ffU) {
        *out_len = 2U;
    } else if (cp <= 0xffffU) {
        *out_len = 3U;
    } else {
        *out_len = 4U;
    }
    return 0;
}

static int dcc_rest_response_add_size(size_t *value, size_t add) {
    if (value == NULL || *value > SIZE_MAX - add) {
        return -1;
    }
    *value += add;
    return 0;
}

static int dcc_rest_response_decoded_string_len(
    const char *start,
    const char *end,
    size_t *out_len
) {
    if (start == NULL || end == NULL || out_len == NULL) {
        return -1;
    }

    size_t len = 0;
    const char *p = start;
    while (p < end) {
        unsigned char c = (unsigned char)*p++;
        if (c != '\\') {
            if (dcc_rest_response_add_size(&len, 1U) != 0) {
                return -1;
            }
            continue;
        }

        if (p >= end) {
            return -1;
        }
        char esc = *p++;
        switch (esc) {
            case '"':
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
                if (dcc_rest_response_add_size(&len, 1U) != 0) {
                    return -1;
                }
                break;
            case 'u': {
                uint32_t cp = 0;
                if (end - p < 4 || dcc_json_parse_hex4_at(p, &cp) != 0) {
                    return -1;
                }
                p += 4;
                if (cp >= 0xd800U && cp <= 0xdbffU) {
                    uint32_t low = 0;
                    if (end - p < 6 || p[0] != '\\' || p[1] != 'u' ||
                        dcc_json_parse_hex4_at(p + 2, &low) != 0 ||
                        low < 0xdc00U || low > 0xdfffU) {
                        return -1;
                    }
                    p += 6;
                    cp = 0x10000U + (((cp - 0xd800U) << 10U) | (low - 0xdc00U));
                } else if (cp >= 0xdc00U && cp <= 0xdfffU) {
                    return -1;
                }
                size_t cp_len = 0;
                if (dcc_rest_response_utf8_len(cp, &cp_len) != 0 ||
                    dcc_rest_response_add_size(&len, cp_len) != 0) {
                    return -1;
                }
                break;
            }
            default:
                return -1;
        }
    }

    *out_len = len;
    return 0;
}

dcc_status_t dcc_rest_response_u64_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    uint64_t *out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = NULL;
    const char *end = NULL;
    dcc_status_t status = dcc_rest_response_find_top_level_field(response, field_name, &value, &end);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_rest_response_parse_u64_value(value, end, out);
}

dcc_status_t dcc_rest_response_i64_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    int64_t *out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = NULL;
    const char *end = NULL;
    dcc_status_t status = dcc_rest_response_find_top_level_field(response, field_name, &value, &end);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_rest_response_parse_i64_value(value, end, out);
}

dcc_status_t dcc_rest_response_bool_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    int *out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *value = NULL;
    const char *end = NULL;
    dcc_status_t status = dcc_rest_response_find_top_level_field(response, field_name, &value, &end);
    if (status != DCC_OK) {
        return status;
    }
    if (end - value >= 4 && memcmp(value, "true", 4U) == 0 &&
        dcc_rest_response_value_done(value + 4, end)) {
        *out = 1;
        return DCC_OK;
    }
    if (end - value >= 5 && memcmp(value, "false", 5U) == 0 &&
        dcc_rest_response_value_done(value + 5, end)) {
        *out = 0;
        return DCC_OK;
    }
    return DCC_ERR_JSON;
}

dcc_status_t dcc_rest_response_string_field_len(
    const dcc_rest_response_t *response,
    const char *field_name,
    char *out,
    size_t out_cap,
    size_t *out_len
) {
    if (out == NULL || out_cap == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    const char *value = NULL;
    const char *end = NULL;
    dcc_status_t status = dcc_rest_response_find_top_level_field(response, field_name, &value, &end);
    if (status != DCC_OK) {
        return status;
    }
    if (value >= end || *value != '"') {
        return DCC_ERR_JSON;
    }

    const char *raw = NULL;
    size_t raw_len = 0;
    int escaped = 0;
    const char *after = dcc_rest_response_scan_string(value, end, &raw, &raw_len, &escaped);
    if (after == NULL || !dcc_rest_response_value_done(after, end)) {
        return DCC_ERR_JSON;
    }

    size_t decoded_len = raw_len;
    if (escaped && dcc_rest_response_decoded_string_len(raw, raw + raw_len, &decoded_len) != 0) {
        return DCC_ERR_JSON;
    }
    if (decoded_len >= out_cap) {
        return DCC_ERR_NOMEM;
    }

    if (!escaped) {
        memcpy(out, raw, raw_len);
        out[raw_len] = '\0';
    } else if (dcc_json_decode_string_span(raw, raw + raw_len, out, out_cap) != 0) {
        return DCC_ERR_JSON;
    }
    if (out_len != NULL) {
        *out_len = decoded_len;
    }
    return DCC_OK;
}

dcc_status_t dcc_rest_response_string_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    char *out,
    size_t out_cap
) {
    return dcc_rest_response_string_field_len(response, field_name, out, out_cap, NULL);
}

dcc_status_t dcc_rest_response_snowflake_field(
    const dcc_rest_response_t *response,
    const char *field_name,
    dcc_snowflake_t *out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    uint64_t value = 0;
    dcc_status_t status = dcc_rest_response_u64_field(response, field_name, &value);
    if (status != DCC_OK) {
        return status;
    }
    *out = (dcc_snowflake_t)value;
    return DCC_OK;
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
