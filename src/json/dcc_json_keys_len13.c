#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len13(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "discriminator")) {
        return DCC_JSON_KEY_DISCRIMINATOR;
    }
    if (dcc_json_key_span_eq(key, len, "channel_types")) {
        return DCC_JSON_KEY_CHANNEL_TYPES;
    }
    if (dcc_json_key_span_eq(key, len, "privacy_level")) {
        return DCC_JSON_KEY_PRIVACY_LEVEL;
    }
    if (dcc_json_key_span_eq(key, len, "premium_since")) {
        return DCC_JSON_KEY_PREMIUM_SINCE;
    }
    if (dcc_json_key_span_eq(key, len, "client_status")) {
        return DCC_JSON_KEY_CLIENT_STATUS;
    }
    if (dcc_json_key_span_eq(key, len, "duration_secs")) {
        return DCC_JSON_KEY_DURATION_SECS;
    }
    if (dcc_json_key_span_eq(key, len, "added_members")) {
        return DCC_JSON_KEY_ADDED_MEMBERS;
    }
    if (dcc_json_key_span_eq(key, len, "newly_created")) {
        return DCC_JSON_KEY_NEWLY_CREATED;
    }
    if (dcc_json_key_span_eq(key, len, "max_presences")) {
        return DCC_JSON_KEY_MAX_PRESENCES;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
