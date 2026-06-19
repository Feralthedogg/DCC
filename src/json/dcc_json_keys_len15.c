#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len15(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "app_permissions")) {
        return DCC_JSON_KEY_APP_PERMISSIONS;
    }
    if (dcc_json_key_span_eq(key, len, "matched_keyword")) {
        return DCC_JSON_KEY_MATCHED_KEYWORD;
    }
    if (dcc_json_key_span_eq(key, len, "matched_content")) {
        return DCC_JSON_KEY_MATCHED_CONTENT;
    }
    if (dcc_json_key_span_eq(key, len, "subscription_id")) {
        return DCC_JSON_KEY_SUBSCRIPTION_ID;
    }
    if (dcc_json_key_span_eq(key, len, "gift_code_flags")) {
        return DCC_JSON_KEY_GIFT_CODE_FLAGS;
    }
    if (dcc_json_key_span_eq(key, len, "entitlement_ids")) {
        return DCC_JSON_KEY_ENTITLEMENT_IDS;
    }
    if (dcc_json_key_span_eq(key, len, "exempt_channels")) {
        return DCC_JSON_KEY_EXEMPT_CHANNELS;
    }
    if (dcc_json_key_span_eq(key, len, "last_message_id")) {
        return DCC_JSON_KEY_LAST_MESSAGE_ID;
    }
    if (dcc_json_key_span_eq(key, len, "entity_metadata")) {
        return DCC_JSON_KEY_ENTITY_METADATA;
    }
    if (dcc_json_key_span_eq(key, len, "stage_instances")) {
        return DCC_JSON_KEY_STAGE_INSTANCES;
    }
    if (dcc_json_key_span_eq(key, len, "vanity_url_code")) {
        return DCC_JSON_KEY_VANITY_URL_CODE;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
