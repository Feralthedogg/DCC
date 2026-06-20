#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len7(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "options")) {
        return DCC_JSON_KEY_OPTIONS;
    }
    if (dcc_json_key_span_eq(key, len, "default")) {
        return DCC_JSON_KEY_DEFAULT;
    }
    if (dcc_json_key_span_eq(key, len, "divider")) {
        return DCC_JSON_KEY_DIVIDER;
    }
    if (dcc_json_key_span_eq(key, len, "spacing")) {
        return DCC_JSON_KEY_SPACING;
    }
    if (dcc_json_key_span_eq(key, len, "spoiler")) {
        return DCC_JSON_KEY_SPOILER;
    }
    if (dcc_json_key_span_eq(key, len, "focused")) {
        return DCC_JSON_KEY_FOCUSED;
    }
    if (dcc_json_key_span_eq(key, len, "content")) {
        return DCC_JSON_KEY_CONTENT;
    }
    if (dcc_json_key_span_eq(key, len, "threads")) {
        return DCC_JSON_KEY_THREADS;
    }
    if (dcc_json_key_span_eq(key, len, "members")) {
        return DCC_JSON_KEY_MEMBERS;
    }
    if (dcc_json_key_span_eq(key, len, "desktop")) {
        return DCC_JSON_KEY_DESKTOP;
    }
    if (dcc_json_key_span_eq(key, len, "user_id")) {
        return DCC_JSON_KEY_USER_ID;
    }
    if (dcc_json_key_span_eq(key, len, "pack_id")) {
        return DCC_JSON_KEY_PACK_ID;
    }
    if (dcc_json_key_span_eq(key, len, "role_id")) {
        return DCC_JSON_KEY_ROLE_ID;
    }
    if (dcc_json_key_span_eq(key, len, "inviter")) {
        return DCC_JSON_KEY_INVITER;
    }
    if (dcc_json_key_span_eq(key, len, "rule_id")) {
        return DCC_JSON_KEY_RULE_ID;
    }
    if (dcc_json_key_span_eq(key, len, "max_age")) {
        return DCC_JSON_KEY_MAX_AGE;
    }
    if (dcc_json_key_span_eq(key, len, "pending")) {
        return DCC_JSON_KEY_PENDING;
    }
    if (dcc_json_key_span_eq(key, len, "enabled")) {
        return DCC_JSON_KEY_ENABLED;
    }
    if (dcc_json_key_span_eq(key, len, "sku_ids")) {
        return DCC_JSON_KEY_SKU_IDS;
    }
    if (dcc_json_key_span_eq(key, len, "ends_at")) {
        return DCC_JSON_KEY_ENDS_AT;
    }
    if (dcc_json_key_span_eq(key, len, "details")) {
        return DCC_JSON_KEY_DETAILS;
    }
    if (dcc_json_key_span_eq(key, len, "managed")) {
        return DCC_JSON_KEY_MANAGED;
    }
    if (dcc_json_key_span_eq(key, len, "bitrate")) {
        return DCC_JSON_KEY_BITRATE;
    }
    if (dcc_json_key_span_eq(key, len, "creator")) {
        return DCC_JSON_KEY_CREATOR;
    }
    if (dcc_json_key_span_eq(key, len, "version")) {
        return DCC_JSON_KEY_VERSION;
    }
    if (dcc_json_key_span_eq(key, len, "context")) {
        return DCC_JSON_KEY_CONTEXT;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
