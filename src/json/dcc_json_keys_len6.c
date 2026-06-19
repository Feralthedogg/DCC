#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len6(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "values")) {
        return DCC_JSON_KEY_VALUES;
    }
    if (dcc_json_key_span_eq(key, len, "author")) {
        return DCC_JSON_KEY_AUTHOR;
    }
    if (dcc_json_key_span_eq(key, len, "pinned")) {
        return DCC_JSON_KEY_PINNED;
    }
    if (dcc_json_key_span_eq(key, len, "status")) {
        return DCC_JSON_KEY_STATUS;
    }
    if (dcc_json_key_span_eq(key, len, "mobile")) {
        return DCC_JSON_KEY_MOBILE;
    }
    if (dcc_json_key_span_eq(key, len, "height")) {
        return DCC_JSON_KEY_HEIGHT;
    }
    if (dcc_json_key_span_eq(key, len, "member")) {
        return DCC_JSON_KEY_MEMBER;
    }
    if (dcc_json_key_span_eq(key, len, "emojis")) {
        return DCC_JSON_KEY_EMOJIS;
    }
    if (dcc_json_key_span_eq(key, len, "guilds")) {
        return DCC_JSON_KEY_GUILDS;
    }
    if (dcc_json_key_span_eq(key, len, "sku_id")) {
        return DCC_JSON_KEY_SKU_ID;
    }
    if (dcc_json_key_span_eq(key, len, "opcode")) {
        return DCC_JSON_KEY_OPCODE;
    }
    if (dcc_json_key_span_eq(key, len, "reason")) {
        return DCC_JSON_KEY_REASON;
    }
    if (dcc_json_key_span_eq(key, len, "avatar")) {
        return DCC_JSON_KEY_AVATAR;
    }
    if (dcc_json_key_span_eq(key, len, "banner")) {
        return DCC_JSON_KEY_BANNER;
    }
    if (dcc_json_key_span_eq(key, len, "splash")) {
        return DCC_JSON_KEY_SPLASH;
    }
    if (dcc_json_key_span_eq(key, len, "volume")) {
        return DCC_JSON_KEY_VOLUME;
    }
    if (dcc_json_key_span_eq(key, len, "action")) {
        return DCC_JSON_KEY_ACTION;
    }
    if (dcc_json_key_span_eq(key, len, "locale")) {
        return DCC_JSON_KEY_LOCALE;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
