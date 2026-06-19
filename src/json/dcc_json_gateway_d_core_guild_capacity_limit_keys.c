#include "internal/json/dcc_json_internal.h"

int dcc_json_gateway_is_capacity_limit_key(dcc_json_key_id_t key_id) {
    return key_id == DCC_JSON_KEY_AFK_TIMEOUT ||
        key_id == DCC_JSON_KEY_MAX_PRESENCES ||
        key_id == DCC_JSON_KEY_MAX_MEMBERS ||
        key_id == DCC_JSON_KEY_PREMIUM_SUBSCRIPTION_COUNT ||
        key_id == DCC_JSON_KEY_MAX_VIDEO_CHANNEL_USERS;
}

void dcc_json_gateway_apply_capacity_limit(
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    uint32_t value
) {
    switch (key_id) {
        case DCC_JSON_KEY_AFK_TIMEOUT:
            out->gateway.afk_timeout = value;
            out->guild.afk_timeout = value;
            break;
        case DCC_JSON_KEY_MAX_PRESENCES:
            out->gateway.max_presences = value;
            out->guild.max_presences = value;
            break;
        case DCC_JSON_KEY_MAX_MEMBERS:
            out->gateway.max_members = value;
            out->guild.max_members = value;
            break;
        case DCC_JSON_KEY_PREMIUM_SUBSCRIPTION_COUNT:
            out->gateway.premium_subscription_count = value;
            out->guild.premium_subscription_count = value;
            break;
        case DCC_JSON_KEY_MAX_VIDEO_CHANNEL_USERS:
            out->gateway.max_video_channel_users = value;
            out->guild.max_video_channel_users = value;
            break;
        default:
            break;
    }
}
