/**
 * @file tests/support/http_smoke_support.h
 * @brief Test declarations for HTTP smoke.
 */

#ifndef DCC_TEST_HTTP_SMOKE_SUPPORT_H
#define DCC_TEST_HTTP_SMOKE_SUPPORT_H

#include "http_smoke_server.h"

#include <dcc/dcc.h>

#if !defined(_WIN32)
#include <pthread.h>
#endif

#include <stddef.h>
#include <stdint.h>

#if !defined(_WIN32)

typedef struct rest_seen {
    int called;
    int rate_limited_called;
    uint16_t status;
    dcc_status_t error;
    double retry_after;
    uint8_t global;
    size_t body_len;
    char method[8];
    char path[128];
    char body[64];
} rest_seen_t;

typedef struct rest_async_seen {
    int called;
    uint16_t statuses[2];
    dcc_status_t errors[2];
    char bodies[2][64];
} rest_async_seen_t;

typedef struct rest_async_order_state {
    int called;
    int expected;
    int labels[4];
    uint16_t statuses[4];
    dcc_status_t errors[4];
} rest_async_order_state_t;

typedef struct rest_async_order_item {
    rest_async_order_state_t *state;
    int label;
} rest_async_order_item_t;

typedef struct client_wait_thread_state {
    dcc_client_t *client;
    dcc_status_t status;
} client_wait_thread_state_t;

typedef struct event_wait_thread_state {
    dcc_client_t *client;
    dcc_event_type_t type;
    uint32_t timeout_ms;
    dcc_event_snapshot_t snapshot;
    dcc_status_t status;
} event_wait_thread_state_t;

typedef dcc_status_t (*rest_wrapper_call_fn)(dcc_client_t *client, dcc_rest_cb cb, void *user_data);

void rest_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data);
void rate_limited_cb(dcc_client_t *client, const dcc_event_t *event, void *user_data);
void async_rest_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data);
void async_order_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data);
void async_record_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data);
void *client_wait_thread_main(void *arg);
void *event_wait_thread_main(void *arg);

int expect_multipart_request(
    const http_server_t *server,
    const char *method,
    const char *path,
    const char *payload_fragment,
    const char *field_fragment,
    const char *file_fragment
);

int run_internal_http_smoke(void);
int run_internal_http_chunked_smoke(void);
int run_public_event_wait_smoke(void);
int run_public_rest_smoke(void);
int run_public_rest_raw_smoke(void);
int run_public_rest_overload_smoke(void);
int run_public_rest_multipart_smoke(void);
int run_public_rest_message_builder_smoke(void);
int run_public_rest_application_command_builder_smoke(void);
int run_public_command_registry_smoke(void);
int run_public_rest_async_smoke(void);
int run_public_rest_future_smoke(void);
int run_public_rest_async_priority_smoke(void);
int run_public_rest_async_route_serial_smoke(void);
int run_public_rest_async_control_smoke(void);
int run_public_rest_async_active_cancel_smoke(void);
int run_public_rest_async_retry_queue_smoke(void);
int run_public_rest_app_send_thread_smoke(void);
int run_public_rest_rate_limit_smoke(void);
int run_public_rest_direct_message_smoke(void);
int run_public_rest_gap_smoke(void);
int run_public_rest_official_surface_smoke(void);
int run_public_rest_wrapper_smoke(void);
int run_public_rest_bucket_smoke(void);

int run_public_rest_wrapper_expect(
    dcc_client_t *client,
    const char *label,
    rest_wrapper_call_fn call,
    const char *expected_method,
    const char *expected_path,
    const char *expected_body
);
int run_public_rest_wrapper_expect_channels_smoke(dcc_client_t *client);
int run_public_rest_wrapper_expect_guilds_smoke(dcc_client_t *client);
int run_public_rest_wrapper_expect_users_threads_smoke(dcc_client_t *client);
int run_public_rest_wrapper_expect_webhooks_apps_smoke(dcc_client_t *client);
int run_public_rest_wrapper_interactions_smoke(dcc_client_t *client);

