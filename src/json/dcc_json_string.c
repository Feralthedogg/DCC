#include "internal/json/dcc_json_internal.h"

#include <stdbool.h>
#include <stdint.h>

int dcc_json_find_string_end(dcc_json_parser_t *parser, const char **raw_start, const char **raw_end, bool *escaped) {
    if (parser->cur >= parser->end || *parser->cur != '"') {
        return -1;
    }
    const char *p = parser->cur + 1;
    *raw_start = p;
    *escaped = false;

    while (p < parser->end) {
        unsigned char c = (unsigned char)*p;
        if (c == '"') {
            *raw_end = p;
            return 0;
        }
        if (c < 0x20U) {
            return -1;
        }
        if (c == '\\') {
            *escaped = true;
            p++;
            if (p >= parser->end) {
                return -1;
            }
            char esc = *p++;
            switch (esc) {
                case '"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                    break;
                case 'u':
                    if (parser->end - p < 4 || dcc_json_parse_hex4_at(p, &(uint32_t){0}) != 0) {
                        return -1;
                    }
                    p += 4;
                    break;
                default:
                    return -1;
            }
            continue;
        }
        if (c >= 0x80U) {
            if (dcc_json_utf8_scan_one(&p, parser->end) != 0) {
                return -1;
            }
            continue;
        }
        p++;
    }

    return -1;
}
