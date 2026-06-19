#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len11(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "description")) {
        return DCC_JSON_KEY_DESCRIPTION;
    }
    if (dcc_json_key_span_eq(key, len, "attachments")) {
        return DCC_JSON_KEY_ATTACHMENTS;
    }
    if (dcc_json_key_span_eq(key, len, "format_type")) {
        return DCC_JSON_KEY_FORMAT_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "global_name")) {
        return DCC_JSON_KEY_GLOBAL_NAME;
    }
    if (dcc_json_key_span_eq(key, len, "channel_ids")) {
        return DCC_JSON_KEY_CHANNEL_IDS;
    }
    if (dcc_json_key_span_eq(key, len, "action_type")) {
        return DCC_JSON_KEY_ACTION_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "unavailable")) {
        return DCC_JSON_KEY_UNAVAILABLE;
    }
    if (dcc_json_key_span_eq(key, len, "self_stream")) {
        return DCC_JSON_KEY_SELF_STREAM;
    }
    if (dcc_json_key_span_eq(key, len, "chunk_index")) {
        return DCC_JSON_KEY_CHUNK_INDEX;
    }
    if (dcc_json_key_span_eq(key, len, "chunk_count")) {
        return DCC_JSON_KEY_CHUNK_COUNT;
    }
    if (dcc_json_key_span_eq(key, len, "retry_after")) {
        return DCC_JSON_KEY_RETRY_AFTER;
    }
    if (dcc_json_key_span_eq(key, len, "target_type")) {
        return DCC_JSON_KEY_TARGET_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "canceled_at")) {
        return DCC_JSON_KEY_CANCELED_AT;
    }
    if (dcc_json_key_span_eq(key, len, "channel_type")) {
        return DCC_JSON_KEY_CHANNEL_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "entity_type")) {
        return DCC_JSON_KEY_ENTITY_TYPE;
    }
    if (dcc_json_key_span_eq(key, len, "permissions")) {
        return DCC_JSON_KEY_PERMISSIONS;
    }
    if (dcc_json_key_span_eq(key, len, "mentionable")) {
        return DCC_JSON_KEY_MENTIONABLE;
    }
    if (dcc_json_key_span_eq(key, len, "afk_timeout")) {
        return DCC_JSON_KEY_AFK_TIMEOUT;
    }
    if (dcc_json_key_span_eq(key, len, "max_members")) {
        return DCC_JSON_KEY_MAX_MEMBERS;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
