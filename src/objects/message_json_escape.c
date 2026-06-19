#include "internal/objects/dcc_message_json_buffer_internal.h"

dcc_status_t dcc_message_json_append_escaped_string(
    dcc_message_json_buffer_t *buffer,
    const char *value
) {
    static const char hex[] = "0123456789ABCDEF";

    dcc_status_t status = dcc_message_json_append_cstr(buffer, "\"");
    if (status != DCC_OK) {
        return status;
    }

    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        unsigned char c = *p;
        switch (c) {
            case '"':
                status = dcc_message_json_append_cstr(buffer, "\\\"");
                break;
            case '\\':
                status = dcc_message_json_append_cstr(buffer, "\\\\");
                break;
            case '\b':
                status = dcc_message_json_append_cstr(buffer, "\\b");
                break;
            case '\f':
                status = dcc_message_json_append_cstr(buffer, "\\f");
                break;
            case '\n':
                status = dcc_message_json_append_cstr(buffer, "\\n");
                break;
            case '\r':
                status = dcc_message_json_append_cstr(buffer, "\\r");
                break;
            case '\t':
                status = dcc_message_json_append_cstr(buffer, "\\t");
                break;
            default:
                if (c < 0x20U) {
                    char escaped[6] = {
                        '\\',
                        'u',
                        '0',
                        '0',
                        hex[c >> 4U],
                        hex[c & 0x0FU],
                    };
                    status = dcc_message_json_append(buffer, escaped, sizeof(escaped));
                } else {
                    status = dcc_message_json_append(buffer, &c, 1U);
                }
                break;
        }
        if (status != DCC_OK) {
            return status;
        }
    }

    return dcc_message_json_append_cstr(buffer, "\"");
}
