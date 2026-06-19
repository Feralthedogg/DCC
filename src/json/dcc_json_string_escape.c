#include "internal/json/dcc_json_internal.h"

static int dcc_json_hex_value(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

int dcc_json_parse_hex4_at(const char *p, uint32_t *out) {
    uint32_t value = 0;
    for (size_t i = 0; i < 4U; ++i) {
        int h = dcc_json_hex_value(p[i]);
        if (h < 0) {
            return -1;
        }
        value = (value << 4U) | (uint32_t)h;
    }
    *out = value;
    return 0;
}
