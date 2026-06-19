#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len16(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "voice_start_time")) {
        return DCC_JSON_KEY_VOICE_START_TIME;
    }
    if (dcc_json_key_span_eq(key, len, "edited_timestamp")) {
        return DCC_JSON_KEY_EDITED_TIMESTAMP;
    }
    if (dcc_json_key_span_eq(key, len, "mention_everyone")) {
        return DCC_JSON_KEY_MENTION_EVERYONE;
    }
    if (dcc_json_key_span_eq(key, len, "preferred_locale")) {
        return DCC_JSON_KEY_PREFERRED_LOCALE;
    }
    if (dcc_json_key_span_eq(key, len, "discovery_splash")) {
        return DCC_JSON_KEY_DISCOVERY_SPLASH;
    }
    if (dcc_json_key_span_eq(key, len, "rules_channel_id")) {
        return DCC_JSON_KEY_RULES_CHANNEL_ID;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
