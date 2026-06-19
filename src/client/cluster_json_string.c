#include "internal/client/dcc_cluster_json_internal.h"

int dcc_cluster_json_append_string(dcc_cluster_json_buffer_t *json, const char *text) {
    if (dcc_cluster_json_append_cstr(json, "\"") != 0) {
        return -1;
    }

    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");
    static const char hex[] = "0123456789abcdef";
    while (*cursor != '\0') {
        unsigned char ch = *cursor++;
        switch (ch) {
            case '"':
                if (dcc_cluster_json_append_cstr(json, "\\\"") != 0) {
                    return -1;
                }
                break;
            case '\\':
                if (dcc_cluster_json_append_cstr(json, "\\\\") != 0) {
                    return -1;
                }
                break;
            case '\b':
                if (dcc_cluster_json_append_cstr(json, "\\b") != 0) {
                    return -1;
                }
                break;
            case '\f':
                if (dcc_cluster_json_append_cstr(json, "\\f") != 0) {
                    return -1;
                }
                break;
            case '\n':
                if (dcc_cluster_json_append_cstr(json, "\\n") != 0) {
                    return -1;
                }
                break;
            case '\r':
                if (dcc_cluster_json_append_cstr(json, "\\r") != 0) {
                    return -1;
                }
                break;
            case '\t':
                if (dcc_cluster_json_append_cstr(json, "\\t") != 0) {
                    return -1;
                }
                break;
            default:
                if (ch < 0x20U) {
                    char escape[6] = {'\\', 'u', '0', '0', hex[ch >> 4U], hex[ch & 0x0fU]};
                    if (dcc_cluster_json_append_raw(json, escape, sizeof(escape)) != 0) {
                        return -1;
                    }
                } else if (dcc_cluster_json_append_raw(json, (const char *)&ch, 1U) != 0) {
                    return -1;
                }
                break;
        }
    }

    return dcc_cluster_json_append_cstr(json, "\"");
}
