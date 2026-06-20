#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len8(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "guild_id")) {
        return DCC_JSON_KEY_GUILD_ID;
    }
    if (dcc_json_key_span_eq(key, len, "username")) {
        return DCC_JSON_KEY_USERNAME;
    }
    if (dcc_json_key_span_eq(key, len, "suppress")) {
        return DCC_JSON_KEY_SUPPRESS;
    }
    if (dcc_json_key_span_eq(key, len, "endpoint")) {
        return DCC_JSON_KEY_ENDPOINT;
    }
    if (dcc_json_key_span_eq(key, len, "sound_id")) {
        return DCC_JSON_KEY_SOUND_ID;
    }
    if (dcc_json_key_span_eq(key, len, "emoji_id")) {
        return DCC_JSON_KEY_EMOJI_ID;
    }
    if (dcc_json_key_span_eq(key, len, "owner_id")) {
        return DCC_JSON_KEY_OWNER_ID;
    }
    if (dcc_json_key_span_eq(key, len, "max_uses")) {
        return DCC_JSON_KEY_MAX_USES;
    }
    if (dcc_json_key_span_eq(key, len, "role_ids")) {
        return DCC_JSON_KEY_ROLE_IDS;
    }
    if (dcc_json_key_span_eq(key, len, "channels")) {
        return DCC_JSON_KEY_CHANNELS;
    }
    if (dcc_json_key_span_eq(key, len, "messages")) {
        return DCC_JSON_KEY_MESSAGES;
    }
    if (dcc_json_key_span_eq(key, len, "disabled")) {
        return DCC_JSON_KEY_DISABLED;
    }
    if (dcc_json_key_span_eq(key, len, "required")) {
        return DCC_JSON_KEY_REQUIRED;
    }
    if (dcc_json_key_span_eq(key, len, "filename")) {
        return DCC_JSON_KEY_FILENAME;
    }
    if (dcc_json_key_span_eq(key, len, "waveform")) {
        return DCC_JSON_KEY_WAVEFORM;
    }
    if (dcc_json_key_span_eq(key, len, "resolved")) {
        return DCC_JSON_KEY_RESOLVED;
    }
    if (dcc_json_key_span_eq(key, len, "stickers")) {
        return DCC_JSON_KEY_STICKERS;
    }
    if (dcc_json_key_span_eq(key, len, "animated")) {
        return DCC_JSON_KEY_ANIMATED;
    }
    if (dcc_json_key_span_eq(key, len, "position")) {
        return DCC_JSON_KEY_POSITION;
    }
    if (dcc_json_key_span_eq(key, len, "location")) {
        return DCC_JSON_KEY_LOCATION;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
