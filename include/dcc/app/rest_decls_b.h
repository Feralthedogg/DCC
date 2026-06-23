/* Included by <dcc/app.h>. */
DCC_API dcc_status_t dcc_app_wait_for_interaction(
    dcc_app_t *app,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API void dcc_app_wait_filter_init(dcc_app_wait_filter_t *filter);
DCC_API dcc_status_t dcc_app_wait_filter_from_ctx(
    const dcc_ctx_t *ctx,
    dcc_app_wait_filter_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_interaction_filtered(
    dcc_app_t *app,
    dcc_event_type_t type,
    const dcc_app_wait_filter_t *filter,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_component(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_component_filtered(
    dcc_app_t *app,
    const dcc_app_wait_filter_t *filter,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_button(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_select(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_modal(
    dcc_app_t *app,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_message(
    dcc_app_t *app,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_app_wait_for_message_filtered(
    dcc_app_t *app,
    dcc_event_type_t type,
    const dcc_app_wait_filter_t *filter,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_event_type_t dcc_app_wait_result_type(const dcc_app_wait_result_t *result);
DCC_API const dcc_interaction_t *dcc_app_wait_result_interaction(
    const dcc_app_wait_result_t *result
);
DCC_API const dcc_message_t *dcc_app_wait_result_message(const dcc_app_wait_result_t *result);
DCC_API dcc_snowflake_t dcc_app_wait_result_id(const dcc_app_wait_result_t *result);
DCC_API dcc_snowflake_t dcc_app_wait_result_user_id(const dcc_app_wait_result_t *result);
DCC_API dcc_snowflake_t dcc_app_wait_result_channel_id(const dcc_app_wait_result_t *result);
DCC_API dcc_snowflake_t dcc_app_wait_result_guild_id(const dcc_app_wait_result_t *result);
DCC_API const char *dcc_app_wait_result_custom_id(const dcc_app_wait_result_t *result);
DCC_API const char *dcc_app_wait_result_message_content(
    const dcc_app_wait_result_t *result,
    const char *fallback
);
DCC_API size_t dcc_app_wait_result_value_count(const dcc_app_wait_result_t *result);
DCC_API const char *dcc_app_wait_result_value(
    const dcc_app_wait_result_t *result,
    size_t index
);
DCC_API const dcc_interaction_form_field_t *dcc_app_wait_result_form_field(
    const dcc_app_wait_result_t *result,
    const char *custom_id
);
DCC_API const char *dcc_app_wait_result_form_string(
    const dcc_app_wait_result_t *result,
    const char *custom_id,
    const char *fallback
);
DCC_API uint8_t dcc_app_wait_result_form_bool(
    const dcc_app_wait_result_t *result,
    const char *custom_id,
    uint8_t fallback
);
DCC_API size_t dcc_app_wait_result_form_value_count(
    const dcc_app_wait_result_t *result,
    const char *custom_id
);
DCC_API const char *dcc_app_wait_result_form_value(
    const dcc_app_wait_result_t *result,
    const char *custom_id,
    size_t index
);
DCC_API void dcc_app_wait_result_deinit(dcc_app_wait_result_t *result);
DCC_API dcc_status_t dcc_app_use(
    dcc_app_t *app,
    dcc_app_middleware_fn middleware,
    void *user_data
);
DCC_API dcc_status_t dcc_app_use_guild_only(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_use_dm_only(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_use_nsfw_only(dcc_app_t *app);
DCC_API dcc_status_t dcc_app_use_channel_types(
    dcc_app_t *app,
    const uint32_t *channel_types,
    size_t channel_type_count
);
DCC_API dcc_status_t dcc_app_use_owner(dcc_app_t *app, dcc_snowflake_t user_id);
DCC_API dcc_status_t dcc_app_use_user_allowlist(
    dcc_app_t *app,
    const dcc_snowflake_t *user_ids,
    size_t user_id_count
);
DCC_API dcc_status_t dcc_app_use_required_permissions(dcc_app_t *app, dcc_permission_t permissions);
DCC_API dcc_status_t dcc_app_use_required_roles(
    dcc_app_t *app,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_use_any_role(
    dcc_app_t *app,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_use_check(
    dcc_app_t *app,
    const dcc_app_check_t *check
);
DCC_API dcc_status_t dcc_app_use_cooldown(
    dcc_app_t *app,
    const dcc_app_cooldown_options_t *options
);
DCC_API dcc_status_t dcc_app_route_use(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_app_middleware_fn middleware,
    void *user_data
);
DCC_API dcc_status_t dcc_app_route_use_guild_only(dcc_app_t *app, dcc_app_route_id_t route_id);
DCC_API dcc_status_t dcc_app_route_use_dm_only(dcc_app_t *app, dcc_app_route_id_t route_id);
DCC_API dcc_status_t dcc_app_route_use_nsfw_only(dcc_app_t *app, dcc_app_route_id_t route_id);
DCC_API dcc_status_t dcc_app_route_use_channel_types(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const uint32_t *channel_types,
    size_t channel_type_count
);
DCC_API dcc_status_t dcc_app_route_use_owner(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_snowflake_t user_id
);
DCC_API dcc_status_t dcc_app_route_use_user_allowlist(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_snowflake_t *user_ids,
    size_t user_id_count
);
DCC_API dcc_status_t dcc_app_route_use_required_permissions(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    dcc_permission_t permissions
);
DCC_API dcc_status_t dcc_app_route_use_required_roles(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_route_use_any_role(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_route_use_check(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_check_t *check
);
DCC_API dcc_status_t dcc_app_route_use_cooldown(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_cooldown_options_t *options
);
DCC_API dcc_status_t dcc_app_route_apply_policy(
    dcc_app_t *app,
    dcc_app_route_id_t route_id,
    const dcc_app_route_policy_t *policy
);
DCC_API dcc_status_t dcc_app_slash(
    dcc_app_t *app,
    const char *name,
    const char *description,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_slash_route(
    dcc_app_t *app,
    const char *name,
    const char *description,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_slash_builder(
    dcc_app_t *app,
    const dcc_application_command_builder_t *command,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_slash_builder_route(
    dcc_app_t *app,
    const dcc_application_command_builder_t *command,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_user_context_menu(
    dcc_app_t *app,
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_user_context_menu_route(
    dcc_app_t *app,
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_message_context_menu(
    dcc_app_t *app,
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_message_context_menu_route(
    dcc_app_t *app,
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_subcommand(
    dcc_app_t *app,
    const char *command_name,
    const char *subcommand_path,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_subcommand_route(
    dcc_app_t *app,
    const char *command_name,
    const char *subcommand_path,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_autocomplete(
    dcc_app_t *app,
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_autocomplete_route(
    dcc_app_t *app,
    const char *name,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_button(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_button_route(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_button_prefix(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_button_prefix_route(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_select(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_select_route(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_select_prefix(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_select_prefix_route(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_modal(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modal_route(
    dcc_app_t *app,
    const char *custom_id,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_modal_prefix(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modal_prefix_route(
    dcc_app_t *app,
    const char *custom_id_prefix,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_component_session(
    dcc_app_t *app,
    const dcc_component_session_t *session,
    const dcc_component_session_listener_options_t *options,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_component_session_store(
    dcc_app_t *app,
    const dcc_component_session_store_t *store,
    const dcc_component_session_listener_options_t *options,
    dcc_app_handler_fn handler,
    void *user_data
);
DCC_API dcc_status_t dcc_app_view_register(dcc_app_t *app, const dcc_app_view_t *view);
DCC_API dcc_status_t dcc_app_route_group_init(
    dcc_app_route_group_t *group,
    dcc_app_t *app,
    const char *custom_id_prefix,
    void *user_data
);
DCC_API dcc_status_t dcc_app_route_group_button(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_button_route(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_button_prefix(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_button_prefix_route(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_select(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_select_route(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_select_prefix(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_select_prefix_route(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_modal(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_modal_route(
    const dcc_app_route_group_t *group,
    const char *action,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_modal_prefix(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler
);
DCC_API dcc_status_t dcc_app_route_group_modal_prefix_route(
    const dcc_app_route_group_t *group,
    const char *action_prefix,
    dcc_app_handler_fn handler,
    dcc_app_route_id_t *out_route
);
DCC_API dcc_status_t dcc_app_route_group_view_register(
    const dcc_app_route_group_t *group,
    const dcc_app_view_t *view
);
DCC_API dcc_status_t dcc_app_route_group_use(
    const dcc_app_route_group_t *group,
    dcc_app_middleware_fn middleware,
    void *user_data
);
DCC_API dcc_status_t dcc_app_route_group_use_guild_only(const dcc_app_route_group_t *group);
DCC_API dcc_status_t dcc_app_route_group_use_dm_only(const dcc_app_route_group_t *group);
DCC_API dcc_status_t dcc_app_route_group_use_nsfw_only(const dcc_app_route_group_t *group);
DCC_API dcc_status_t dcc_app_route_group_use_channel_types(
    const dcc_app_route_group_t *group,
    const uint32_t *channel_types,
    size_t channel_type_count
);
DCC_API dcc_status_t dcc_app_route_group_use_owner(
    const dcc_app_route_group_t *group,
    dcc_snowflake_t user_id
);
DCC_API dcc_status_t dcc_app_route_group_use_user_allowlist(
    const dcc_app_route_group_t *group,
    const dcc_snowflake_t *user_ids,
    size_t user_id_count
);
DCC_API dcc_status_t dcc_app_route_group_use_required_permissions(
    const dcc_app_route_group_t *group,
    dcc_permission_t permissions
);
DCC_API dcc_status_t dcc_app_route_group_use_required_roles(
    const dcc_app_route_group_t *group,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_route_group_use_any_role(
    const dcc_app_route_group_t *group,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API dcc_status_t dcc_app_route_group_use_check(
    const dcc_app_route_group_t *group,
    const dcc_app_check_t *check
);
DCC_API dcc_status_t dcc_app_route_group_use_cooldown(
    const dcc_app_route_group_t *group,
    const dcc_app_cooldown_options_t *options
);
DCC_API dcc_status_t dcc_app_route_group_apply_policy(
    const dcc_app_route_group_t *group,
    const dcc_app_route_policy_t *policy
);

DCC_API dcc_status_t dcc_app_every_ms(
    dcc_app_t *app,
    uint64_t interval_ms,
    dcc_app_task_fn fn,
    void *user_data
);
DCC_API dcc_status_t dcc_app_every_seconds(
    dcc_app_t *app,
    uint64_t interval_seconds,
    dcc_app_task_fn fn,
    void *user_data
);
DCC_API dcc_status_t dcc_app_every_minutes(
    dcc_app_t *app,
    uint64_t interval_minutes,
    dcc_app_task_fn fn,
    void *user_data
);
DCC_API dcc_status_t dcc_app_every_hours(
    dcc_app_t *app,
    uint64_t interval_hours,
    dcc_app_task_fn fn,
    void *user_data
);
DCC_API dcc_status_t dcc_app_every_kst(
    dcc_app_t *app,
    const char *hhmm,
    dcc_app_task_fn fn,
    void *user_data
);
DCC_API dcc_status_t dcc_app_every_day_at_kst(
    dcc_app_t *app,
    uint8_t hour,
    uint8_t minute,
    dcc_app_task_fn fn,
    void *user_data
);

DCC_API dcc_status_t dcc_app_build_command_plan(
    dcc_app_t *app,
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_remote_entry_t *remote,
    size_t remote_count,
    dcc_command_registry_plan_t *out
);
DCC_API dcc_status_t dcc_app_sync_commands_from_json(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *options,
    const char *remote_json,
    size_t remote_json_len,
    dcc_command_registry_plan_t *out_plan,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_apply_command_plan(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_plan_t *plan,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_app_t *dcc_ctx_app(const dcc_ctx_t *ctx);
DCC_API dcc_client_t *dcc_ctx_client(const dcc_ctx_t *ctx);
DCC_API const dcc_event_t *dcc_ctx_event(const dcc_ctx_t *ctx);
DCC_API const dcc_interaction_t *dcc_ctx_interaction(const dcc_ctx_t *ctx);
DCC_API dcc_interaction_flow_t *dcc_ctx_flow(dcc_ctx_t *ctx);
DCC_API void *dcc_ctx_user_data(const dcc_ctx_t *ctx);
DCC_API void *dcc_ctx_state(const dcc_ctx_t *ctx);
DCC_API dcc_event_type_t dcc_ctx_event_type(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_interaction_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_application_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_guild_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_channel_id(const dcc_ctx_t *ctx);
DCC_API dcc_status_t dcc_ctx_channel_type(const dcc_ctx_t *ctx, uint32_t *out_type);
DCC_API uint8_t dcc_ctx_channel_is_type(const dcc_ctx_t *ctx, uint32_t channel_type);
DCC_API uint8_t dcc_ctx_channel_is_nsfw(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_command_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_message_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_target_id(const dcc_ctx_t *ctx);
DCC_API dcc_snowflake_t dcc_ctx_user_id(const dcc_ctx_t *ctx);
DCC_API const dcc_guild_t *dcc_ctx_guild(const dcc_ctx_t *ctx);
DCC_API const dcc_channel_t *dcc_ctx_channel(const dcc_ctx_t *ctx);
DCC_API const dcc_user_t *dcc_ctx_user(const dcc_ctx_t *ctx);
DCC_API const dcc_member_t *dcc_ctx_member(const dcc_ctx_t *ctx);
DCC_API const dcc_user_t *dcc_ctx_resolved_user(const dcc_ctx_t *ctx, dcc_snowflake_t id);
DCC_API const dcc_member_t *dcc_ctx_resolved_member(const dcc_ctx_t *ctx, dcc_snowflake_t user_id);
DCC_API const dcc_role_t *dcc_ctx_resolved_role(const dcc_ctx_t *ctx, dcc_snowflake_t id);
DCC_API const dcc_channel_t *dcc_ctx_resolved_channel(const dcc_ctx_t *ctx, dcc_snowflake_t id);
DCC_API const dcc_message_t *dcc_ctx_resolved_message(const dcc_ctx_t *ctx, dcc_snowflake_t id);
DCC_API const dcc_attachment_t *dcc_ctx_resolved_attachment(const dcc_ctx_t *ctx, dcc_snowflake_t id);
DCC_API const dcc_user_t *dcc_ctx_target_user(const dcc_ctx_t *ctx);
DCC_API const dcc_member_t *dcc_ctx_target_member(const dcc_ctx_t *ctx);
DCC_API const dcc_role_t *dcc_ctx_target_role(const dcc_ctx_t *ctx);
DCC_API const dcc_channel_t *dcc_ctx_target_channel(const dcc_ctx_t *ctx);
DCC_API const dcc_message_t *dcc_ctx_target_message(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_has_member_permissions(const dcc_ctx_t *ctx);
DCC_API dcc_permission_t dcc_ctx_member_permissions(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_member_has_permissions(const dcc_ctx_t *ctx, dcc_permission_t permissions);
DCC_API uint8_t dcc_ctx_member_has_role(const dcc_ctx_t *ctx, dcc_snowflake_t role_id);
DCC_API uint8_t dcc_ctx_member_has_any_role(
    const dcc_ctx_t *ctx,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API uint8_t dcc_ctx_member_has_all_roles(
    const dcc_ctx_t *ctx,
    const dcc_snowflake_t *role_ids,
    size_t role_id_count
);
DCC_API const dcc_component_session_result_t *dcc_ctx_component_session_result(const dcc_ctx_t *ctx);
DCC_API dcc_component_session_verify_status_t dcc_ctx_component_session_status(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_component_session_action(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_token(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_name(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_subcommand(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_subcommand_group(const dcc_ctx_t *ctx);
DCC_API dcc_status_t dcc_ctx_subcommand_path(
    const dcc_ctx_t *ctx,
    char *buffer,
    size_t buffer_len
);
DCC_API const char *dcc_ctx_custom_id(const dcc_ctx_t *ctx);
DCC_API dcc_interaction_flow_state_t dcc_ctx_response_state(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_response_state_string(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_response_sent(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_deferred(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_replied(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_followed_up(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_response_failed(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_can_followup(const dcc_ctx_t *ctx);
DCC_API uint8_t dcc_ctx_can_edit_original(const dcc_ctx_t *ctx);

DCC_API dcc_status_t dcc_ctx_reply(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_ephemeral_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_embed(
    dcc_ctx_t *ctx,
    const char *title,
    const char *description,
    uint32_t color,
    uint8_t ephemeral,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_error(
    dcc_ctx_t *ctx,
    const char *title,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_validation_error(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_validation_result(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const dcc_ctx_validation_error_t *error,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_success(
    dcc_ctx_t *ctx,
    const char *title,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_defer(dcc_ctx_t *ctx, dcc_rest_cb cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_defer_ephemeral(dcc_ctx_t *ctx, dcc_rest_cb cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_update_message(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_show_modal(
    dcc_ctx_t *ctx,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_show_modal_flow(
    dcc_ctx_t *ctx,
    const dcc_app_modal_flow_t *flow,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_autocomplete(
    dcc_ctx_t *ctx,
    const dcc_autocomplete_builder_t *autocomplete,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_autocomplete_matching(
    dcc_ctx_t *ctx,
    const dcc_autocomplete_choice_t *choices,
    size_t choices_count,
    size_t max_choices,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_edit_original(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_delete_original(dcc_ctx_t *ctx, dcc_rest_cb cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_followup(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_followup_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_followup_ephemeral_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send_with_id(
    dcc_ctx_t *ctx,
    const dcc_message_builder_t *message,
    dcc_app_message_id_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send_text(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_send_text_with_id(
    dcc_ctx_t *ctx,
    const char *content,
    dcc_app_message_id_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_channel_webhooks(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_webhooks(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_webhook_with_token(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_webhook(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_webhook_params(
    dcc_app_t *app,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_params(
    dcc_app_t *app,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_with_token(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_with_token_params(
    dcc_app_t *app,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_webhook_with_token(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *query,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook_options(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook_message(
