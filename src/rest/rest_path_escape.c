#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>

static int dcc_rest_path_unreserved(unsigned char c) {
    return (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') ||
        c == '-' ||
        c == '.' ||
        c == '_' ||
        c == '~';
}

dcc_status_t dcc_rest_escape_path_segment(const char *value, char **out) {
    static const char hex[] = "0123456789ABCDEF";

    if (value == NULL || value[0] == '\0' || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    size_t encoded_len = 0;
    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        size_t add = dcc_rest_path_unreserved(*p) ? 1U : 3U;
        if (encoded_len > SIZE_MAX - add) {
            return DCC_ERR_NOMEM;
        }
        encoded_len += add;
    }
    if (encoded_len == SIZE_MAX) {
        return DCC_ERR_NOMEM;
    }

    char *encoded = (char *)malloc(encoded_len + 1U);
    if (encoded == NULL) {
        return DCC_ERR_NOMEM;
    }

    char *w = encoded;
    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        if (dcc_rest_path_unreserved(*p)) {
            *w++ = (char)*p;
        } else {
            *w++ = '%';
            *w++ = hex[*p >> 4U];
            *w++ = hex[*p & 0x0FU];
        }
    }
    *w = '\0';
    *out = encoded;
    return DCC_OK;
}
