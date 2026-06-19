#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len10(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "session_id")) {
        return DCC_JSON_KEY_SESSION_ID;
    }
    if (dcc_json_key_span_eq(key, len, "channel_id")) {
        return DCC_JSON_KEY_CHANNEL_ID;
    }
    if (dcc_json_key_span_eq(key, len, "self_video")) {
        return DCC_JSON_KEY_SELF_VIDEO;
    }
    if (dcc_json_key_span_eq(key, len, "message_id")) {
        return DCC_JSON_KEY_MESSAGE_ID;
    }
    if (dcc_json_key_span_eq(key, len, "webhook_id")) {
        return DCC_JSON_KEY_WEBHOOK_ID;
    }
    if (dcc_json_key_span_eq(key, len, "user_count")) {
        return DCC_JSON_KEY_USER_COUNT;
    }
    if (dcc_json_key_span_eq(key, len, "created_at")) {
        return DCC_JSON_KEY_CREATED_AT;
    }
    if (dcc_json_key_span_eq(key, len, "expires_at")) {
        return DCC_JSON_KEY_EXPIRES_AT;
    }
    if (dcc_json_key_span_eq(key, len, "creator_id")) {
        return DCC_JSON_KEY_CREATOR_ID;
    }
    if (dcc_json_key_span_eq(key, len, "event_type")) {
        return DCC_JSON_KEY_EVENT_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "emoji_name")) {
        return DCC_JSON_KEY_EMOJI_NAME;
    }
    if (dcc_json_key_span_eq(key, len, "activities")) {
        return DCC_JSON_KEY_ACTIVITIES;
    }
    if (dcc_json_key_span_eq(key, len, "user_limit")) {
        return DCC_JSON_KEY_USER_LIMIT;
    }
    if (dcc_json_key_span_eq(key, len, "nsfw_level")) {
        return DCC_JSON_KEY_NSFW_LEVEL;
    }
    if (dcc_json_key_span_eq(key, len, "sort_value")) {
        return DCC_JSON_KEY_SORT_VALUE;
    }
    if (dcc_json_key_span_eq(key, len, "permission")) {
        return DCC_JSON_KEY_PERMISSION;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
