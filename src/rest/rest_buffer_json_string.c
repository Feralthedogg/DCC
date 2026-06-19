#include "internal/rest/dcc_rest_buffer_internal.h"

dcc_status_t dcc_rest_buffer_append_json_string(dcc_rest_buffer_t *buffer, const char *value) {
    static const char hex[] = "0123456789abcdef";

    if (buffer == NULL || value == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(buffer, "\"");
    if (status != DCC_OK) {
        return status;
    }

    const unsigned char *p = (const unsigned char *)value;
    while (*p != '\0') {
        unsigned char ch = *p++;
        switch (ch) {
            case '"':
                status = dcc_rest_buffer_append_cstr(buffer, "\\\"");
                break;
            case '\\':
                status = dcc_rest_buffer_append_cstr(buffer, "\\\\");
                break;
            case '\b':
                status = dcc_rest_buffer_append_cstr(buffer, "\\b");
                break;
            case '\f':
                status = dcc_rest_buffer_append_cstr(buffer, "\\f");
                break;
            case '\n':
                status = dcc_rest_buffer_append_cstr(buffer, "\\n");
                break;
            case '\r':
                status = dcc_rest_buffer_append_cstr(buffer, "\\r");
                break;
            case '\t':
                status = dcc_rest_buffer_append_cstr(buffer, "\\t");
                break;
            default:
                if (ch < 0x20U) {
                    char escaped[] = {'\\', 'u', '0', '0', hex[ch >> 4], hex[ch & 0x0FU]};
                    status = dcc_rest_buffer_append(buffer, escaped, sizeof(escaped));
                } else {
                    status = dcc_rest_buffer_append(buffer, &ch, 1U);
                }
                break;
        }
        if (status != DCC_OK) {
            return status;
        }
    }

    return dcc_rest_buffer_append_cstr(buffer, "\"");
}
