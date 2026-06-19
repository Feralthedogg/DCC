#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_apply_d_bool_scalar_key(
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int value,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out == NULL) {
        return DCC_ERR_JSON;
    }

    unsigned int bit = value ? 1U : 0U;
    switch (key_id) {
        case DCC_JSON_KEY_AVAILABLE:
            out->gateway.available = bit;
            out->soundboard_sound.available = bit;
            out->has_soundboard_sound = 1;
            break;
        case DCC_JSON_KEY_NSFW:
            out->channel.nsfw = bit;
            out->has_channel = 1;
            out->cache_fields |= DCC_CACHE_FIELD_CHANNEL_NSFW;
            break;
        case DCC_JSON_KEY_ENABLED:
            out->gateway.enabled = bit;
            out->auto_moderation_rule.enabled = bit;
            out->integration.enabled = bit;
            out->has_auto_moderation_rule = 1;
            out->has_integration = 1;
            break;
        case DCC_JSON_KEY_DISCOVERABLE_DISABLED:
            out->gateway.discoverable_disabled = bit;
            out->stage_instance.discoverable_disabled = bit;
            out->has_stage_instance = 1;
            break;
        case DCC_JSON_KEY_PENDING:
            out->gateway.pending = bit;
            out->member.pending = bit;
            out->has_member = 1;
            out->cache_fields |= DCC_CACHE_FIELD_MEMBER_PENDING;
            break;
        case DCC_JSON_KEY_TEMPORARY:
            out->gateway.temporary = bit;
            out->invite.temporary = bit;
            out->has_invite = 1;
            break;
        case DCC_JSON_KEY_BURST:
            out->gateway.burst = bit;
            out->message_reaction.burst = bit;
            out->has_message_reaction = 1;
            break;
        case DCC_JSON_KEY_NEWLY_CREATED:
            out->gateway.newly_created = bit;
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
