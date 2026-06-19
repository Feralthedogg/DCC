#include "internal/rest/dcc_rest_rate_limit_internal.h"

#include <string.h>

static int dcc_rest_ascii_lower(int c) {
    return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
}

static int dcc_rest_ascii_case_eq_span(const char *left, size_t left_len, const char *right) {
    size_t right_len = strlen(right);
    if (left == NULL || left_len != right_len) {
        return 0;
    }
    for (size_t i = 0; i < left_len; i++) {
        if (dcc_rest_ascii_lower((unsigned char)left[i]) != dcc_rest_ascii_lower((unsigned char)right[i])) {
            return 0;
        }
    }
    return 1;
}

const char *dcc_rest_response_header(
    const dcc_http_response_t *response,
    const char *name,
    size_t *out_len
) {
    if (response == NULL || response->headers == NULL || name == NULL) {
        return NULL;
    }

    const char *p = response->headers;
    const char *end = response->headers + response->headers_len;
    while (p < end) {
        const char *line_end = p;
        while (line_end < end && *line_end != '\r' && *line_end != '\n') {
            line_end++;
        }

        const char *colon = memchr(p, ':', (size_t)(line_end - p));
        if (colon != NULL && dcc_rest_ascii_case_eq_span(p, (size_t)(colon - p), name)) {
            const char *value = colon + 1;
            while (value < line_end && (*value == ' ' || *value == '\t')) {
                value++;
            }
            while (line_end > value && (line_end[-1] == ' ' || line_end[-1] == '\t')) {
                line_end--;
            }
            if (out_len != NULL) {
                *out_len = (size_t)(line_end - value);
            }
            return value;
        }

        p = line_end;
        while (p < end && (*p == '\r' || *p == '\n')) {
            p++;
        }
    }
    return NULL;
}
