#ifndef DCC_GATEWAY_SMOKE_HANDLERS_H
#define DCC_GATEWAY_SMOKE_HANDLERS_H

#include <dcc/dcc.h>

#include "gateway_smoke_state.h"

#if !defined(_WIN32)

void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_resumed(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_typed_dispatch(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_message_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_message_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_message_delete(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_delete(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_interaction_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_interaction_subtype(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_named_interaction_route(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_voice_state_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_channel_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_subscription_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_raw_dispatch(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_message_reaction_add(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_typing_start(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_message_delete_bulk(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_thread_members_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_member_add(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_role_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_user_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_automod_action(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_automod_rule_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_poll_vote_add(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_invite_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_voice_channel_effect_send(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_scheduled_event_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_member_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_members_chunk(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_channel_pins_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_stage_instance_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_soundboard_sound_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_entitlement_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_voice_server_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_integration_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_presence_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_thread_member_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_emojis_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_thread_list_sync(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_soundboard_sounds_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_application_command_permissions_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_audit_log_entry_create(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_ban(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_stickers_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_integrations_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_scheduled_event_user_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_soundboard_sounds(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_voice_channel_start_time_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_voice_channel_status_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_webhooks_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_join_request_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_guild_application_command_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_embedded_activity_update(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_log(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void on_socket_close(dcc_client_t *client, const dcc_event_t *event, void *user_data);

#endif

#endif
