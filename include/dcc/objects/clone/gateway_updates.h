#ifndef DCC_OBJECTS_CLONE_GATEWAY_UPDATES_H
#define DCC_OBJECTS_CLONE_GATEWAY_UPDATES_H

#include <dcc/objects/gateway_updates.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_message_reaction_t *dcc_message_reaction_clone(const dcc_message_reaction_t *reaction);
DCC_API void dcc_message_reaction_free(dcc_message_reaction_t *reaction);
DCC_API dcc_poll_vote_t *dcc_poll_vote_clone(const dcc_poll_vote_t *poll_vote);
DCC_API void dcc_poll_vote_free(dcc_poll_vote_t *poll_vote);
DCC_API dcc_voice_channel_effect_t *dcc_voice_channel_effect_clone(const dcc_voice_channel_effect_t *effect);
DCC_API void dcc_voice_channel_effect_free(dcc_voice_channel_effect_t *effect);
DCC_API dcc_auto_moderation_action_t *dcc_auto_moderation_action_clone(const dcc_auto_moderation_action_t *action);
DCC_API void dcc_auto_moderation_action_free(dcc_auto_moderation_action_t *action);
DCC_API dcc_auto_moderation_rule_t *dcc_auto_moderation_rule_clone(const dcc_auto_moderation_rule_t *rule);
DCC_API void dcc_auto_moderation_rule_free(dcc_auto_moderation_rule_t *rule);
DCC_API dcc_message_delete_bulk_t *dcc_message_delete_bulk_clone(const dcc_message_delete_bulk_t *bulk_delete);
DCC_API void dcc_message_delete_bulk_free(dcc_message_delete_bulk_t *bulk_delete);
DCC_API dcc_thread_member_t *dcc_thread_member_clone(const dcc_thread_member_t *thread_member);
DCC_API void dcc_thread_member_free(dcc_thread_member_t *thread_member);
DCC_API dcc_thread_members_update_t *dcc_thread_members_update_clone(
    const dcc_thread_members_update_t *thread_members
);
DCC_API void dcc_thread_members_update_free(dcc_thread_members_update_t *thread_members);
DCC_API dcc_guild_members_chunk_t *dcc_guild_members_chunk_clone(const dcc_guild_members_chunk_t *chunk);
DCC_API void dcc_guild_members_chunk_free(dcc_guild_members_chunk_t *chunk);
DCC_API dcc_channel_pins_update_t *dcc_channel_pins_update_clone(const dcc_channel_pins_update_t *pins);
DCC_API void dcc_channel_pins_update_free(dcc_channel_pins_update_t *pins);
DCC_API dcc_typing_start_t *dcc_typing_start_clone(const dcc_typing_start_t *typing_start);
DCC_API void dcc_typing_start_free(dcc_typing_start_t *typing_start);
DCC_API dcc_voice_server_update_t *dcc_voice_server_update_clone(const dcc_voice_server_update_t *voice_server);
DCC_API void dcc_voice_server_update_free(dcc_voice_server_update_t *voice_server);
DCC_API dcc_integration_t *dcc_integration_clone(const dcc_integration_t *integration);
DCC_API void dcc_integration_free(dcc_integration_t *integration);
DCC_API dcc_presence_t *dcc_presence_clone(const dcc_presence_t *presence);
DCC_API void dcc_presence_free(dcc_presence_t *presence);
DCC_API dcc_application_command_permission_t *dcc_application_command_permission_clone(
    const dcc_application_command_permission_t *permission
);
DCC_API void dcc_application_command_permission_free(dcc_application_command_permission_t *permission);
DCC_API dcc_application_command_permissions_update_t *dcc_application_command_permissions_update_clone(
    const dcc_application_command_permissions_update_t *update
);
DCC_API void dcc_application_command_permissions_update_free(
    dcc_application_command_permissions_update_t *update
);
DCC_API dcc_guild_audit_log_entry_t *dcc_guild_audit_log_entry_clone(
    const dcc_guild_audit_log_entry_t *entry
);
DCC_API void dcc_guild_audit_log_entry_free(dcc_guild_audit_log_entry_t *entry);
DCC_API dcc_guild_ban_t *dcc_guild_ban_clone(const dcc_guild_ban_t *ban);
DCC_API void dcc_guild_ban_free(dcc_guild_ban_t *ban);
DCC_API dcc_guild_id_list_update_t *dcc_guild_id_list_update_clone(
    const dcc_guild_id_list_update_t *update
);
DCC_API void dcc_guild_id_list_update_free(dcc_guild_id_list_update_t *update);
DCC_API dcc_guild_update_notice_t *dcc_guild_update_notice_clone(
    const dcc_guild_update_notice_t *notice
);
DCC_API void dcc_guild_update_notice_free(dcc_guild_update_notice_t *notice);
DCC_API dcc_scheduled_event_user_update_t *dcc_scheduled_event_user_update_clone(
    const dcc_scheduled_event_user_update_t *update
);
DCC_API void dcc_scheduled_event_user_update_free(dcc_scheduled_event_user_update_t *update);
DCC_API dcc_thread_list_sync_t *dcc_thread_list_sync_clone(const dcc_thread_list_sync_t *sync);
DCC_API void dcc_thread_list_sync_free(dcc_thread_list_sync_t *sync);
DCC_API dcc_voice_channel_start_time_update_t *dcc_voice_channel_start_time_update_clone(
    const dcc_voice_channel_start_time_update_t *update
);
DCC_API void dcc_voice_channel_start_time_update_free(dcc_voice_channel_start_time_update_t *update);
DCC_API dcc_voice_channel_status_update_t *dcc_voice_channel_status_update_clone(
    const dcc_voice_channel_status_update_t *update
);
DCC_API void dcc_voice_channel_status_update_free(dcc_voice_channel_status_update_t *update);
DCC_API dcc_webhooks_update_t *dcc_webhooks_update_clone(const dcc_webhooks_update_t *update);
DCC_API void dcc_webhooks_update_free(dcc_webhooks_update_t *update);
DCC_API dcc_guild_join_request_update_t *dcc_guild_join_request_update_clone(
    const dcc_guild_join_request_update_t *update
);
DCC_API void dcc_guild_join_request_update_free(dcc_guild_join_request_update_t *update);
DCC_API dcc_guild_application_command_update_t *dcc_guild_application_command_update_clone(
    const dcc_guild_application_command_update_t *update
);
DCC_API void dcc_guild_application_command_update_free(dcc_guild_application_command_update_t *update);
DCC_API dcc_embedded_activity_update_t *dcc_embedded_activity_update_clone(
    const dcc_embedded_activity_update_t *update
);
DCC_API void dcc_embedded_activity_update_free(dcc_embedded_activity_update_t *update);

#ifdef __cplusplus
}
#endif

#endif
