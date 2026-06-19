#include "internal/rest/dcc_rest_rate_limit_internal.h"

#include <stdlib.h>
#include <string.h>

static int dcc_rest_header_ascii_lower(int c) {
    return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c;
}

double dcc_rest_parse_header_double(const char *value, size_t len) {
    if (value == NULL || len == 0) {
        return 0.0;
    }
    char tmp[64];
    if (len >= sizeof(tmp)) {
        len = sizeof(tmp) - 1U;
    }
    memcpy(tmp, value, len);
    tmp[len] = '\0';
    char *end = NULL;
    double parsed = strtod(tmp, &end);
    return end != tmp ? parsed : 0.0;
}

int dcc_rest_parse_header_u32(const char *value, size_t len, uint32_t *out) {
    if (value == NULL || len == 0 || out == NULL) {
        return 0;
    }
    char tmp[32];
    if (len >= sizeof(tmp)) {
        return 0;
    }
    memcpy(tmp, value, len);
    tmp[len] = '\0';
    char *end = NULL;
    unsigned long parsed = strtoul(tmp, &end, 10);
    if (end == tmp || *end != '\0' || parsed > UINT32_MAX) {
        return 0;
    }
    *out = (uint32_t)parsed;
    return 1;
}

uint8_t dcc_rest_header_bool_true(const char *value, size_t len) {
    return len == 4U &&
        dcc_rest_header_ascii_lower((unsigned char)value[0]) == 't' &&
        dcc_rest_header_ascii_lower((unsigned char)value[1]) == 'r' &&
        dcc_rest_header_ascii_lower((unsigned char)value[2]) == 'u' &&
        dcc_rest_header_ascii_lower((unsigned char)value[3]) == 'e';
}
