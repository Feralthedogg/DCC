#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len9(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "custom_id")) {
        return DCC_JSON_KEY_CUSTOM_ID;
    }
    if (dcc_json_key_span_eq(key, len, "self_deaf")) {
        return DCC_JSON_KEY_SELF_DEAF;
    }
    if (dcc_json_key_span_eq(key, len, "self_mute")) {
        return DCC_JSON_KEY_SELF_MUTE;
    }
    if (dcc_json_key_span_eq(key, len, "parent_id")) {
        return DCC_JSON_KEY_PARENT_ID;
    }
    if (dcc_json_key_span_eq(key, len, "target_id")) {
        return DCC_JSON_KEY_TARGET_ID;
    }
    if (dcc_json_key_span_eq(key, len, "timestamp")) {
        return DCC_JSON_KEY_TIMESTAMP;
    }
    if (dcc_json_key_span_eq(key, len, "proxy_url")) {
        return DCC_JSON_KEY_PROXY_URL;
    }
    if (dcc_json_key_span_eq(key, len, "ephemeral")) {
        return DCC_JSON_KEY_EPHEMERAL;
    }
    if (dcc_json_key_span_eq(key, len, "answer_id")) {
        return DCC_JSON_KEY_ANSWER_ID;
    }
    if (dcc_json_key_span_eq(key, len, "entity_id")) {
        return DCC_JSON_KEY_ENTITY_ID;
    }
    if (dcc_json_key_span_eq(key, len, "joined_at")) {
        return DCC_JSON_KEY_JOINED_AT;
    }
    if (dcc_json_key_span_eq(key, len, "starts_at")) {
        return DCC_JSON_KEY_STARTS_AT;
    }
    if (dcc_json_key_span_eq(key, len, "not_found")) {
        return DCC_JSON_KEY_NOT_FOUND;
    }
    if (dcc_json_key_span_eq(key, len, "temporary")) {
        return DCC_JSON_KEY_TEMPORARY;
    }
    if (dcc_json_key_span_eq(key, len, "available")) {
        return DCC_JSON_KEY_AVAILABLE;
    }
    if (dcc_json_key_span_eq(key, len, "mfa_level")) {
        return DCC_JSON_KEY_MFA_LEVEL;
    }
    if (dcc_json_key_span_eq(key, len, "presences")) {
        return DCC_JSON_KEY_PRESENCES;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
