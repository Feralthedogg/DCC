#ifndef DCC_APP_CONTEXT_H
#define DCC_APP_CONTEXT_H

#include <dcc/app/base.h>
#include <dcc/autocomplete.h>
#include <dcc/events/wait/types.h>
#include <dcc/objects/interactions.h>
#include <dcc/rest/base.h>
#include <dcc/rest/request.h>
#include <dcc/rest/types/threads.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_app_modal_flow dcc_app_modal_flow_t;

/** Returns the borrowed context interaction ID, or zero when unavailable. */
DCC_API dcc_snowflake_t dcc_ctx_interaction_id(const dcc_ctx_t *ctx);
/** Returns the client borrowed from the context, or null when unavailable. */
DCC_API dcc_client_t *dcc_ctx_client(const dcc_ctx_t *ctx);
/** Returns the interaction borrowed for the context callback duration. */
DCC_API const dcc_interaction_t *dcc_ctx_interaction(const dcc_ctx_t *ctx);

typedef struct dcc_app_wait_result {
  size_t size;
  dcc_event_snapshot_t snapshot;
  dcc_interaction_t *interaction;
  dcc_message_t *message;
} dcc_app_wait_result_t;

typedef struct dcc_app_wait_filter {
  size_t size;
  dcc_snowflake_t user_id;
  dcc_snowflake_t channel_id;
  dcc_snowflake_t guild_id;
  const char *custom_id;
  const char *custom_id_prefix;
} dcc_app_wait_filter_t;

typedef enum dcc_ctx_option_bind_type {
  DCC_CTX_OPTION_BIND_STRING = 1,
  DCC_CTX_OPTION_BIND_INTEGER,
  DCC_CTX_OPTION_BIND_NUMBER,
  DCC_CTX_OPTION_BIND_BOOLEAN,
  DCC_CTX_OPTION_BIND_SNOWFLAKE,
  DCC_CTX_OPTION_BIND_USER,
  DCC_CTX_OPTION_BIND_MEMBER,
  DCC_CTX_OPTION_BIND_ROLE,
  DCC_CTX_OPTION_BIND_CHANNEL,
  DCC_CTX_OPTION_BIND_MESSAGE,
  DCC_CTX_OPTION_BIND_ATTACHMENT
} dcc_ctx_option_bind_type_t;

typedef struct dcc_ctx_option_binding {
  size_t size;
  const char *name;
  dcc_ctx_option_bind_type_t type;
  void *out;
  uint8_t required;
  const char *fallback_string;
  int64_t fallback_integer;
  double fallback_number;
  uint8_t fallback_boolean;
  dcc_snowflake_t fallback_snowflake;
} dcc_ctx_option_binding_t;

typedef struct dcc_ctx_option_field_binding {
  size_t size;
  const char *name;
  dcc_ctx_option_bind_type_t type;
  size_t field_offset;
  uint8_t required;
  const char *fallback_string;
  int64_t fallback_integer;
  double fallback_number;
  uint8_t fallback_boolean;
  dcc_snowflake_t fallback_snowflake;
} dcc_ctx_option_field_binding_t;

typedef struct dcc_ctx_option_field_binding_list {
  const dcc_ctx_option_field_binding_t *items;
  size_t count;
} dcc_ctx_option_field_binding_list_t;

typedef enum dcc_ctx_form_bind_type {
  DCC_CTX_FORM_BIND_STRING = 1,
  DCC_CTX_FORM_BIND_BOOLEAN,
  DCC_CTX_FORM_BIND_VALUES
} dcc_ctx_form_bind_type_t;

typedef struct dcc_ctx_form_binding {
  size_t size;
  const char *custom_id;
  dcc_ctx_form_bind_type_t type;
  void *out;
  size_t *out_count;
  uint8_t required;
  const char *fallback_string;
  uint8_t fallback_boolean;
  const char **fallback_values;
  size_t fallback_values_count;
} dcc_ctx_form_binding_t;

typedef struct dcc_ctx_form_field_binding {
  size_t size;
  const char *custom_id;
  dcc_ctx_form_bind_type_t type;
  size_t field_offset;
  size_t count_offset;
  uint8_t has_count_offset;
  uint8_t required;
  const char *fallback_string;
  uint8_t fallback_boolean;
  const char **fallback_values;
  size_t fallback_values_count;
} dcc_ctx_form_field_binding_t;

