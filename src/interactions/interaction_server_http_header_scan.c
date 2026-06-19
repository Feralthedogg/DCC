#include "internal/interactions/dcc_interaction_server_request_internal.h"

size_t dcc_interaction_find_header_end(const char *data, size_t len) {
    if (data == NULL || len < 4U) {
        return 0;
    }
    for (size_t i = 0; i + 3U < len; ++i) {
        if (data[i] == '\r' && data[i + 1U] == '\n' &&
            data[i + 2U] == '\r' && data[i + 3U] == '\n') {
            return i + 4U;
        }
    }
    return 0;
}

static const char *dcc_interaction_line_end(const char *cur, const char *end) {
    while (cur + 1 < end) {
        if (cur[0] == '\r' && cur[1] == '\n') {
            return cur;
        }
        cur++;
    }
    return NULL;
}

dcc_status_t dcc_interaction_parse_content_length(
    const char *headers,
    size_t header_len,
    size_t *out
) {
    const char *cur = headers;
    const char *end = headers + header_len;
    const char *line_end = dcc_interaction_line_end(cur, end);
    *out = 0;
    if (line_end == NULL) {
        return DCC_ERR_JSON;
    }
    cur = line_end + 2;

    while (cur < end) {
        line_end = dcc_interaction_line_end(cur, end);
        if (line_end == NULL) {
            return DCC_ERR_JSON;
        }
        if (line_end == cur) {
            return DCC_OK;
        }

        const char *colon = cur;
        while (colon < line_end && *colon != ':') {
            colon++;
        }
        if (colon < line_end) {
            const char *name_end = colon;
            while (name_end > cur && (name_end[-1] == ' ' || name_end[-1] == '\t')) {
                name_end--;
            }
            if (dcc_interaction_ascii_eq_n(cur, (size_t)(name_end - cur), "content-length")) {
                const char *value = colon + 1;
                while (value < line_end && (*value == ' ' || *value == '\t')) {
                    value++;
                }
                size_t parsed = 0;
                if (value == line_end) {
                    return DCC_ERR_JSON;
                }
                while (value < line_end) {
                    if (*value < '0' || *value > '9') {
                        return DCC_ERR_JSON;
                    }
                    size_t digit = (size_t)(*value - '0');
                    if (parsed > (SIZE_MAX - digit) / 10U) {
                        return DCC_ERR_NOMEM;
                    }
                    parsed = parsed * 10U + digit;
                    value++;
                }
                *out = parsed;
                return DCC_OK;
            }
        }
        cur = line_end + 2;
    }
    return DCC_OK;
}
