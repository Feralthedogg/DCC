#include "internal/http/dcc_http.h"

#include <ctype.h>
#include <string.h>

static int dcc_http_ascii_ieq_n(const char *lhs, const char *rhs, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (tolower((unsigned char)lhs[i]) != tolower((unsigned char)rhs[i])) {
            return 0;
        }
    }
    return 1;
}

int dcc_http_headers_token_contains(
    const char *headers,
    size_t headers_len,
    const char *name,
    const char *token
) {
    size_t name_len = strlen(name);
    size_t token_len = strlen(token);
    const char *line = headers;
    const char *end = headers + headers_len;

    while (line < end) {
        const char *line_end = memchr(line, '\n', (size_t)(end - line));
        if (line_end == NULL) {
            line_end = end;
        }
        const char *colon = memchr(line, ':', (size_t)(line_end - line));
        if (colon != NULL &&
            (size_t)(colon - line) == name_len &&
            dcc_http_ascii_ieq_n(line, name, name_len)) {
            const char *value = colon + 1;
            while (value < line_end && (*value == ' ' || *value == '\t')) {
                value++;
            }
            for (const char *p = value; p + token_len <= line_end; ++p) {
                if (dcc_http_ascii_ieq_n(p, token, token_len)) {
                    return 1;
                }
            }
        }
        line = line_end < end ? line_end + 1 : end;
        if (line < end && *line == '\r') {
            line++;
        }
    }
    return 0;
}

char *dcc_http_find_header_end(const char *data, size_t len) {
    if (len < 4) {
        return NULL;
    }
    for (size_t i = 0; i + 3 < len; ++i) {
        if (data[i] == '\r' && data[i + 1] == '\n' && data[i + 2] == '\r' && data[i + 3] == '\n') {
            return (char *)(data + i);
        }
    }
    return NULL;
}