typedef struct dcc_ctx_form_field_binding_list {
  const dcc_ctx_form_field_binding_t *items;
  size_t count;
} dcc_ctx_form_field_binding_list_t;

typedef enum dcc_ctx_component_bind_type {
  DCC_CTX_COMPONENT_BIND_CUSTOM_ID = 1,
  DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX,
  DCC_CTX_COMPONENT_BIND_VALUES,
  DCC_CTX_COMPONENT_BIND_FIRST_VALUE,
  DCC_CTX_COMPONENT_BIND_COMPONENT_TYPE
} dcc_ctx_component_bind_type_t;

typedef struct dcc_ctx_component_binding {
  size_t size;
  dcc_ctx_component_bind_type_t type;
  const char *custom_id_prefix;
  void *out;
  size_t *out_count;
  uint8_t required;
  const char *fallback_string;
  const char **fallback_values;
  size_t fallback_values_count;
  uint32_t fallback_component_type;
} dcc_ctx_component_binding_t;

typedef struct dcc_ctx_component_field_binding {
  size_t size;
  dcc_ctx_component_bind_type_t type;
  const char *custom_id_prefix;
  size_t field_offset;
  size_t count_offset;
  uint8_t has_count_offset;
  uint8_t required;
  const char *fallback_string;
  const char **fallback_values;
  size_t fallback_values_count;
  uint32_t fallback_component_type;
} dcc_ctx_component_field_binding_t;

typedef struct dcc_ctx_component_field_binding_list {
  const dcc_ctx_component_field_binding_t *items;
  size_t count;
} dcc_ctx_component_field_binding_list_t;

typedef enum dcc_ctx_field_validate_type {
  DCC_CTX_FIELD_VALIDATE_STRING_REQUIRED = 1,
  DCC_CTX_FIELD_VALIDATE_STRING_LENGTH,
  DCC_CTX_FIELD_VALIDATE_INTEGER_RANGE,
  DCC_CTX_FIELD_VALIDATE_NUMBER_RANGE,
  DCC_CTX_FIELD_VALIDATE_DATE_YYYYMMDD,
  DCC_CTX_FIELD_VALIDATE_DATE_MMDD,
  DCC_CTX_FIELD_VALIDATE_DATE_MMDD_OR_YYYYMMDD
} dcc_ctx_field_validate_type_t;

typedef struct dcc_ctx_field_validator {
  size_t size;
  const char *field;
  dcc_ctx_field_validate_type_t type;
  size_t field_offset;
  size_t min_length;
  size_t max_length;
  int64_t min_integer;
  int64_t max_integer;
  double min_number;
  double max_number;
} dcc_ctx_field_validator_t;

typedef struct dcc_ctx_validation_error {
  size_t size;
  const char *field;
  const char *message;
  dcc_status_t status;
} dcc_ctx_validation_error_t;

typedef struct dcc_ctx_parsed_date {
  size_t size;
  int year;
  uint8_t month;
  uint8_t day;
  uint8_t has_year;
} dcc_ctx_parsed_date_t;

typedef struct dcc_ctx_field_validator_list {
  const dcc_ctx_field_validator_t *items;
  size_t count;
} dcc_ctx_field_validator_list_t;

/** Forwards a non-OK context status to the current application observer. */
DCC_API dcc_status_t dcc_ctx_handle_error(dcc_ctx_t *ctx, dcc_status_t status,
                                          const char *message);

