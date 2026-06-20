#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len4(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "name")) {
        return DCC_JSON_KEY_NAME;
    }
    if (dcc_json_key_span_eq(key, len, "nick")) {
        return DCC_JSON_KEY_NICK;
    }
    if (dcc_json_key_span_eq(key, len, "icon")) {
        return DCC_JSON_KEY_ICON;
    }
    if (dcc_json_key_span_eq(key, len, "type")) {
        return DCC_JSON_KEY_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "user")) {
        return DCC_JSON_KEY_USER;
    }
    if (dcc_json_key_span_eq(key, len, "role")) {
        return DCC_JSON_KEY_ROLE;
    }
    if (dcc_json_key_span_eq(key, len, "data")) {
        return DCC_JSON_KEY_DATA;
    }
    if (dcc_json_key_span_eq(key, len, "file")) {
        return DCC_JSON_KEY_FILE;
    }
    if (dcc_json_key_span_eq(key, len, "tags")) {
        return DCC_JSON_KEY_TAGS;
    }
    if (dcc_json_key_span_eq(key, len, "code")) {
        return DCC_JSON_KEY_CODE;
    }
    if (dcc_json_key_span_eq(key, len, "size")) {
        return DCC_JSON_KEY_SIZE;
    }
    if (dcc_json_key_span_eq(key, len, "uses")) {
        return DCC_JSON_KEY_USES;
    }
    if (dcc_json_key_span_eq(key, len, "deaf")) {
        return DCC_JSON_KEY_DEAF;
    }
    if (dcc_json_key_span_eq(key, len, "mute")) {
        return DCC_JSON_KEY_MUTE;
    }
    if (dcc_json_key_span_eq(key, len, "nsfw")) {
        return DCC_JSON_KEY_NSFW;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
