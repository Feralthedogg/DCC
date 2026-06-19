#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len17(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "rule_trigger_type")) {
        return DCC_JSON_KEY_RULE_TRIGGER_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "message_author_id")) {
        return DCC_JSON_KEY_MESSAGE_AUTHOR_ID;
    }
    if (dcc_json_key_span_eq(key, len, "soundboard_sounds")) {
        return DCC_JSON_KEY_SOUNDBOARD_SOUNDS;
    }
    if (dcc_json_key_span_eq(key, len, "system_channel_id")) {
        return DCC_JSON_KEY_SYSTEM_CHANNEL_ID;
    }
    if (dcc_json_key_span_eq(key, len, "widget_channel_id")) {
        return DCC_JSON_KEY_WIDGET_CHANNEL_ID;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
