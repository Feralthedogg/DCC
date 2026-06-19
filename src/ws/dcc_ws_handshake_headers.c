#include "internal/ws/dcc_ws_handshake_helpers_internal.h"

#include <string.h>

static int dcc_ws_ascii_ieq_n(const char *lhs, const char *rhs, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        unsigned char a = (unsigned char)lhs[i];
        unsigned char b = (unsigned char)rhs[i];
        if (a >= 'A' && a <= 'Z') {
            a = (unsigned char)(a - 'A' + 'a');
        }
        if (b >= 'A' && b <= 'Z') {
            b = (unsigned char)(b - 'A' + 'a');
        }
        if (a != b) {
            return 0;
        }
    }
    return 1;
}

char *dcc_ws_header_value(char *headers, const char *name) {
    size_t name_len = strlen(name);
    char *line = headers;
    while (line != NULL && *line != '\0') {
        char *next = strstr(line, "\r\n");
        if (next != NULL) {
            *next = '\0';
        }

        char *colon = strchr(line, ':');
        if (colon != NULL && (size_t)(colon - line) == name_len && dcc_ws_ascii_ieq_n(line, name, name_len)) {
            char *value = colon + 1;
            while (*value == ' ' || *value == '\t') {
                value++;
            }
            if (next != NULL) {
                *next = '\r';
            }
            return value;
        }

        if (next == NULL) {
            break;
        }
        *next = '\r';
        line = next + 2;
    }
    return NULL;
}

char *dcc_ws_find_header_end(unsigned char *data, size_t len) {
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
