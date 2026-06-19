#include "internal/json/dcc_json_internal.h"

int dcc_json_utf8_scan_one(const char **cursor, const char *end) {
    const unsigned char *p = (const unsigned char *)*cursor;
    if ((const char *)p >= end) {
        return -1;
    }

    unsigned char c0 = p[0];
    if (c0 <= 0x7fU) {
        if (c0 < 0x20U) {
            return -1;
        }
        *cursor += 1;
        return 0;
    }
    if (c0 >= 0xc2U && c0 <= 0xdfU) {
        if (end - *cursor < 2 || (p[1] & 0xc0U) != 0x80U) {
            return -1;
        }
        *cursor += 2;
        return 0;
    }
    if (c0 == 0xe0U) {
        if (end - *cursor < 3 || p[1] < 0xa0U || p[1] > 0xbfU || (p[2] & 0xc0U) != 0x80U) {
            return -1;
        }
        *cursor += 3;
        return 0;
    }
    if (c0 >= 0xe1U && c0 <= 0xecU) {
        if (end - *cursor < 3 || (p[1] & 0xc0U) != 0x80U || (p[2] & 0xc0U) != 0x80U) {
            return -1;
        }
        *cursor += 3;
        return 0;
    }
    if (c0 == 0xedU) {
        if (end - *cursor < 3 || p[1] < 0x80U || p[1] > 0x9fU || (p[2] & 0xc0U) != 0x80U) {
            return -1;
        }
        *cursor += 3;
        return 0;
    }
    if (c0 >= 0xeeU && c0 <= 0xefU) {
        if (end - *cursor < 3 || (p[1] & 0xc0U) != 0x80U || (p[2] & 0xc0U) != 0x80U) {
            return -1;
        }
        *cursor += 3;
        return 0;
    }
    if (c0 == 0xf0U) {
        if (end - *cursor < 4 || p[1] < 0x90U || p[1] > 0xbfU ||
            (p[2] & 0xc0U) != 0x80U || (p[3] & 0xc0U) != 0x80U) {
            return -1;
        }
        *cursor += 4;
        return 0;
    }
    if (c0 >= 0xf1U && c0 <= 0xf3U) {
        if (end - *cursor < 4 || (p[1] & 0xc0U) != 0x80U ||
            (p[2] & 0xc0U) != 0x80U || (p[3] & 0xc0U) != 0x80U) {
            return -1;
        }
        *cursor += 4;
        return 0;
    }
    if (c0 == 0xf4U) {
        if (end - *cursor < 4 || p[1] < 0x80U || p[1] > 0x8fU ||
            (p[2] & 0xc0U) != 0x80U || (p[3] & 0xc0U) != 0x80U) {
            return -1;
        }
        *cursor += 4;
        return 0;
    }
    return -1;
}
