#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len5(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "flags")) {
        return DCC_JSON_KEY_FLAGS;
    }
    if (dcc_json_key_span_eq(key, len, "asset")) {
        return DCC_JSON_KEY_ASSET;
    }
    if (dcc_json_key_span_eq(key, len, "large")) {
        return DCC_JSON_KEY_LARGE;
    }
    if (dcc_json_key_span_eq(key, len, "roles")) {
        return DCC_JSON_KEY_ROLES;
    }
    if (dcc_json_key_span_eq(key, len, "users")) {
        return DCC_JSON_KEY_USERS;
    }
    if (dcc_json_key_span_eq(key, len, "nonce")) {
        return DCC_JSON_KEY_NONCE;
    }
    if (dcc_json_key_span_eq(key, len, "burst")) {
        return DCC_JSON_KEY_BURST;
    }
    if (dcc_json_key_span_eq(key, len, "since")) {
        return DCC_JSON_KEY_SINCE;
    }
    if (dcc_json_key_span_eq(key, len, "topic")) {
        return DCC_JSON_KEY_TOPIC;
    }
    if (dcc_json_key_span_eq(key, len, "token")) {
        return DCC_JSON_KEY_TOKEN;
    }
    if (dcc_json_key_span_eq(key, len, "emoji")) {
        return DCC_JSON_KEY_EMOJI;
    }
    if (dcc_json_key_span_eq(key, len, "count")) {
        return DCC_JSON_KEY_COUNT;
    }
    if (dcc_json_key_span_eq(key, len, "state")) {
        return DCC_JSON_KEY_STATE;
    }
    if (dcc_json_key_span_eq(key, len, "color")) {
        return DCC_JSON_KEY_COLOR;
    }
    if (dcc_json_key_span_eq(key, len, "hoist")) {
        return DCC_JSON_KEY_HOIST;
    }
    if (dcc_json_key_span_eq(key, len, "image")) {
        return DCC_JSON_KEY_IMAGE;
    }
    if (dcc_json_key_span_eq(key, len, "width")) {
        return DCC_JSON_KEY_WIDTH;
    }
    if (dcc_json_key_span_eq(key, len, "value")) {
        return DCC_JSON_KEY_VALUE;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
