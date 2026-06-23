/* Included by <dcc/app.h>. */
DCC_API dcc_status_t dcc_app_start(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_stop(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_wait(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_run(dcc_app_t *app);

DCC_API dcc_status_t dcc_app_auto_defer(dcc_app_t *app, uint64_t after_ms);
DCC_API dcc_status_t dcc_app_auto_defer_ephemeral(dcc_app_t *app, uint64_t after_ms);
DCC_API dcc_status_t dcc_app_disable_auto_defer(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_sync_commands_on_ready(
    dcc_app_t *app,
    const dcc_app_command_sync_options_t *options
);
DCC_API dcc_status_t dcc_app_disable_command_sync_on_ready(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_on_error(
    dcc_app_t *app,
    dcc_app_error_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_use_default_error_responses(dcc_app_t *app);
DCC_API dcc_status_t dcc_ctx_handle_error(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const char *message
);
DCC_API void dcc_app_cooldown_options_init(dcc_app_cooldown_options_t *options);
DCC_API dcc_status_t dcc_app_on(
    dcc_app_t *app,
    dcc_event_type_t type,
    dcc_app_event_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_ready(
    dcc_app_t *app,
    dcc_app_ready_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_ready_once(
    dcc_app_t *app,
    dcc_app_ready_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_message_create(
    dcc_app_t *app,
    dcc_app_message_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_message_command(
    dcc_app_t *app,
    const char *prefix,
    const char *name,
    dcc_app_message_command_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_message_update(
    dcc_app_t *app,
    dcc_app_message_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_on_message_delete(
    dcc_app_t *app,
    dcc_app_message_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_with_id(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    dcc_app_message_id_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_with_thread(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    const dcc_thread_params_t *thread,
    dcc_app_message_thread_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_with_thread_name(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    const char *thread_name,
    dcc_app_message_thread_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_text(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_text_with_id(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *content,
    dcc_app_message_id_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_text_with_thread(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *content,
    const char *thread_name,
    dcc_app_message_thread_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_send_json(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_infer_guild_id_from_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_app_infer_guild_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_channels(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_channel(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_channel_params(
    dcc_app_t *app,
    const dcc_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_channel_params(
    dcc_app_t *app,
    const dcc_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_trigger_channel_typing(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_follow_news_channel(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t webhook_channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_set_channel_voice_status(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *status,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_user_voice_state_params(
    dcc_app_t *app,
    const dcc_voice_state_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_user_voice_state(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_user_voice_state_params(
    dcc_app_t *app,
    const dcc_voice_state_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_voice_regions(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_voice_regions(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_channel_positions(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_channel_positions_params(
    dcc_app_t *app,
    const dcc_channel_positions_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_channel_invites(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_channel_invite(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_channel_invite_params(
    dcc_app_t *app,
    const dcc_invite_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_channel_permission(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_channel_permission_params(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const dcc_channel_permission_overwrite_t *overwrite,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_channel_permission(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_roles(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_role_params(
    dcc_app_t *app,
    const dcc_role_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_role_params(
    dcc_app_t *app,
    const dcc_role_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_role_positions(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_role_position_t *positions,
    size_t position_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_edit_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_edit_message_text(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_crosspost_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_pin_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_unpin_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_add_message_reaction(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_own_message_reaction(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_user_message_reaction(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_all_message_reactions(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_all_message_reactions_for_emoji(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_thread_from_message(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_thread_from_message_name(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *name,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_thread(
    dcc_app_t *app,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_archive_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_lock_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_unlock_thread(
    dcc_app_t *app,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_add_member_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_remove_member_role(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_set_member_timeout(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *communication_disabled_until,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_clear_member_timeout(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_move_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_remove_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_bans(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_bans_page(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    uint64_t limit,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_ban(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_ban(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_ban_seconds(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    uint32_t delete_message_seconds,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_ban_params(
    dcc_app_t *app,
    const dcc_guild_ban_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_ban(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_audit_log(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_audit_log_page(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    uint32_t action_type,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    uint32_t limit,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_guild_member(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_guild_member_params(
    dcc_app_t *app,
    const dcc_current_guild_member_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_set_current_guild_member_nickname(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_set_current_guild_member_nickname_params(
    dcc_app_t *app,
    const dcc_guild_member_nickname_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_prune_count_options(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    uint32_t days,
    const dcc_snowflake_t *include_roles,
    size_t include_role_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_begin_guild_prune_options(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    uint32_t days,
    const dcc_snowflake_t *include_roles,
    size_t include_role_count,
    uint8_t compute_prune_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_begin_guild_prune_params(
    dcc_app_t *app,
    const dcc_guild_prune_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_integrations(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_sync_guild_integration(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_widget(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_widget(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_widget_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_guild_widget_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_vanity_url(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_onboarding(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_onboarding(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_onboarding_params(
    dcc_app_t *app,
    const dcc_onboarding_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_welcome_screen(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_welcome_screen(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_welcome_screen_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_welcome_screen_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_auto_moderation_rules(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_auto_moderation_rule_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_auto_moderation_rule_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_auto_moderation_rule(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_emojis(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_emoji_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_emoji_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_emoji_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_emoji_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_emoji(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t emoji_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_stickers(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_sticker(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_sticker_multipart(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *file,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_sticker_params(
    dcc_app_t *app,
    const dcc_guild_sticker_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_sticker(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_sticker(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_sticker(
    dcc_app_t *app,
    dcc_snowflake_t sticker_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_sticker_packs(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_guild_message_search(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_guild_message_search_params(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const dcc_message_search_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_events(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_events_with_user_count(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_event_with_user_count(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_scheduled_event_params(
    dcc_app_t *app,
    const dcc_scheduled_event_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_scheduled_event_params(
    dcc_app_t *app,
    const dcc_scheduled_event_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_scheduled_event(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_event_users(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_scheduled_event_users_page(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    uint8_t limit,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_managed_message_publish_latest(
    dcc_app_t *app,
    const dcc_managed_message_options_t *options,
    dcc_managed_message_publish_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_managed_message_publish_latest_store(
    dcc_app_t *app,
    const char *key,
    dcc_snowflake_t channel_id,
    const dcc_message_builder_t *message,
    dcc_managed_message_publish_cb cb,
    void *user_data
);