dcc_status_t call_rest_create_guild_channel(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_channel_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_channels(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_channel_positions(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_channel_positions_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_channel_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_channel_media_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_channel_permission(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_channel_permission_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_channel_permission(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_follow_news_channel(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_trigger_channel_typing(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_set_channel_voice_status(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_set_channel_voice_status_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_channel_invites(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_channel_invite(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_channel_invite_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_edit_message_flags(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_edit_message_flags_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_crosspost_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_bulk_delete_messages(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_add_message_reaction(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_add_message_reaction_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_own_message_reaction(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_user_message_reaction(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_message_reactions(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_message_reactions_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_all_message_reactions(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_all_message_reactions_for_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_pin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_unpin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_channel_pins(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_channel_pins_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_legacy_pin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_legacy_unpin_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_legacy_channel_pins(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_poll_answer_voters(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_poll_answer_voters_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_end_poll(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_invites(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_guild(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_current_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_current_guild_member_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_set_current_guild_member_nickname(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_set_current_guild_member_nickname_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_audit_log(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_audit_log_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_preview(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_bans(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_bans_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_ban(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_ban(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_ban_seconds(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_ban_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_guild_ban(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_integrations(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_integration(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_guild_integration(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_sync_guild_integration(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_widget(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_widget(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_widget_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_vanity_url(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_prune_count(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_prune_count_options(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_begin_guild_prune(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_begin_guild_prune_options(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_begin_guild_prune_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_onboarding(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_onboarding(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_onboarding_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_welcome_screen(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_welcome_screen(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_welcome_screen_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_auto_moderation_rules(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_auto_moderation_rule(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_auto_moderation_rule(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_auto_moderation_rule_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_auto_moderation_rule(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_auto_moderation_rule_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_auto_moderation_rule(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_scheduled_events(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_scheduled_events_with_user_count(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_scheduled_event(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_scheduled_event_with_user_count(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_scheduled_event(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_scheduled_event_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_scheduled_event(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_scheduled_event_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_guild_scheduled_event(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_scheduled_event_users(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_scheduled_event_users_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_stage_instance(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_stage_instance_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_stage_instance(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_stage_instance(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_stage_instance_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_stage_instance(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_from_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_from_template_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_templates(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_template_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_template_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_sync_guild_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_guild_template(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_current_application(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_current_user(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_current_user(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_current_user_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_user(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_current_user_connections(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_current_user_guilds(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_current_user_dms(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_dm_channel(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_dm_channel_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_direct_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_direct_message_builder(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_leave_guild(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_add_group_dm_recipient(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_add_group_dm_recipient_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_remove_group_dm_recipient(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_current_user_voice_state(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_current_user_voice_state(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_current_user_voice_state_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_user_voice_state(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_user_voice_state(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_user_voice_state_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_voice_regions(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_voice_regions(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_skus(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_entitlements(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_entitlements_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_test_entitlement(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_test_entitlement_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_test_entitlement(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_consume_entitlement(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_gateway_bot(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_list_guild_members(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_list_guild_members_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_search_guild_members(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_search_guild_members_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_add_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_add_guild_member_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_member_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_set_guild_member_timeout(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_clear_guild_member_timeout(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_move_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_disconnect_guild_member_voice(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_remove_guild_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_add_guild_member_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_add_guild_member_role_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_remove_guild_member_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_roles(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_role_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_role_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_role_positions(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_role_positions_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_guild_role(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_application_role_connection_metadata(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_update_application_role_connection_metadata(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_update_application_role_connection_metadata_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_current_user_application_role_connection(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_update_current_user_application_role_connection(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_update_current_user_application_role_connection_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_thread_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_forum_thread_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_thread_from_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_thread_from_message_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_thread_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_join_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_leave_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_add_thread_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_add_thread_member_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_remove_thread_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_thread_member(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_thread_members(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_active_threads(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_public_archived_threads(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_public_archived_threads_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_private_archived_threads(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_private_archived_threads_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_joined_private_archived_threads(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_joined_private_archived_threads_page(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_emojis(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_guild_emoji_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_emoji_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_guild_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_stickers(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_sticker(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_guild_sticker(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_guild_sticker(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_sticker(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_sticker_packs(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_invite(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_invite_full(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_invite(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_webhook_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_channel_webhooks(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_guild_webhooks(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_webhook_with_token(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_webhook_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_webhook_with_token(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_webhook_with_token_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_webhook_with_token(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_execute_webhook(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_execute_webhook_options(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_webhook_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_webhook_message_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_webhook_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_webhook_message_builder(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_webhook_message_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_webhook_message(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_webhook_message_thread(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_application_emojis(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_get_application_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_application_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_create_application_emoji_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_application_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_modify_application_emoji_params(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_application_emoji(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_all_global_commands(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_delete_all_guild_commands(dcc_client_t *client, dcc_rest_cb cb, void *user_data);
dcc_status_t call_rest_bulk_edit_guild_command_permissions(dcc_client_t *client, dcc_rest_cb cb, void *user_data);

#endif

#endif
