#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_apply_d_core_guild_id(
    dcc_json_gateway_payload_t *out,
    uint64_t value
) {
    out->message.guild_id = value;
    out->interaction.guild_id = value;
    out->voice_state.guild_id = value;
    out->channel.guild_id = value;
    out->role.guild_id = value;
    out->member.guild_id = value;
    out->scheduled_event.guild_id = value;
    out->entitlement.guild_id = value;
    out->invite.guild_id = value;
    out->stage_instance.guild_id = value;
    out->soundboard_sound.guild_id = value;
    out->message_reaction.guild_id = value;
    out->poll_vote.guild_id = value;
    out->voice_channel_effect.guild_id = value;
    out->auto_moderation_action.guild_id = value;
    out->auto_moderation_rule.guild_id = value;
    out->message_delete_bulk.guild_id = value;
    out->thread_member.guild_id = value;
    out->thread_members_update.guild_id = value;
    out->guild_members_chunk.guild_id = value;
    out->channel_pins_update.guild_id = value;
    out->voice_server_update.guild_id = value;
    out->integration.guild_id = value;
    out->presence.guild_id = value;
    out->application_command_permissions_update.guild_id = value;
    out->guild_audit_log_entry.guild_id = value;
    out->guild_ban.guild_id = value;
    out->guild_id_list_update.guild_id = value;
    out->guild_update_notice.guild_id = value;
    out->scheduled_event_user_update.guild_id = value;
    out->thread_list_sync.guild_id = value;
    out->voice_channel_start_time_update.guild_id = value;
    out->voice_channel_status_update.guild_id = value;
    out->webhooks_update.guild_id = value;
    out->guild_join_request_update.guild_id = value;
    out->guild_application_command_update.guild_id = value;
    out->embedded_activity_update.guild_id = value;
    out->gateway.guild_id = value;
}
