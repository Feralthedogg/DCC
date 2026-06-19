#include "internal/voice/dcc_voice_internal.h"

#include <stdio.h>

int dcc_voice_json_append_string(dcc_voice_json_buffer_t *buf, const char *value) {
    if (value == NULL || dcc_voice_json_append_cstr(buf, "\"") != 0) {
        return -1;
    }

    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        unsigned char c = *p;
        if (c == '"' || c == '\\') {
            char escaped[2] = {'\\', (char)c};
            if (dcc_voice_json_append_mem(buf, escaped, sizeof(escaped)) != 0) {
                return -1;
            }
        } else if (c == '\b') {
            if (dcc_voice_json_append_cstr(buf, "\\b") != 0) {
                return -1;
            }
        } else if (c == '\f') {
            if (dcc_voice_json_append_cstr(buf, "\\f") != 0) {
                return -1;
            }
        } else if (c == '\n') {
            if (dcc_voice_json_append_cstr(buf, "\\n") != 0) {
                return -1;
            }
        } else if (c == '\r') {
            if (dcc_voice_json_append_cstr(buf, "\\r") != 0) {
                return -1;
            }
        } else if (c == '\t') {
            if (dcc_voice_json_append_cstr(buf, "\\t") != 0) {
                return -1;
            }
        } else if (c < 0x20U) {
            char escaped[7];
            int n = snprintf(escaped, sizeof(escaped), "\\u%04x", (unsigned)c);
            if (n != 6 || dcc_voice_json_append_mem(buf, escaped, 6U) != 0) {
                return -1;
            }
        } else {
            char raw = (char)c;
            if (dcc_voice_json_append_mem(buf, &raw, 1U) != 0) {
                return -1;
            }
        }
    }

    return dcc_voice_json_append_cstr(buf, "\"");
}
