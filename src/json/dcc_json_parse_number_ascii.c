#include "internal/json/dcc_json_internal.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

int dcc_json_parse_i64_ascii(const char *data, size_t len, int64_t *out) {
    if (data == NULL || out == NULL || len == 0) {
        return -1;
    }

    size_t pos = 0;
    int neg = 0;
    if (data[pos] == '-') {
        neg = 1;
        pos++;
        if (pos == len) {
            return -1;
        }
    }

    uint64_t limit = neg ? (uint64_t)LLONG_MAX + 1ULL : (uint64_t)LLONG_MAX;
    uint64_t value = 0;
    for (; pos < len; ++pos) {
        unsigned char c = (unsigned char)data[pos];
        if (c < '0' || c > '9') {
            return -1;
        }
        uint64_t digit = (uint64_t)(c - '0');
        if (value > (limit - digit) / 10ULL) {
            return -1;
        }
        value = value * 10ULL + digit;
    }

    if (neg) {
        *out = value == limit ? (int64_t)LLONG_MIN : -(int64_t)value;
    } else {
        *out = (int64_t)value;
    }
    return 0;
}

int dcc_json_parse_u64_ascii(const char *data, size_t len, uint64_t *out) {
    if (data == NULL || out == NULL || len == 0 || data[0] == '-') {
        return -1;
    }

    uint64_t value = 0;
    for (size_t pos = 0; pos < len; ++pos) {
        unsigned char c = (unsigned char)data[pos];
        if (c < '0' || c > '9') {
            return -1;
        }
        uint64_t digit = (uint64_t)(c - '0');
        if (value > (UINT64_MAX - digit) / 10ULL) {
            return -1;
        }
        value = value * 10ULL + digit;
    }

    *out = value;
    return 0;
}
