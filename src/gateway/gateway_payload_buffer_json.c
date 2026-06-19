#include "internal/gateway/dcc_gateway_payload_buffer_internal.h"

int dcc_gateway_payload_buffer_append_json_string(dcc_gateway_payload_buffer_t *buf, const char *value) {
    static const char hex[] = "0123456789abcdef";
    if (dcc_gateway_payload_buffer_append_cstr(buf, "\"") != 0) {
        return -1;
    }
    if (value == NULL) {
        value = "";
    }
    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        unsigned char c = *p;
        if (c == '"' || c == '\\') {
            char escaped[2] = {'\\', (char)c};
            if (dcc_gateway_payload_buffer_append_mem(buf, escaped, sizeof(escaped)) != 0) {
                return -1;
            }
        } else if (c == '\b') {
            if (dcc_gateway_payload_buffer_append_cstr(buf, "\\b") != 0) {
                return -1;
            }
        } else if (c == '\f') {
            if (dcc_gateway_payload_buffer_append_cstr(buf, "\\f") != 0) {
                return -1;
            }
        } else if (c == '\n') {
            if (dcc_gateway_payload_buffer_append_cstr(buf, "\\n") != 0) {
                return -1;
            }
        } else if (c == '\r') {
            if (dcc_gateway_payload_buffer_append_cstr(buf, "\\r") != 0) {
                return -1;
            }
        } else if (c == '\t') {
            if (dcc_gateway_payload_buffer_append_cstr(buf, "\\t") != 0) {
                return -1;
            }
        } else if (c < 0x20U) {
            char escaped[6] = {'\\', 'u', '0', '0', hex[c >> 4U], hex[c & 0x0fU]};
            if (dcc_gateway_payload_buffer_append_mem(buf, escaped, sizeof(escaped)) != 0) {
                return -1;
            }
        } else {
            char raw = (char)c;
            if (dcc_gateway_payload_buffer_append_mem(buf, &raw, 1U) != 0) {
                return -1;
            }
        }
    }
    return dcc_gateway_payload_buffer_append_cstr(buf, "\"");
}
