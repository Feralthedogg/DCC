#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_len18(const char *key, size_t len) {
    if (dcc_json_key_span_eq(key, len, "heartbeat_interval")) {
        return DCC_JSON_KEY_HEARTBEAT_INTERVAL;
    }
    if (dcc_json_key_span_eq(key, len, "resume_gateway_url")) {
        return DCC_JSON_KEY_RESUME_GATEWAY_URL;
    }
    if (dcc_json_key_span_eq(key, len, "removed_member_ids")) {
        return DCC_JSON_KEY_REMOVED_MEMBER_IDS;
    }
    if (dcc_json_key_span_eq(key, len, "last_pin_timestamp")) {
        return DCC_JSON_KEY_LAST_PIN_TIMESTAMP;
    }
    if (dcc_json_key_span_eq(key, len, "scheduled_end_time")) {
        return DCC_JSON_KEY_SCHEDULED_END_TIME;
    }
    if (dcc_json_key_span_eq(key, len, "current_period_end")) {
        return DCC_JSON_KEY_CURRENT_PERIOD_END;
    }
    if (dcc_json_key_span_eq(key, len, "verification_level")) {
        return DCC_JSON_KEY_VERIFICATION_LEVEL;
    }
    return DCC_JSON_KEY_UNKNOWN;
}
