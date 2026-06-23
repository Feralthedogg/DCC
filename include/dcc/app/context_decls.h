/* Included by <dcc/app.h>. */
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook_text(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook_multipart(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *query,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_execute_webhook_multipart_options(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    uint8_t wait,
    dcc_snowflake_t thread_id,
    uint8_t with_components,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_webhook_message_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message_builder(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message_builder_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message_multipart(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_webhook_message_multipart_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    const char *payload_json,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_webhook_message(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_webhook_message_thread(
    dcc_app_t *app,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_application(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_gateway_bot(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_skus(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_entitlements(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_entitlements_page(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_snowflake_t user_id,
    const dcc_snowflake_t *sku_ids,
    size_t sku_id_count,
    dcc_snowflake_t before_id,
    dcc_snowflake_t after_id,
    uint8_t limit,
    dcc_snowflake_t guild_id,
    uint8_t exclude_ended,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_test_entitlement(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_test_entitlement_params(
    dcc_app_t *app,
    const dcc_test_entitlement_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_test_entitlement(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_consume_entitlement(
    dcc_app_t *app,
    dcc_snowflake_t application_id,
    dcc_snowflake_t entitlement_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_user(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_user(
    dcc_app_t *app,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_current_user_params(
    dcc_app_t *app,
    const dcc_current_user_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_user(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_user_connections(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_user_guilds(
    dcc_app_t *app,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_current_user_dms(
    dcc_app_t *app,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_dm_channel(
    dcc_app_t *app,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_dm_channel_params(
    dcc_app_t *app,
    const dcc_dm_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_add_group_dm_recipient(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_add_group_dm_recipient_params(
    dcc_app_t *app,
    const dcc_group_dm_recipient_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_remove_group_dm_recipient(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_direct_message(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_direct_message_builder(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_direct_message_text(
    dcc_app_t *app,
    dcc_snowflake_t user_id,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_leave_guild(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_invites(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_invite(
    dcc_app_t *app,
    const char *invite_code,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_invite_full(
    dcc_app_t *app,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_invite(
    dcc_app_t *app,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_stage_instance(
    dcc_app_t *app,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_stage_instance_params(
    dcc_app_t *app,
    const dcc_stage_instance_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_stage_instance(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_stage_instance(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_stage_instance_params(
    dcc_app_t *app,
    const dcc_stage_instance_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_stage_instance(
    dcc_app_t *app,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_template(
    dcc_app_t *app,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_from_template(
    dcc_app_t *app,
    const char *code,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_from_template_params(
    dcc_app_t *app,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_get_guild_templates(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_create_guild_template_params(
    dcc_app_t *app,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *code,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_modify_guild_template_params(
    dcc_app_t *app,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_sync_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_app_delete_guild_template(
    dcc_app_t *app,
    dcc_snowflake_t guild_id,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_create_thread_from_message(
    dcc_ctx_t *ctx,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_create_thread_from_message_name(
    dcc_ctx_t *ctx,
    const char *name,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_archive_current_thread(
    dcc_ctx_t *ctx,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_lock_current_thread(
    dcc_ctx_t *ctx,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_unlock_current_thread(
    dcc_ctx_t *ctx,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_add_member_role(
    dcc_ctx_t *ctx,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_remove_member_role(
    dcc_ctx_t *ctx,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_add_author_role(
    dcc_ctx_t *ctx,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_remove_author_role(
    dcc_ctx_t *ctx,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_ok(
    dcc_ctx_t *ctx,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_fail(
    dcc_ctx_t *ctx,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_ctx_wait_for_component(
    dcc_ctx_t *ctx,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_ctx_wait_for_button(
    dcc_ctx_t *ctx,
    const char *custom_id,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_ctx_wait_for_button_prefix(
    dcc_ctx_t *ctx,
    const char *custom_id_prefix,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_ctx_wait_for_select(
    dcc_ctx_t *ctx,
    const char *custom_id,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_ctx_wait_for_select_prefix(
    dcc_ctx_t *ctx,
    const char *custom_id_prefix,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_ctx_wait_for_modal(
    dcc_ctx_t *ctx,
    const char *custom_id,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_ctx_wait_for_modal_prefix(
    dcc_ctx_t *ctx,
    const char *custom_id_prefix,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);
DCC_API dcc_status_t dcc_ctx_wait_for_message(
    dcc_ctx_t *ctx,
    uint32_t timeout_ms,
    dcc_app_wait_result_t *out
);

DCC_API const dcc_interaction_option_t *dcc_ctx_option(const dcc_ctx_t *ctx, const char *name);
DCC_API const dcc_interaction_option_t *dcc_ctx_focused_option(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_focused_option_name(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_focused_option_string(const dcc_ctx_t *ctx, const char *fallback);
DCC_API uint8_t dcc_ctx_option_present(const dcc_ctx_t *ctx, const char *name);
DCC_API const char *dcc_ctx_option_string(const dcc_ctx_t *ctx, const char *name, const char *fallback);
DCC_API int64_t dcc_ctx_option_int(const dcc_ctx_t *ctx, const char *name, int64_t fallback);
DCC_API double dcc_ctx_option_number(const dcc_ctx_t *ctx, const char *name, double fallback);
DCC_API uint8_t dcc_ctx_option_bool(const dcc_ctx_t *ctx, const char *name, uint8_t fallback);
DCC_API dcc_snowflake_t dcc_ctx_option_snowflake(
    const dcc_ctx_t *ctx,
    const char *name,
    dcc_snowflake_t fallback
);
DCC_API const dcc_user_t *dcc_ctx_option_user(const dcc_ctx_t *ctx, const char *name);
DCC_API const dcc_member_t *dcc_ctx_option_member(const dcc_ctx_t *ctx, const char *name);
DCC_API const dcc_role_t *dcc_ctx_option_role(const dcc_ctx_t *ctx, const char *name);
DCC_API const dcc_channel_t *dcc_ctx_option_channel(const dcc_ctx_t *ctx, const char *name);
DCC_API const dcc_message_t *dcc_ctx_option_message(const dcc_ctx_t *ctx, const char *name);
DCC_API const dcc_attachment_t *dcc_ctx_option_attachment(const dcc_ctx_t *ctx, const char *name);
DCC_API dcc_status_t dcc_ctx_autocomplete_filter_choices(
    const dcc_ctx_t *ctx,
    const dcc_autocomplete_choice_t *choices,
    size_t choices_count,
    dcc_autocomplete_choice_t *out_choices,
    size_t out_capacity,
    size_t *out_count
);
DCC_API dcc_status_t dcc_ctx_bind_options(
    const dcc_ctx_t *ctx,
    const dcc_ctx_option_binding_t *bindings,
    size_t binding_count
);
DCC_API dcc_status_t dcc_ctx_bind_option_fields(
    const dcc_ctx_t *ctx,
    void *base,
    const dcc_ctx_option_field_binding_t *bindings,
    size_t binding_count
);

DCC_API const dcc_interaction_form_field_t *dcc_ctx_form_field(const dcc_ctx_t *ctx, const char *custom_id);
DCC_API const char *dcc_ctx_form_string(const dcc_ctx_t *ctx, const char *custom_id, const char *fallback);
DCC_API uint8_t dcc_ctx_form_bool(const dcc_ctx_t *ctx, const char *custom_id, uint8_t fallback);
DCC_API size_t dcc_ctx_form_values_count(const dcc_ctx_t *ctx, const char *custom_id);
DCC_API const char *dcc_ctx_form_value(const dcc_ctx_t *ctx, const char *custom_id, size_t index);
DCC_API const char *dcc_ctx_form_first_value(
    const dcc_ctx_t *ctx,
    const char *custom_id,
    const char *fallback
);
DCC_API const char **dcc_ctx_form_values(
    const dcc_ctx_t *ctx,
    const char *custom_id,
    size_t *out_count
);
DCC_API size_t dcc_ctx_component_value_count(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_component_value(const dcc_ctx_t *ctx, size_t index);
DCC_API const char *dcc_ctx_component_first_value(const dcc_ctx_t *ctx, const char *fallback);
DCC_API const char **dcc_ctx_component_values(const dcc_ctx_t *ctx, size_t *out_count);
DCC_API dcc_status_t dcc_ctx_bind_form(
    const dcc_ctx_t *ctx,
    const dcc_ctx_form_binding_t *bindings,
    size_t binding_count
);
DCC_API dcc_status_t dcc_ctx_bind_form_fields(
    const dcc_ctx_t *ctx,
    void *base,
    const dcc_ctx_form_field_binding_t *bindings,
    size_t binding_count
);
DCC_API dcc_status_t dcc_ctx_bind_component(
    const dcc_ctx_t *ctx,
    const dcc_ctx_component_binding_t *bindings,
    size_t binding_count
);
DCC_API dcc_status_t dcc_ctx_bind_component_fields(
    const dcc_ctx_t *ctx,
    void *base,
    const dcc_ctx_component_field_binding_t *bindings,
    size_t binding_count
);
DCC_API dcc_status_t dcc_ctx_validate_fields(
    const void *base,
    const dcc_ctx_field_validator_t *validators,
    size_t validator_count,
    dcc_ctx_validation_error_t *out_error
);
DCC_API dcc_status_t dcc_ctx_parse_date(
    const char *value,
    dcc_ctx_parsed_date_t *out
);
DCC_API dcc_status_t dcc_ctx_parse_date_mmdd(
    const char *value,
    dcc_ctx_parsed_date_t *out
);
DCC_API dcc_status_t dcc_ctx_parse_date_yyyymmdd(
    const char *value,
    dcc_ctx_parsed_date_t *out
);

