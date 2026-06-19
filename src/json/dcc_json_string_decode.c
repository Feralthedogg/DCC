#include "internal/json/dcc_json_internal.h"

#include <stdint.h>

int dcc_json_decode_string_span(const char *start, const char *end, char *out, size_t out_cap) {
    char *dst = out;
    char *limit = out + out_cap;
    const char *p = start;

    while (p < end) {
        unsigned char c = (unsigned char)*p++;
        if (c != '\\') {
            if (dst >= limit) {
                return -1;
            }
            *dst++ = (char)c;
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
                if (dst >= limit) {
                    return -1;
                }
                *dst++ = esc;
                break;
            case 'b':
                if (dst >= limit) {
                    return -1;
                }
                *dst++ = '\b';
                break;
            case 'f':
                if (dst >= limit) {
                    return -1;
                }
                *dst++ = '\f';
                break;
            case 'n':
                if (dst >= limit) {
                    return -1;
                }
                *dst++ = '\n';
                break;
            case 'r':
                if (dst >= limit) {
                    return -1;
                }
                *dst++ = '\r';
                break;
            case 't':
                if (dst >= limit) {
                    return -1;
                }
                *dst++ = '\t';
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
                if (dcc_json_utf8_append(&dst, limit, cp) != 0) {
                    return -1;
                }
                break;
            }
            default:
                return -1;
        }
    }

    if (dst >= limit) {
        return -1;
    }
    *dst = '\0';
    return 0;
}
