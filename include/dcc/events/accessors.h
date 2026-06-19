#ifndef DCC_EVENTS_ACCESSORS_H
#define DCC_EVENTS_ACCESSORS_H

#include <dcc/events/payloads.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_event_type_t dcc_event_type(const dcc_event_t *event);
/*
 * Event names and raw payload pointers are borrowed. They are valid only while
 * the event callback is running unless the documentation for a specific local
 * event says otherwise.
 */
DCC_API const char *dcc_event_name(const dcc_event_t *event);
DCC_API const dcc_raw_event_t *dcc_event_raw(const dcc_event_t *event);
DCC_API const char *dcc_event_type_name(dcc_event_type_t type);
DCC_API uint32_t dcc_event_shard_id(const dcc_event_t *event);
DCC_API void dcc_event_cancel(const dcc_event_t *event);
DCC_API uint8_t dcc_event_is_cancelled(const dcc_event_t *event);
DCC_API const dcc_ready_event_t *dcc_event_ready(const dcc_event_t *event);
DCC_API const dcc_ready_event_t *dcc_event_resumed(const dcc_event_t *event);
DCC_API const dcc_log_event_t *dcc_event_log(const dcc_event_t *event);
DCC_API const dcc_socket_close_event_t *dcc_event_socket_close(const dcc_event_t *event);
DCC_API const dcc_rate_limited_event_t *dcc_event_rate_limited(const dcc_event_t *event);
DCC_API const dcc_message_t *dcc_event_message(const dcc_event_t *event);
DCC_API const dcc_guild_t *dcc_event_guild(const dcc_event_t *event);
DCC_API const dcc_channel_t *dcc_event_channel(const dcc_event_t *event);
DCC_API const dcc_role_t *dcc_event_role(const dcc_event_t *event);
DCC_API const dcc_member_t *dcc_event_member(const dcc_event_t *event);
DCC_API const dcc_scheduled_event_t *dcc_event_scheduled_event(const dcc_event_t *event);
DCC_API const dcc_entitlement_t *dcc_event_entitlement(const dcc_event_t *event);
DCC_API const dcc_subscription_t *dcc_event_subscription(const dcc_event_t *event);
DCC_API const dcc_invite_t *dcc_event_invite(const dcc_event_t *event);
DCC_API const dcc_stage_instance_t *dcc_event_stage_instance(const dcc_event_t *event);
DCC_API const dcc_soundboard_sound_t *dcc_event_soundboard_sound(const dcc_event_t *event);
DCC_API const dcc_message_reaction_t *dcc_event_message_reaction(const dcc_event_t *event);
DCC_API const dcc_poll_vote_t *dcc_event_poll_vote(const dcc_event_t *event);
DCC_API const dcc_voice_channel_effect_t *dcc_event_voice_channel_effect(const dcc_event_t *event);
DCC_API const dcc_auto_moderation_action_t *dcc_event_auto_moderation_action(const dcc_event_t *event);
DCC_API const dcc_auto_moderation_rule_t *dcc_event_auto_moderation_rule(const dcc_event_t *event);
DCC_API const dcc_message_delete_bulk_t *dcc_event_message_delete_bulk(const dcc_event_t *event);
DCC_API const dcc_thread_member_t *dcc_event_thread_member(const dcc_event_t *event);
DCC_API const dcc_thread_members_update_t *dcc_event_thread_members_update(const dcc_event_t *event);
DCC_API const dcc_guild_members_chunk_t *dcc_event_guild_members_chunk(const dcc_event_t *event);
DCC_API const dcc_channel_pins_update_t *dcc_event_channel_pins_update(const dcc_event_t *event);
DCC_API const dcc_typing_start_t *dcc_event_typing_start(const dcc_event_t *event);
DCC_API const dcc_voice_server_update_t *dcc_event_voice_server_update(const dcc_event_t *event);
DCC_API const dcc_integration_t *dcc_event_integration(const dcc_event_t *event);
DCC_API const dcc_presence_t *dcc_event_presence(const dcc_event_t *event);
DCC_API const dcc_user_t *dcc_event_user(const dcc_event_t *event);
DCC_API const dcc_application_command_permissions_update_t *dcc_event_application_command_permissions_update(
    const dcc_event_t *event
);
DCC_API const dcc_guild_audit_log_entry_t *dcc_event_guild_audit_log_entry(const dcc_event_t *event);
DCC_API const dcc_guild_ban_t *dcc_event_guild_ban(const dcc_event_t *event);
DCC_API const dcc_guild_id_list_update_t *dcc_event_guild_id_list_update(const dcc_event_t *event);
DCC_API const dcc_guild_update_notice_t *dcc_event_guild_update_notice(const dcc_event_t *event);
DCC_API const dcc_scheduled_event_user_update_t *dcc_event_scheduled_event_user_update(
    const dcc_event_t *event
);
DCC_API const dcc_thread_list_sync_t *dcc_event_thread_list_sync(const dcc_event_t *event);
DCC_API const dcc_voice_channel_start_time_update_t *dcc_event_voice_channel_start_time_update(
    const dcc_event_t *event
);
DCC_API const dcc_voice_channel_status_update_t *dcc_event_voice_channel_status_update(
    const dcc_event_t *event
);
DCC_API const dcc_voice_buffer_send_event_t *dcc_event_voice_buffer_send(const dcc_event_t *event);
DCC_API const dcc_voice_ready_event_t *dcc_event_voice_ready(const dcc_event_t *event);
DCC_API const dcc_voice_receive_event_t *dcc_event_voice_receive(const dcc_event_t *event);
DCC_API const dcc_voice_track_marker_event_t *dcc_event_voice_track_marker(const dcc_event_t *event);
DCC_API const dcc_voice_client_speaking_event_t *dcc_event_voice_client_speaking(const dcc_event_t *event);
DCC_API const dcc_voice_client_disconnect_event_t *dcc_event_voice_client_disconnect(const dcc_event_t *event);
DCC_API const dcc_voice_client_platform_event_t *dcc_event_voice_client_platform(const dcc_event_t *event);
DCC_API const dcc_webhooks_update_t *dcc_event_webhooks_update(const dcc_event_t *event);
DCC_API const dcc_guild_join_request_update_t *dcc_event_guild_join_request_update(
    const dcc_event_t *event
);
DCC_API const dcc_guild_application_command_update_t *dcc_event_guild_application_command_update(
    const dcc_event_t *event
);
DCC_API const dcc_embedded_activity_update_t *dcc_event_embedded_activity_update(const dcc_event_t *event);
DCC_API const dcc_interaction_t *dcc_event_interaction(const dcc_event_t *event);
DCC_API const dcc_voice_state_t *dcc_event_voice_state(const dcc_event_t *event);
/*
 * Gateway event data is a borrowed view into the gateway parser scratch buffer.
 * It is invalid after the callback returns or after another gateway frame is
 * parsed on the same thread. Use dcc_gateway_event_data_clone() to retain it.
 */
DCC_API const dcc_gateway_event_data_t *dcc_event_data(const dcc_event_t *event);

#ifdef __cplusplus
}
#endif

#endif
