#ifndef DCC_GATEWAY_SMOKE_STATE_H
#define DCC_GATEWAY_SMOKE_STATE_H

#if !defined(_WIN32)

#include <dcc/dcc.h>

#include <stdint.h>

typedef struct gateway_state {
    int ready_seen;
    int resumed_seen;
    int message_create_seen;
    int message_update_seen;
    int message_delete_seen;
    int guild_create_seen;
    int guild_update_seen;
    int guild_delete_seen;
    int interaction_seen;
    int voice_state_seen;
    int voice_connect_seen;
    int voice_auto_ready_seen;
    int voice_auto_disconnected_seen;
    int channel_create_seen;
    int channel_update_seen;
    int channel_delete_seen;
    int channel_info_seen;
    int thread_create_seen;
    int thread_update_seen;
    int thread_delete_seen;
    int channel_topic_update_seen;
    int subscription_create_seen;
    int raw_subscription_seen;
    int reaction_add_seen;
    int reaction_remove_seen;
    int reaction_remove_all_seen;
    int reaction_remove_emoji_seen;
    int typing_start_seen;
    int bulk_delete_seen;
    int thread_members_seen;
    int guild_member_add_seen;
    int guild_member_remove_seen;
    int guild_role_create_seen;
    int user_update_seen;
    int automod_action_seen;
    int automod_rule_seen;
    int automod_rule_update_seen;
    int automod_rule_delete_seen;
    int poll_vote_seen;
    int poll_vote_remove_seen;
    int invite_create_seen;
    int invite_delete_seen;
    int voice_effect_seen;
    int scheduled_event_seen;
    int scheduled_event_update_seen;
    int scheduled_event_delete_seen;
    int guild_member_update_seen;
    int members_chunk_seen;
    int channel_pins_seen;
    int stage_instance_seen;
    int stage_instance_update_seen;
    int stage_instance_delete_seen;
    int soundboard_sound_seen;
    int soundboard_sound_update_seen;
    int soundboard_sound_delete_seen;
    int entitlement_create_seen;
    int entitlement_update_seen;
    int entitlement_delete_seen;
    int subscription_update_seen;
    int subscription_delete_seen;
    int voice_server_update_seen;
    int integration_create_seen;
    int integration_update_seen;
    int integration_delete_seen;
    int presence_update_seen;
    int thread_member_update_seen;
    int emojis_update_seen;
    int thread_list_sync_seen;
    int soundboard_sounds_update_seen;
    int guild_role_update_seen;
    int guild_role_delete_seen;
    int application_command_permissions_update_seen;
    int audit_log_entry_seen;
    int guild_ban_add_seen;
    int guild_ban_remove_seen;
    int guild_stickers_update_seen;
    int guild_integrations_update_seen;
    int scheduled_event_user_add_seen;
    int scheduled_event_user_remove_seen;
    int soundboard_sounds_seen;
    int voice_channel_start_time_update_seen;
    int voice_channel_status_update_seen;
    int webhooks_update_seen;
    int guild_join_request_delete_seen;
    int guild_join_request_update_seen;
    int guild_application_command_counts_seen;
    int guild_application_command_index_seen;
    int embedded_activity_update_seen;
    int rate_limited_seen;
    int log_seen;
    int slash_command_seen;
    int button_click_seen;
    int autocomplete_seen;
    int select_click_seen;
    int message_context_menu_seen;
    int user_context_menu_seen;
    int form_submit_seen;
    int named_slash_command_seen;
    int named_button_click_seen;
    int named_autocomplete_seen;
    int named_select_click_seen;
    int named_message_context_menu_seen;
    int named_user_context_menu_seen;
    int named_form_submit_seen;
    int named_interaction_unexpected_seen;
    int named_interaction_off_seen;
    int socket_close_seen;
    uint64_t raw_seen_low;
    uint64_t raw_seen_high;
    uint64_t typed_seen_low;
    uint64_t typed_seen_high;
    uint64_t typed_accessor_seen_low;
    uint64_t typed_accessor_seen_high;
    dcc_voice_client_t *voice_client;
    char session_id[64];
    char last_log[256];
} gateway_state_t;

void gateway_state_mark_raw_event(gateway_state_t *state, dcc_event_type_t type);
int gateway_state_raw_event_seen(const gateway_state_t *state, dcc_event_type_t type);
int gateway_state_required_raw_events_seen(const gateway_state_t *state);

void gateway_state_mark_typed_event(gateway_state_t *state, dcc_event_type_t type);
int gateway_state_typed_event_seen(const gateway_state_t *state, dcc_event_type_t type);
int gateway_state_required_typed_events_seen(const gateway_state_t *state);

void gateway_state_mark_typed_accessor_event(gateway_state_t *state, dcc_event_type_t type);
int gateway_state_typed_accessor_event_seen(const gateway_state_t *state, dcc_event_type_t type);
int gateway_state_required_typed_accessors_seen(const gateway_state_t *state);

int gateway_smoke_event_is_not_gateway_dispatch(dcc_event_type_t type);
int gateway_smoke_event_is_interaction_subtype(dcc_event_type_t type);
const char *gateway_smoke_expected_dispatch_name(dcc_event_type_t type);
int gateway_smoke_typed_metadata_ok(const dcc_event_t *event, dcc_event_type_t type);

#endif

#endif
