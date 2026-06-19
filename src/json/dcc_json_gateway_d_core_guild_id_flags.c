#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_mark_d_core_guild_id(dcc_json_gateway_payload_t *out) {
    out->has_message = 1;
    out->has_interaction = 1;
    out->has_voice_state = 1;
    out->has_channel = 1;
    out->has_role = 1;
    out->has_member = 1;
    out->has_scheduled_event = 1;
    out->has_entitlement = 1;
    out->has_invite = 1;
    out->has_stage_instance = 1;
    out->has_soundboard_sound = 1;
    out->has_message_reaction = 1;
    out->has_poll_vote = 1;
    out->has_voice_channel_effect = 1;
    out->has_auto_moderation_action = 1;
    out->has_auto_moderation_rule = 1;
    out->has_message_delete_bulk = 1;
    out->has_thread_member = 1;
    out->has_thread_members_update = 1;
    out->has_guild_members_chunk = 1;
    out->has_channel_pins_update = 1;
    out->has_voice_server_update = 1;
    out->has_integration = 1;
    out->has_presence = 1;
    out->has_application_command_permissions_update = 1;
    out->has_guild_audit_log_entry = 1;
    out->has_guild_ban = 1;
    out->has_guild_id_list_update = 1;
    out->has_guild_update_notice = 1;
    out->has_scheduled_event_user_update = 1;
    out->has_thread_list_sync = 1;
    out->has_voice_channel_start_time_update = 1;
    out->has_voice_channel_status_update = 1;
    out->has_webhooks_update = 1;
    out->has_guild_join_request_update = 1;
    out->has_guild_application_command_update = 1;
    out->has_embedded_activity_update = 1;
}
