#include "internal/json/dcc_json_internal.h"

dcc_json_key_id_t dcc_json_gateway_key_id_extended_tail(const char *key, size_t len) {
    switch (len) {
        case 19:
            return dcc_json_key_span_eq(key, len, "rate_limit_per_user")
                ? DCC_JSON_KEY_RATE_LIMIT_PER_USER
                : DCC_JSON_KEY_UNKNOWN;
        case 20:
            if (dcc_json_key_span_eq(key, len, "scheduled_start_time")) {
                return DCC_JSON_KEY_SCHEDULED_START_TIME;
            }
            if (dcc_json_key_span_eq(key, len, "current_period_start")) {
                return DCC_JSON_KEY_CURRENT_PERIOD_START;
            }
            return DCC_JSON_KEY_UNKNOWN;
        case 21:
            if (dcc_json_key_span_eq(key, len, "discoverable_disabled")) {
                return DCC_JSON_KEY_DISCOVERABLE_DISABLED;
            }
            if (dcc_json_key_span_eq(key, len, "attachment_size_limit")) {
                return DCC_JSON_KEY_ATTACHMENT_SIZE_LIMIT;
            }
            return DCC_JSON_KEY_UNKNOWN;
        case 22:
            return dcc_json_key_span_eq(key, len, "guild_scheduled_events")
                ? DCC_JSON_KEY_GUILD_SCHEDULED_EVENTS
                : DCC_JSON_KEY_UNKNOWN;
        case 23:
            if (dcc_json_key_span_eq(key, len, "alert_system_message_id")) {
                return DCC_JSON_KEY_ALERT_SYSTEM_MESSAGE_ID;
            }
            if (dcc_json_key_span_eq(key, len, "explicit_content_filter")) {
                return DCC_JSON_KEY_EXPLICIT_CONTENT_FILTER;
            }
            if (dcc_json_key_span_eq(key, len, "max_video_channel_users")) {
                return DCC_JSON_KEY_MAX_VIDEO_CHANNEL_USERS;
            }
            return DCC_JSON_KEY_UNKNOWN;
        case 24:
            if (dcc_json_key_span_eq(key, len, "guild_scheduled_event_id")) {
                return DCC_JSON_KEY_GUILD_SCHEDULED_EVENT_ID;
            }
            if (dcc_json_key_span_eq(key, len, "safety_alerts_channel_id")) {
                return DCC_JSON_KEY_SAFETY_ALERTS_CHANNEL_ID;
            }
            return DCC_JSON_KEY_UNKNOWN;
        case 25:
            return dcc_json_key_span_eq(key, len, "public_updates_channel_id")
                ? DCC_JSON_KEY_PUBLIC_UPDATES_CHANNEL_ID
                : DCC_JSON_KEY_UNKNOWN;
        case 26:
            if (dcc_json_key_span_eq(key, len, "request_to_speak_timestamp")) {
                return DCC_JSON_KEY_REQUEST_TO_SPEAK_TIMESTAMP;
            }
            if (dcc_json_key_span_eq(key, len, "premium_subscription_count")) {
                return DCC_JSON_KEY_PREMIUM_SUBSCRIPTION_COUNT;
            }
            return DCC_JSON_KEY_UNKNOWN;
        case 28:
            if (dcc_json_key_span_eq(key, len, "communication_disabled_until")) {
                return DCC_JSON_KEY_COMMUNICATION_DISABLED_UNTIL;
            }
            if (dcc_json_key_span_eq(key, len, "premium_progress_bar_enabled")) {
                return DCC_JSON_KEY_PREMIUM_PROGRESS_BAR_ENABLED;
            }
            return DCC_JSON_KEY_UNKNOWN;
        case 29:
            return dcc_json_key_span_eq(key, len, "default_message_notifications")
                ? DCC_JSON_KEY_DEFAULT_MESSAGE_NOTIFICATIONS
                : DCC_JSON_KEY_UNKNOWN;
        case 30:
            return dcc_json_key_span_eq(key, len, "authorizing_integration_owners")
                ? DCC_JSON_KEY_AUTHORIZING_INTEGRATION_OWNERS
                : DCC_JSON_KEY_UNKNOWN;
        default:
            return DCC_JSON_KEY_UNKNOWN;
    }
}
