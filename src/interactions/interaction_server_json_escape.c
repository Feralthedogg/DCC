#include "internal/interactions/dcc_interaction_health_json_internal.h"

dcc_status_t dcc_interaction_json_escape_string(const char *value, char *out, size_t out_size) {
    if (out == NULL || out_size == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    if (value == NULL) {
        value = "";
    }

    size_t used = 0;
#define DCC_INTERACTION_JSON_PUTC(ch) \
    do { \
        if (used + 1U >= out_size) { \
            return DCC_ERR_NOMEM; \
        } \
        out[used++] = (char)(ch); \
    } while (0)

    DCC_INTERACTION_JSON_PUTC('"');
    for (const unsigned char *cur = (const unsigned char *)value; *cur != '\0'; ++cur) {
        unsigned char ch = *cur;
        switch (ch) {
            case '"':
                DCC_INTERACTION_JSON_PUTC('\\');
                DCC_INTERACTION_JSON_PUTC('"');
                break;
            case '\\':
                DCC_INTERACTION_JSON_PUTC('\\');
                DCC_INTERACTION_JSON_PUTC('\\');
                break;
            case '\b':
                DCC_INTERACTION_JSON_PUTC('\\');
                DCC_INTERACTION_JSON_PUTC('b');
                break;
            case '\f':
                DCC_INTERACTION_JSON_PUTC('\\');
                DCC_INTERACTION_JSON_PUTC('f');
                break;
            case '\n':
                DCC_INTERACTION_JSON_PUTC('\\');
                DCC_INTERACTION_JSON_PUTC('n');
                break;
            case '\r':
                DCC_INTERACTION_JSON_PUTC('\\');
                DCC_INTERACTION_JSON_PUTC('r');
                break;
            case '\t':
                DCC_INTERACTION_JSON_PUTC('\\');
                DCC_INTERACTION_JSON_PUTC('t');
                break;
            default:
                if (ch < 0x20U) {
                    static const char hex[] = "0123456789abcdef";
                    DCC_INTERACTION_JSON_PUTC('\\');
                    DCC_INTERACTION_JSON_PUTC('u');
                    DCC_INTERACTION_JSON_PUTC('0');
                    DCC_INTERACTION_JSON_PUTC('0');
                    DCC_INTERACTION_JSON_PUTC('0');
                    DCC_INTERACTION_JSON_PUTC(hex[ch >> 4U]);
                    DCC_INTERACTION_JSON_PUTC(hex[ch & 0x0fU]);
                } else {
                    DCC_INTERACTION_JSON_PUTC(ch);
                }
                break;
        }
    }
    DCC_INTERACTION_JSON_PUTC('"');
    out[used] = '\0';
#undef DCC_INTERACTION_JSON_PUTC
    return DCC_OK;
}
