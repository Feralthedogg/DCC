#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len12(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "guild_locale")) {
        return DCC_JSON_KEY_GUILD_LOCALE;
    }
    if (dcc_json_key_span_eq(key, len, "accent_color")) {
        return DCC_JSON_KEY_ACCENT_COLOR;
    }
    if (dcc_json_key_span_eq(key, len, "entitlements")) {
        return DCC_JSON_KEY_ENTITLEMENTS;
    }
    if (dcc_json_key_span_eq(key, len, "member_count")) {
        return DCC_JSON_KEY_MEMBER_COUNT;
    }
    if (dcc_json_key_span_eq(key, len, "public_flags")) {
        return DCC_JSON_KEY_PUBLIC_FLAGS;
    }
    if (dcc_json_key_span_eq(key, len, "trigger_type")) {
        return DCC_JSON_KEY_TRIGGER_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "sound_volume")) {
        return DCC_JSON_KEY_VOLUME;
    }
    if (dcc_json_key_span_eq(key, len, "content_type")) {
        return DCC_JSON_KEY_CONTENT_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "exempt_roles")) {
        return DCC_JSON_KEY_EXEMPT_ROLES;
    }
    if (dcc_json_key_span_eq(key, len, "promotion_id")) {
        return DCC_JSON_KEY_PROMOTION_ID;
    }
    if (dcc_json_key_span_eq(key, len, "animation_id")) {
        return DCC_JSON_KEY_ANIMATION_ID;
    }
    if (dcc_json_key_span_eq(key, len, "premium_tier")) {
        return DCC_JSON_KEY_PREMIUM_TIER;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
