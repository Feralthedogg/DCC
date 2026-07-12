#include "internal/replay/dcc_replay_internal.h"
#include "internal/json/dcc_json.h"

#include <stdint.h>
#include <string.h>

dcc_status_t dcc_replay_json_escape(FILE *file, const char *text) {
    if (file == NULL || text == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    for (const unsigned char *p = (const unsigned char *)text; *p != '\0'; ++p) {
        switch (*p) {
            case '"':
                if (fputs("\\\"", file) < 0) {
                    return DCC_ERR_RUNTIME;
                }
                break;
            case '\\':
                if (fputs("\\\\", file) < 0) {
                    return DCC_ERR_RUNTIME;
                }
                break;
            case '\b':
                if (fputs("\\b", file) < 0) {
                    return DCC_ERR_RUNTIME;
                }
                break;
            case '\f':
                if (fputs("\\f", file) < 0) {
                    return DCC_ERR_RUNTIME;
                }
                break;
            case '\n':
                if (fputs("\\n", file) < 0) {
                    return DCC_ERR_RUNTIME;
                }
                break;
            case '\r':
                if (fputs("\\r", file) < 0) {
                    return DCC_ERR_RUNTIME;
                }
                break;
            case '\t':
                if (fputs("\\t", file) < 0) {
                    return DCC_ERR_RUNTIME;
                }
                break;
            default:
                if (*p < 0x20U) {
                    if (fprintf(file, "\\u%04x", (unsigned)*p) < 0) {
                        return DCC_ERR_RUNTIME;
                    }
                } else if (fputc(*p, file) == EOF) {
                    return DCC_ERR_RUNTIME;
                }
                break;
        }
    }
    return DCC_OK;
}

int dcc_replay_payload_shape_ok(const char *payload, size_t payload_len) {
    if (payload == NULL || payload_len == 0U) {
        return 0;
    }
    size_t begin = 0;
    while (begin < payload_len &&
           (payload[begin] == ' ' || payload[begin] == '\t' ||
            payload[begin] == '\r' || payload[begin] == '\n')) {
        ++begin;
    }
    if (begin == payload_len) {
        return 0;
    }
    char first = payload[begin];
    if (first != '{' && first != '[') {
        return 0;
    }
    dcc_json_t *json = NULL;
    dcc_status_t status = dcc_json_parse(payload, payload_len, &json);
    int ok = status == DCC_OK &&
        (dcc_json_typeof(json) == DCC_JSON_OBJECT || dcc_json_typeof(json) == DCC_JSON_ARRAY);
    dcc_json_free(json);
    return ok;
}