DCC_API dcc_status_t dcc_ctx_reply(dcc_ctx_t *ctx,
                                   const dcc_message_builder_t *message,
                                   dcc_rest_result_fn cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_reply_text(dcc_ctx_t *ctx, const char *content,
                                        dcc_rest_result_fn cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_reply_ephemeral_text(dcc_ctx_t *ctx,
                                                  const char *content,
                                                  dcc_rest_result_fn cb,
                                                  void *user_data);
DCC_API dcc_status_t dcc_ctx_reply_embed(dcc_ctx_t *ctx, const char *title,
                                         const char *description,
                                         uint32_t color, uint8_t ephemeral,
                                         dcc_rest_result_fn cb,
                                         void *user_data);
DCC_API dcc_status_t dcc_ctx_reply_error(dcc_ctx_t *ctx, const char *title,
                                         const char *description,
                                         dcc_rest_result_fn cb,
                                         void *user_data);
DCC_API dcc_status_t dcc_ctx_reply_success(dcc_ctx_t *ctx, const char *title,
                                           const char *description,
                                           dcc_rest_result_fn cb,
                                           void *user_data);
DCC_API dcc_status_t dcc_ctx_defer(dcc_ctx_t *ctx, dcc_rest_result_fn cb,
                                   void *user_data);
DCC_API dcc_status_t dcc_ctx_defer_ephemeral(dcc_ctx_t *ctx,
                                             dcc_rest_result_fn cb,
                                             void *user_data);
DCC_API dcc_status_t
dcc_ctx_update_message(dcc_ctx_t *ctx, const dcc_message_builder_t *message,
                       dcc_rest_result_fn cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_show_modal(dcc_ctx_t *ctx,
                                        const dcc_modal_builder_t *modal,
                                        dcc_rest_result_fn cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_show_modal_flow(dcc_ctx_t *ctx,
                                             const dcc_app_modal_flow_t *flow,
                                             dcc_rest_result_fn cb,
                                             void *user_data);
DCC_API dcc_status_t dcc_ctx_reply_autocomplete(
    dcc_ctx_t *ctx, const dcc_autocomplete_builder_t *autocomplete,
    dcc_rest_result_fn cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_reply_autocomplete_matching(
    dcc_ctx_t *ctx, const dcc_autocomplete_choice_t *choices,
    size_t choices_count, size_t max_choices, dcc_rest_result_fn cb,
    void *user_data);
DCC_API dcc_status_t dcc_ctx_edit_original(dcc_ctx_t *ctx,
                                           const dcc_message_builder_t *message,
                                           dcc_rest_result_fn cb,
                                           void *user_data);
DCC_API dcc_status_t dcc_ctx_delete_original(dcc_ctx_t *ctx,
                                             dcc_rest_result_fn cb,
                                             void *user_data);
DCC_API dcc_status_t dcc_ctx_followup(dcc_ctx_t *ctx,
                                      const dcc_message_builder_t *message,
                                      dcc_rest_result_fn cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_followup_text(dcc_ctx_t *ctx, const char *content,
                                           dcc_rest_result_fn cb,
                                           void *user_data);
DCC_API dcc_status_t dcc_ctx_followup_ephemeral_text(dcc_ctx_t *ctx,
                                                     const char *content,
                                                     dcc_rest_result_fn cb,
                                                     void *user_data);
/** Sends an unrelated channel message; builder input is consumed synchronously. */
DCC_API dcc_status_t dcc_ctx_send(
    dcc_ctx_t *ctx, const dcc_message_builder_t *message,
    dcc_rest_cb cb, void *user_data
);
/** Sends unrelated channel text; content is borrowed for the call. */
DCC_API dcc_status_t dcc_ctx_send_text(
    dcc_ctx_t *ctx, const char *content, dcc_rest_cb cb, void *user_data
);
DCC_API dcc_status_t dcc_ctx_reply_validation_error(dcc_ctx_t *ctx,
                                                    dcc_status_t status,
                                                    dcc_rest_result_fn cb,
                                                    void *user_data);
DCC_API dcc_status_t
dcc_ctx_reply_validation_result(dcc_ctx_t *ctx, dcc_status_t status,
                                const dcc_ctx_validation_error_t *error,
                                dcc_rest_result_fn cb, void *user_data);

DCC_API dcc_status_t dcc_ctx_create_thread_from_message(
    dcc_ctx_t *ctx, const dcc_thread_params_t *params, dcc_rest_result_fn cb,
    void *user_data);
DCC_API dcc_status_t dcc_ctx_create_thread_from_message_name(
    dcc_ctx_t *ctx, const char *name, dcc_rest_result_fn cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_archive_current_thread(dcc_ctx_t *ctx,
                                                    dcc_rest_result_fn cb,
                                                    void *user_data);
DCC_API dcc_status_t dcc_ctx_lock_current_thread(dcc_ctx_t *ctx,
                                                 dcc_rest_result_fn cb,
                                                 void *user_data);
DCC_API dcc_status_t dcc_ctx_unlock_current_thread(dcc_ctx_t *ctx,
                                                   dcc_rest_result_fn cb,
                                                   void *user_data);
DCC_API dcc_status_t dcc_ctx_add_member_role(dcc_ctx_t *ctx,
                                             dcc_snowflake_t user_id,
                                             dcc_snowflake_t role_id,
                                             dcc_rest_result_fn cb,
                                             void *user_data);
DCC_API dcc_status_t dcc_ctx_remove_member_role(dcc_ctx_t *ctx,
                                                dcc_snowflake_t user_id,
                                                dcc_snowflake_t role_id,
                                                dcc_rest_result_fn cb,
                                                void *user_data);
DCC_API dcc_status_t dcc_ctx_add_author_role(dcc_ctx_t *ctx,
                                             dcc_snowflake_t role_id,
                                             dcc_rest_result_fn cb,
                                             void *user_data);
DCC_API dcc_status_t dcc_ctx_remove_author_role(dcc_ctx_t *ctx,
                                                dcc_snowflake_t role_id,
                                                dcc_rest_result_fn cb,
                                                void *user_data);
DCC_API dcc_status_t dcc_ctx_ok(dcc_ctx_t *ctx, const char *description,
                                dcc_rest_result_fn cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_fail(dcc_ctx_t *ctx, const char *description,
                                  dcc_rest_result_fn cb, void *user_data);
DCC_API dcc_status_t dcc_ctx_wait_for_component(dcc_ctx_t *ctx,
                                                uint32_t timeout_ms,
                                                dcc_app_wait_result_t *out);
DCC_API dcc_status_t dcc_ctx_wait_for_button(dcc_ctx_t *ctx,
                                             const char *custom_id,
                                             uint32_t timeout_ms,
                                             dcc_app_wait_result_t *out);
DCC_API dcc_status_t
dcc_ctx_wait_for_button_prefix(dcc_ctx_t *ctx, const char *custom_id_prefix,
                               uint32_t timeout_ms, dcc_app_wait_result_t *out);
DCC_API dcc_status_t dcc_ctx_wait_for_select(dcc_ctx_t *ctx,
                                             const char *custom_id,
                                             uint32_t timeout_ms,
                                             dcc_app_wait_result_t *out);
DCC_API dcc_status_t
dcc_ctx_wait_for_select_prefix(dcc_ctx_t *ctx, const char *custom_id_prefix,
                               uint32_t timeout_ms, dcc_app_wait_result_t *out);
DCC_API dcc_status_t dcc_ctx_wait_for_modal(dcc_ctx_t *ctx,
                                            const char *custom_id,
                                            uint32_t timeout_ms,
                                            dcc_app_wait_result_t *out);
DCC_API dcc_status_t dcc_ctx_wait_for_modal_prefix(dcc_ctx_t *ctx,
                                                   const char *custom_id_prefix,
                                                   uint32_t timeout_ms,
                                                   dcc_app_wait_result_t *out);
DCC_API dcc_status_t dcc_ctx_wait_for_message(dcc_ctx_t *ctx,
                                              uint32_t timeout_ms,
                                              dcc_app_wait_result_t *out);

DCC_API const dcc_interaction_option_t *dcc_ctx_option(const dcc_ctx_t *ctx,
                                                       const char *name);
DCC_API const dcc_interaction_option_t *
dcc_ctx_focused_option(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_focused_option_name(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_focused_option_string(const dcc_ctx_t *ctx,
                                                  const char *fallback);
DCC_API uint8_t dcc_ctx_option_present(const dcc_ctx_t *ctx, const char *name);
DCC_API const char *dcc_ctx_option_string(const dcc_ctx_t *ctx,
                                          const char *name,
                                          const char *fallback);
DCC_API int64_t dcc_ctx_option_int(const dcc_ctx_t *ctx, const char *name,
                                   int64_t fallback);
DCC_API double dcc_ctx_option_number(const dcc_ctx_t *ctx, const char *name,
                                     double fallback);
DCC_API uint8_t dcc_ctx_option_bool(const dcc_ctx_t *ctx, const char *name,
                                    uint8_t fallback);
DCC_API dcc_snowflake_t dcc_ctx_option_snowflake(const dcc_ctx_t *ctx,
                                                 const char *name,
                                                 dcc_snowflake_t fallback);
DCC_API const dcc_user_t *dcc_ctx_option_user(const dcc_ctx_t *ctx,
                                              const char *name);
DCC_API const dcc_member_t *dcc_ctx_option_member(const dcc_ctx_t *ctx,
                                                  const char *name);
DCC_API const dcc_role_t *dcc_ctx_option_role(const dcc_ctx_t *ctx,
                                              const char *name);
DCC_API const dcc_channel_t *dcc_ctx_option_channel(const dcc_ctx_t *ctx,
                                                    const char *name);
DCC_API const dcc_message_t *dcc_ctx_option_message(const dcc_ctx_t *ctx,
                                                    const char *name);
DCC_API const dcc_attachment_t *dcc_ctx_option_attachment(const dcc_ctx_t *ctx,
                                                          const char *name);
/*
 * out_choices is a caller-owned array whose out_capacity slots have each been
 * initialized with DCC_AUTOCOMPLETE_CHOICE_INIT or
 * dcc_autocomplete_choice_init(). All slots must declare the same stride.
 * Every source choice is validated even when out_capacity is zero. out_count
 * is required, is set to zero before validation, and remains zero on error or
 * when no choice is emitted. A valid empty source with zero output capacity
 * succeeds; out_choices may be NULL in that case.
 */
DCC_API dcc_status_t dcc_ctx_autocomplete_filter_choices(
    const dcc_ctx_t *ctx, const dcc_autocomplete_choice_t *choices,
    size_t choices_count, dcc_autocomplete_choice_t *out_choices,
    size_t out_capacity, size_t *out_count);
DCC_API dcc_status_t dcc_ctx_bind_options(
    const dcc_ctx_t *ctx, const dcc_ctx_option_binding_t *bindings,
    size_t binding_count);
DCC_API dcc_status_t dcc_ctx_bind_option_fields(
    const dcc_ctx_t *ctx, void *base,
    const dcc_ctx_option_field_binding_t *bindings, size_t binding_count);

DCC_API const dcc_interaction_form_field_t *
dcc_ctx_form_field(const dcc_ctx_t *ctx, const char *custom_id);
DCC_API const char *dcc_ctx_form_string(const dcc_ctx_t *ctx,
                                        const char *custom_id,
                                        const char *fallback);
DCC_API uint8_t dcc_ctx_form_bool(const dcc_ctx_t *ctx, const char *custom_id,
                                  uint8_t fallback);
DCC_API size_t dcc_ctx_form_values_count(const dcc_ctx_t *ctx,
                                         const char *custom_id);
DCC_API const char *dcc_ctx_form_value(const dcc_ctx_t *ctx,
                                       const char *custom_id, size_t index);
DCC_API const char *dcc_ctx_form_first_value(const dcc_ctx_t *ctx,
                                             const char *custom_id,
                                             const char *fallback);
DCC_API const char **dcc_ctx_form_values(const dcc_ctx_t *ctx,
                                         const char *custom_id,
                                         size_t *out_count);
DCC_API size_t dcc_ctx_component_value_count(const dcc_ctx_t *ctx);
DCC_API const char *dcc_ctx_component_value(const dcc_ctx_t *ctx, size_t index);
DCC_API const char *dcc_ctx_component_first_value(const dcc_ctx_t *ctx,
                                                  const char *fallback);
DCC_API const char **dcc_ctx_component_values(const dcc_ctx_t *ctx,
                                              size_t *out_count);
DCC_API dcc_status_t dcc_ctx_bind_form(const dcc_ctx_t *ctx,
                                       const dcc_ctx_form_binding_t *bindings,
                                       size_t binding_count);
DCC_API dcc_status_t dcc_ctx_bind_form_fields(
    const dcc_ctx_t *ctx, void *base,
    const dcc_ctx_form_field_binding_t *bindings, size_t binding_count);
DCC_API dcc_status_t dcc_ctx_bind_component(
    const dcc_ctx_t *ctx, const dcc_ctx_component_binding_t *bindings,
    size_t binding_count);
DCC_API dcc_status_t dcc_ctx_bind_component_fields(
    const dcc_ctx_t *ctx, void *base,
    const dcc_ctx_component_field_binding_t *bindings, size_t binding_count);
DCC_API dcc_status_t dcc_ctx_validate_fields(
    const void *base, const dcc_ctx_field_validator_t *validators,
    size_t validator_count, dcc_ctx_validation_error_t *out_error);
DCC_API dcc_status_t dcc_ctx_parse_date(const char *value,
                                        dcc_ctx_parsed_date_t *out);
DCC_API dcc_status_t dcc_ctx_parse_date_mmdd(const char *value,
                                             dcc_ctx_parsed_date_t *out);
DCC_API dcc_status_t dcc_ctx_parse_date_yyyymmdd(const char *value,
                                                 dcc_ctx_parsed_date_t *out);

#ifdef __cplusplus
}
#endif

#endif
