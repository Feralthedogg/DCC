#ifndef DCC_APP_CONTEXT_H
#define DCC_APP_CONTEXT_H

#include <dcc/app/legacy.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Forwards a non-OK context status to the current application observer. */
DCC_API dcc_status_t dcc_ctx_handle_error(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const char *message
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


#ifdef __cplusplus
}
#endif

#endif
