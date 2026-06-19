#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_apply_d_core_guild_policy_limit(
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    uint32_t value
) {
    switch (key_id) {
        case DCC_JSON_KEY_DEFAULT_MESSAGE_NOTIFICATIONS:
            out->gateway.default_message_notifications = value;
            out->guild.default_message_notifications = value;
            break;
        case DCC_JSON_KEY_PREMIUM_TIER:
            out->gateway.premium_tier = value;
            out->guild.premium_tier = value;
            break;
        case DCC_JSON_KEY_VERIFICATION_LEVEL:
            out->gateway.verification_level = value;
            out->guild.verification_level = value;
            break;
        case DCC_JSON_KEY_EXPLICIT_CONTENT_FILTER:
            out->gateway.explicit_content_filter = value;
            out->guild.explicit_content_filter = value;
            break;
        case DCC_JSON_KEY_MFA_LEVEL:
            out->gateway.mfa_level = value;
            out->guild.mfa_level = value;
            break;
        case DCC_JSON_KEY_NSFW_LEVEL:
            out->gateway.nsfw_level = value;
            out->guild.nsfw_level = value;
            break;
        default:
            break;
    }
}
