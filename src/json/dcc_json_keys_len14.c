#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len14(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "require_colons")) {
        return DCC_JSON_KEY_REQUIRE_COLONS;
    }
    if (dcc_json_key_span_eq(key, len, "application_id")) {
        return DCC_JSON_KEY_APPLICATION_ID;
    }
    if (dcc_json_key_span_eq(key, len, "component_type")) {
        return DCC_JSON_KEY_COMPONENT_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "entitlement_id")) {
        return DCC_JSON_KEY_ENTITLEMENT_ID;
    }
    if (dcc_json_key_span_eq(key, len, "animation_type")) {
        return DCC_JSON_KEY_ANIMATION_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "join_timestamp")) {
        return DCC_JSON_KEY_JOIN_TIMESTAMP;
    }
    if (dcc_json_key_span_eq(key, len, "afk_channel_id")) {
        return DCC_JSON_KEY_AFK_CHANNEL_ID;
    }
    if (dcc_json_key_span_eq(key, len, "widget_enabled")) {
        return DCC_JSON_KEY_WIDGET_ENABLED;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
