#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_primary_user_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_USER_ID) {
        *handled = 0;
        return DCC_OK;
    }

    uint64_t value = 0;
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }

    out->voice_state.user_id = value;
    out->gateway.user_id = value;
    out->gateway.user.id = value;
    out->member.user.id = value;
    out->entitlement.user_id = value;
    out->subscription.user_id = value;
    out->invite.inviter.id = value;
    out->soundboard_sound.user_id = value;
    out->soundboard_sound.user.id = value;
    out->message_reaction.user_id = value;
    out->poll_vote.user_id = value;
    out->voice_channel_effect.user_id = value;
    out->auto_moderation_action.user_id = value;
    out->thread_member.user_id = value;
    out->presence.user.id = value;
    out->user.id = value;
    out->guild_audit_log_entry.user_id = value;
    out->guild_ban.user.id = value;
    out->scheduled_event_user_update.user_id = value;
    out->guild_join_request_update.user_id = value;
    out->has_voice_state = 1;
    out->has_member = 1;
    out->has_entitlement = 1;
    out->has_subscription = 1;
    out->has_invite = 1;
    out->has_soundboard_sound = 1;
    out->has_message_reaction = 1;
    out->has_poll_vote = 1;
    out->has_voice_channel_effect = 1;
    out->has_auto_moderation_action = 1;
    out->has_thread_member = 1;
    out->has_presence = 1;
    out->has_user = 1;
    out->has_guild_audit_log_entry = 1;
    out->has_guild_ban = 1;
    out->has_scheduled_event_user_update = 1;
    out->has_guild_join_request_update = 1;
    (void)has_value;
    *handled = 1;
    return DCC_OK;
}
