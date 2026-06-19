#include "internal/json/dcc_json_internal.h"

#include <string.h>

int dcc_json_utf8_append(char **out, const char *limit, uint32_t codepoint) {
    char tmp[4];
    size_t len = 0;
    if (codepoint <= 0x7fU) {
        tmp[len++] = (char)codepoint;
    } else if (codepoint <= 0x7ffU) {
        tmp[len++] = (char)(0xc0U | ((codepoint >> 6U) & 0x1fU));
        tmp[len++] = (char)(0x80U | (codepoint & 0x3fU));
    } else if (codepoint <= 0xffffU) {
        if (codepoint >= 0xd800U && codepoint <= 0xdfffU) {
            return -1;
        }
        tmp[len++] = (char)(0xe0U | ((codepoint >> 12U) & 0x0fU));
        tmp[len++] = (char)(0x80U | ((codepoint >> 6U) & 0x3fU));
        tmp[len++] = (char)(0x80U | (codepoint & 0x3fU));
    } else if (codepoint <= 0x10ffffU) {
        tmp[len++] = (char)(0xf0U | ((codepoint >> 18U) & 0x07U));
        tmp[len++] = (char)(0x80U | ((codepoint >> 12U) & 0x3fU));
        tmp[len++] = (char)(0x80U | ((codepoint >> 6U) & 0x3fU));
        tmp[len++] = (char)(0x80U | (codepoint & 0x3fU));
    } else {
        return -1;
    }
    if ((size_t)(limit - *out) < len) {
        return -1;
    }
    memcpy(*out, tmp, len);
    *out += len;
    return 0;
}
