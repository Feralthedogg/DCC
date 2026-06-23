#include <dcc/sugar.h>

#include "internal/app/dcc_app_internal.h"

#include <dcc/cache.h>

#include <stdio.h>
#include <string.h>

static int require_contains(const char *json, const char *needle) {
    if (json == NULL || strstr(json, needle) == NULL) {
        fprintf(stderr, "missing JSON fragment: %s\njson=%s\n", needle, json != NULL ? json : "(null)");
        return 1;
    }
    return 0;
}

static void sugar_dummy_handler(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
}

static void sugar_dummy_typed_handler(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)ctx;
    (void)args;
    (void)user_data;
}

typedef struct sugar_typed_impl_args {
    const char *value;
    uint32_t count;
} sugar_typed_impl_args_t;

DCC_SLASH_ARGS_IMPL(sugar_typed_impl_handler, sugar_typed_impl_args_t, command) {
    (void)ctx;
    if (command->value != NULL && strcmp(command->value, "ok") == 0 && user_data != NULL) {
        *((uint32_t *)user_data) += command->count;
    }
}

DCC_BUTTON_ARGS_MUT_IMPL(sugar_typed_mut_impl_handler, sugar_typed_impl_args_t, button) {
    (void)ctx;
    (void)user_data;
    if (button->value != NULL && strcmp(button->value, "ok") == 0) {
        button->count++;
    }
}

DCC_COMMAND_IMPL(sugar_no_args_command_handler) {
    (void)ctx;
    if (user_data != NULL) {
        (*(uint32_t *)user_data)++;
    }
}

static dcc_status_t sugar_dummy_middleware(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
    return DCC_OK;
}

static dcc_status_t sugar_dummy_check(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
    return DCC_OK;
}

typedef struct sugar_error_state {
    uint32_t count;
    uint8_t after_try;
    dcc_status_t status;
    const char *message;
} sugar_error_state_t;

static void sugar_error_handler(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const char *message,
    void *user_data
) {
    sugar_error_state_t *state = (sugar_error_state_t *)user_data;
    if (ctx == NULL || state == NULL) {
        return;
    }
    state->count++;
    state->status = status;
    state->message = message;
}

static dcc_status_t sugar_status_ok(void) {
    return DCC_OK;
}

static dcc_status_t sugar_status_failed(void) {
    return DCC_ERR_STATE;
}

static void sugar_try_ok(dcc_ctx_t *ctx, sugar_error_state_t *state) {
    DCC_CTX_TRY(ctx, sugar_status_ok());
    state->after_try = 1U;
}

static void sugar_try_failed(dcc_ctx_t *ctx, sugar_error_state_t *state) {
    DCC_CTX_TRY_MSG(ctx, sugar_status_failed(), "custom failure");
    state->after_try = 1U;
}

static void sugar_dummy_event_handler(dcc_app_t *app, const dcc_event_t *event, void *user_data) {
    (void)app;
    (void)event;
    (void)user_data;
}

static void sugar_dummy_ready_handler(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data) {
    (void)app;
    (void)ready;
    (void)user_data;
}

static void sugar_dummy_message_handler(
    dcc_app_t *app,
    const dcc_message_t *message,
    const dcc_event_t *event,
    void *user_data
) {
    (void)app;
    (void)message;
    (void)event;
    (void)user_data;
}

static void sugar_dummy_message_command_handler(
    dcc_app_t *app,
    const dcc_message_t *message,
    const char *args,
    const dcc_event_t *event,
    void *user_data
) {
    (void)app;
    (void)message;
    (void)args;
    (void)event;
    (void)user_data;
}

DCC_PREFIX_COMMAND_FN(sugar_prefix_command_alias_handler) {
    (void)app;
    (void)message;
    (void)args;
    (void)event;
    (void)user_data;
}

static void sugar_dummy_task_handler(dcc_app_t *app, void *user_data) {
    (void)app;
    (void)user_data;
}

static void sugar_guard_compile_smoke(dcc_ctx_t *ctx) {
    dcc_snowflake_t roles[] = { 1ULL, 2ULL };
    if (ctx != NULL) {
        DCC_REQUIRE(ctx, 1, "blocked");
        DCC_CTX_REQUIRE(ctx, 1, "blocked");
        DCC_REQUIRE_BAD_INPUT(ctx, 1, "Invalid input.");
        DCC_CTX_REQUIRE_BAD_INPUT(ctx, 1, "Invalid input.");
        DCC_REQUIRE_BAD_INPUT_F(ctx, 1, "Invalid input %d.", 1);
        DCC_CTX_REQUIRE_BAD_INPUT_F(ctx, 1, "Invalid input %d.", 1);
        DCC_REQUIRE_DENY(ctx, 1, "Permission denied.");
        DCC_CTX_REQUIRE_DENY(ctx, 1, "Permission denied.");
        DCC_REQUIRE_DENY_F(ctx, 1, "Permission denied %d.", 1);
        DCC_CTX_REQUIRE_DENY_F(ctx, 1, "Permission denied %d.", 1);
        DCC_REQUIRE_NOT_FOUND(ctx, 1, "Not found.");
        DCC_CTX_REQUIRE_NOT_FOUND(ctx, 1, "Not found.");
        DCC_REQUIRE_NOT_FOUND_F(ctx, 1, "Not found %d.", 1);
        DCC_CTX_REQUIRE_NOT_FOUND_F(ctx, 1, "Not found %d.", 1);
        DCC_REQUIRE_BUSY(ctx, 1, "Please wait.");
        DCC_CTX_REQUIRE_BUSY(ctx, 1, "Please wait.");
        DCC_REQUIRE_BUSY_F(ctx, 1, "Please wait %d.", 1);
        DCC_CTX_REQUIRE_BUSY_F(ctx, 1, "Please wait %d.", 1);
        DCC_REQUIRE_FAIL(ctx, 1, "Failed.");
        DCC_CTX_REQUIRE_FAIL(ctx, 1, "Failed.");
        DCC_REQUIRE_FAIL_F(ctx, 1, "Failed %d.", 1);
        DCC_CTX_REQUIRE_FAIL_F(ctx, 1, "Failed %d.", 1);
        DCC_REQUIRE_INTERNAL(ctx, 1);
        DCC_CTX_REQUIRE_INTERNAL(ctx, 1);
        DCC_REQUIRE_GUILD(ctx, "Guild only.");
        DCC_CTX_REQUIRE_GUILD(ctx, "Guild only.");
        DCC_REQUIRE_DM(ctx, "DM only.");
        DCC_CTX_REQUIRE_DM(ctx, "DM only.");
        DCC_REQUIRE_AUTHOR(ctx, 1ULL, "Author only.");
        DCC_CTX_REQUIRE_AUTHOR(ctx, 1ULL, "Author only.");
        DCC_REQUIRE_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_MESSAGES, "Missing permissions.");
        DCC_CTX_REQUIRE_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_MESSAGES, "Missing permissions.");
        DCC_CTX_REQUIRE_CAN(ctx, DCC_PERMISSION_MANAGE_MESSAGES, "Missing permissions.");
        DCC_CTX_REQUIRE_CAN_ANY(ctx, DCC_PERMISSION_MODERATION_BASIC, "Missing permissions.");
        DCC_REQUIRE_ROLE(ctx, 1ULL, "Missing role.");
        DCC_CTX_REQUIRE_ROLE(ctx, 1ULL, "Missing role.");
        DCC_REQUIRE_ANY_ROLE(ctx, roles, DCC_ARRAY_LEN(roles), "Missing role.");
        DCC_CTX_REQUIRE_ANY_ROLE(ctx, roles, DCC_ARRAY_LEN(roles), "Missing role.");
        DCC_REQUIRE_ALL_ROLES(ctx, roles, DCC_ARRAY_LEN(roles), "Missing role.");
        DCC_CTX_REQUIRE_ALL_ROLES(ctx, roles, DCC_ARRAY_LEN(roles), "Missing role.");
        DCC_REQUIRE_ANY_ROLE_IDS(ctx, "Missing role.", 1ULL, 2ULL);
        DCC_CTX_REQUIRE_ANY_ROLE_IDS(ctx, "Missing role.", 1ULL, 2ULL);
        DCC_REQUIRE_ALL_ROLE_IDS(ctx, "Missing role.", 1ULL, 2ULL);
        DCC_CTX_REQUIRE_ALL_ROLE_IDS(ctx, "Missing role.", 1ULL, 2ULL);
        DCC_CTX_REQUIRE_TEXT(ctx, "value", "Missing text.");
        DCC_CTX_REQUIRE_OPTION(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_FIELD(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_SELECTED(ctx, "Choose at least one option.");
        DCC_CTX_REQUIRED_GUILD(ctx);
        DCC_CTX_REQUIRED_DM(ctx);
        DCC_CTX_REQUIRED_AUTHOR(ctx, 1ULL);
        DCC_CTX_REQUIRED_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_MESSAGES);
        DCC_CTX_REQUIRED_ROLE(ctx, 1ULL);
        DCC_CTX_REQUIRED_ANY_ROLE(ctx, roles, DCC_ARRAY_LEN(roles));
        DCC_CTX_REQUIRED_ALL_ROLES(ctx, roles, DCC_ARRAY_LEN(roles));
        DCC_CTX_REQUIRED_ANY_ROLE_IDS(ctx, 1ULL, 2ULL);
        DCC_CTX_REQUIRED_ALL_ROLE_IDS(ctx, 1ULL, 2ULL);
        DCC_CTX_REQUIRED_TEXT(ctx, "value");
        DCC_CTX_REQUIRED_OPTION(ctx, "name");
        DCC_CTX_REQUIRED_ARG(ctx, "name");
        DCC_CTX_REQUIRED_ARG_TEXT(ctx, "name");
        DCC_CTX_REQUIRED_ARG_STRING(ctx, "name");
        DCC_CTX_REQUIRED_FIELD(ctx, "field");
        DCC_CTX_REQUIRED_FORM(ctx, "field");
        DCC_CTX_REQUIRED_FORM_TEXT(ctx, "field");
        DCC_CTX_REQUIRED_FORM_STRING(ctx, "field");
        DCC_CTX_REQUIRED_SELECTED(ctx);
        DCC_CTX_REQUIRED_SELECT(ctx);
        DCC_CTX_REQUIRE_ARG_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_ARG_STRING_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_OPTION_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_OPTION_TEXT_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_OPTION_STRING_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_STRING_OPTION(required_name, ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_TEXT_OPTION(required_text, ctx, "text", "Missing option.");
        DCC_CTX_REQUIRE_INT_OPTION(required_limit, ctx, "limit", "Missing option.");
        DCC_CTX_REQUIRE_INTEGER_OPTION(required_count, ctx, "count", "Missing option.");
        DCC_CTX_REQUIRE_NUMBER_OPTION(required_score, ctx, "score", "Missing option.");
        DCC_CTX_REQUIRE_BOOL_OPTION(required_public, ctx, "public", "Missing option.");
        DCC_CTX_REQUIRE_BOOLEAN_OPTION(required_enabled, ctx, "enabled", "Missing option.");
        DCC_CTX_REQUIRE_SNOWFLAKE_OPTION(required_id, ctx, "id", "Missing option.");
        DCC_CTX_REQUIRE_USER_OPTION(required_user, ctx, "user", "Missing option.");
        DCC_CTX_REQUIRE_MEMBER_OPTION(required_member, ctx, "member", "Missing option.");
        DCC_CTX_REQUIRE_ROLE_OPTION(required_role, ctx, "role", "Missing option.");
        DCC_CTX_REQUIRE_CHANNEL_OPTION(required_channel, ctx, "channel", "Missing option.");
        DCC_CTX_REQUIRE_MESSAGE_OPTION(required_message, ctx, "message", "Missing option.");
        DCC_CTX_REQUIRE_ATTACHMENT_OPTION(required_attachment, ctx, "attachment", "Missing option.");
        DCC_CTX_REQUIRED_STRING_OPTION(default_name, ctx, "name");
        DCC_CTX_REQUIRED_TEXT_OPTION(default_text, ctx, "text");
        DCC_CTX_REQUIRED_INT_OPTION(default_limit, ctx, "limit");
        DCC_CTX_REQUIRED_INTEGER_OPTION(default_count, ctx, "count");
        DCC_CTX_REQUIRED_NUMBER_OPTION(default_score, ctx, "score");
        DCC_CTX_REQUIRED_BOOL_OPTION(default_public, ctx, "public");
        DCC_CTX_REQUIRED_BOOLEAN_OPTION(default_enabled, ctx, "enabled");
        DCC_CTX_REQUIRED_SNOWFLAKE_OPTION(default_id, ctx, "id");
        DCC_CTX_REQUIRED_USER_OPTION(default_user, ctx, "user");
        DCC_CTX_REQUIRED_MEMBER_OPTION(default_member, ctx, "member");
        DCC_CTX_REQUIRED_ROLE_OPTION(default_role, ctx, "role");
        DCC_CTX_REQUIRED_CHANNEL_OPTION(default_channel, ctx, "channel");
        DCC_CTX_REQUIRED_MESSAGE_OPTION(default_message, ctx, "message");
        DCC_CTX_REQUIRED_ATTACHMENT_OPTION(default_attachment, ctx, "attachment");
        (void)required_name;
        (void)required_text;
        (void)required_limit;
        (void)required_count;
        (void)required_score;
        (void)required_public;
        (void)required_enabled;
        (void)required_id;
        (void)required_user;
        (void)required_member;
        (void)required_role;
        (void)required_channel;
        (void)required_message;
        (void)required_attachment;
        (void)default_name;
        (void)default_text;
        (void)default_limit;
        (void)default_count;
        (void)default_score;
        (void)default_public;
        (void)default_enabled;
        (void)default_id;
        (void)default_user;
        (void)default_member;
        (void)default_role;
        (void)default_channel;
        (void)default_message;
        (void)default_attachment;
        DCC_CTX_REQUIRE_FORM_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_FORM_STRING_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_FIELD_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_FIELD_TEXT_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_FIELD_STRING_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_STRING_FIELD(required_field, ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_TEXT_FIELD(required_field_text, ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_STRING_FORM(required_form, ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_TEXT_FORM(required_form_text, ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_BOOL_FIELD(required_field_bool, ctx, "visible", "Missing field.");
        DCC_CTX_REQUIRE_BOOLEAN_FIELD(required_field_boolean, ctx, "visible", "Missing field.");
        DCC_CTX_REQUIRE_CHECKBOX_FIELD(required_field_checkbox, ctx, "visible", "Missing field.");
        DCC_CTX_REQUIRE_BOOL_FORM(required_form_bool, ctx, "visible", "Missing field.");
        DCC_CTX_REQUIRE_BOOLEAN_FORM(required_form_boolean, ctx, "visible", "Missing field.");
        DCC_CTX_REQUIRE_CHECKBOX_FORM(required_form_checkbox, ctx, "visible", "Missing field.");
        DCC_CTX_REQUIRE_VALUES_FIELD(required_values, required_value_count, ctx, "choices", "Choose a value.");
        DCC_CTX_REQUIRE_VALUES_FORM(required_form_values, required_form_value_count, ctx, "choices", "Choose a value.");
        DCC_CTX_REQUIRE_FIRST_VALUE_FIELD(required_first_value, ctx, "choices", "Choose a value.");
        DCC_CTX_REQUIRE_FIRST_VALUE_FORM(required_form_first_value, ctx, "choices", "Choose a value.");
        DCC_CTX_REQUIRED_STRING_FIELD(default_field, ctx, "field");
        DCC_CTX_REQUIRED_TEXT_FIELD(default_field_text, ctx, "field");
        DCC_CTX_REQUIRED_STRING_FORM(default_form, ctx, "field");
        DCC_CTX_REQUIRED_TEXT_FORM(default_form_text, ctx, "field");
        DCC_CTX_REQUIRED_BOOL_FIELD(default_field_bool, ctx, "visible");
        DCC_CTX_REQUIRED_BOOLEAN_FIELD(default_field_boolean, ctx, "visible");
        DCC_CTX_REQUIRED_CHECKBOX_FIELD(default_field_checkbox, ctx, "visible");
        DCC_CTX_REQUIRED_BOOL_FORM(default_form_bool, ctx, "visible");
        DCC_CTX_REQUIRED_BOOLEAN_FORM(default_form_boolean, ctx, "visible");
        DCC_CTX_REQUIRED_CHECKBOX_FORM(default_form_checkbox, ctx, "visible");
        DCC_CTX_REQUIRED_VALUES_FIELD(default_values, default_value_count, ctx, "choices");
        DCC_CTX_REQUIRED_VALUES_FORM(default_form_values, default_form_value_count, ctx, "choices");
        DCC_CTX_REQUIRED_FIRST_VALUE_FIELD(default_first_value, ctx, "choices");
        DCC_CTX_REQUIRED_FIRST_VALUE_FORM(default_form_first_value, ctx, "choices");
        (void)required_field;
        (void)required_field_text;
        (void)required_form;
        (void)required_form_text;
        (void)required_field_bool;
        (void)required_field_boolean;
        (void)required_field_checkbox;
        (void)required_form_bool;
        (void)required_form_boolean;
        (void)required_form_checkbox;
        (void)required_values;
        (void)required_value_count;
        (void)required_form_values;
        (void)required_form_value_count;
        (void)required_first_value;
        (void)required_form_first_value;
        (void)default_field;
        (void)default_field_text;
        (void)default_form;
        (void)default_form_text;
        (void)default_field_bool;
        (void)default_field_boolean;
        (void)default_field_checkbox;
        (void)default_form_bool;
        (void)default_form_boolean;
        (void)default_form_checkbox;
        (void)default_values;
        (void)default_value_count;
        (void)default_form_values;
        (void)default_form_value_count;
        (void)default_first_value;
        (void)default_form_first_value;
        DCC_CTX_REQUIRE_SELECT_BAD_INPUT(ctx, "Choose at least one option.");
        DCC_CTX_REQUIRE_SELECTED_FIRST(required_selected_first, ctx, "Choose a value.");
        DCC_CTX_REQUIRE_SELECT_FIRST(required_select_first, ctx, "Choose a value.");
        DCC_CTX_REQUIRE_SELECTED_VALUE(required_selected_value, ctx, 0U, "Choose a value.");
        DCC_CTX_REQUIRE_SELECT_VALUE(required_select_value, ctx, 0U, "Choose a value.");
        DCC_CTX_REQUIRE_SELECTED_VALUES(required_selected_values, required_selected_count, ctx, "Choose a value.");
        DCC_CTX_REQUIRE_SELECT_VALUES(required_select_values, required_select_count, ctx, "Choose a value.");
        DCC_CTX_REQUIRED_SELECTED_FIRST(default_selected_first, ctx);
        DCC_CTX_REQUIRED_SELECT_FIRST(default_select_first, ctx);
        DCC_CTX_REQUIRED_SELECTED_VALUE(default_selected_value, ctx, 0U);
        DCC_CTX_REQUIRED_SELECT_VALUE(default_select_value, ctx, 0U);
        DCC_CTX_REQUIRED_SELECTED_VALUES(default_selected_values, default_selected_count, ctx);
        DCC_CTX_REQUIRED_SELECT_VALUES(default_select_values, default_select_count, ctx);
        (void)required_selected_first;
        (void)required_select_first;
        (void)required_selected_value;
        (void)required_select_value;
        (void)required_selected_values;
        (void)required_selected_count;
        (void)required_select_values;
        (void)required_select_count;
        (void)default_selected_first;
        (void)default_select_first;
        (void)default_selected_value;
        (void)default_select_value;
        (void)default_selected_values;
        (void)default_selected_count;
        (void)default_select_values;
        (void)default_select_count;
        DCC_CTX_REQUIRE_CUSTOM_ID(ctx, "custom.id", "Wrong component.");
        DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX(ctx, "custom.", "Wrong component.");
        DCC_CTX_REQUIRED_CUSTOM_ID(ctx, "custom.id");
        DCC_CTX_REQUIRED_CUSTOM_ID_PREFIX(ctx, "custom.");
        DCC_CTX_REQUIRE_CUSTOM_ID_BAD_INPUT(ctx, "custom.id", "Wrong component.");
        DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX_BAD_INPUT(ctx, "custom.", "Wrong component.");
        DCC_CTX_REQUIRE_CUSTOM_ID_VALUE(required_custom_id, ctx, "Wrong component.");
        DCC_CTX_REQUIRE_CUSTOM_ID_SUFFIX(required_custom_suffix, ctx, "custom.", "Wrong component.");
        DCC_CTX_REQUIRE_ACTION(required_action, ctx, "custom.", "Wrong component.");
        DCC_CTX_REQUIRE_COMPONENT_ACTION(required_component_action, ctx, "custom", "Wrong component.");
        DCC_CTX_REQUIRE_ACTION_IN_NS(required_action_ns, ctx, "custom", "Wrong component.");
        DCC_CTX_REQUIRE_ACTION_IN_NAMESPACE(required_action_namespace, ctx, "custom", "Wrong component.");
        DCC_CTX_REQUIRE_COMPONENT_IS(ctx, "custom", "id", "Wrong component.");
        DCC_CTX_REQUIRED_CUSTOM_ID_VALUE(default_custom_id, ctx);
        DCC_CTX_REQUIRED_CUSTOM_ID_SUFFIX(default_custom_suffix, ctx, "custom.");
        DCC_CTX_REQUIRED_ACTION(default_action, ctx, "custom.");
        DCC_CTX_REQUIRED_COMPONENT_ACTION(default_component_action, ctx, "custom");
        DCC_CTX_REQUIRED_ACTION_IN_NS(default_action_ns, ctx, "custom");
        DCC_CTX_REQUIRED_ACTION_IN_NAMESPACE(default_action_namespace, ctx, "custom");
        DCC_CTX_REQUIRED_COMPONENT_IS(ctx, "custom", "id");
        (void)required_custom_id;
        (void)required_custom_suffix;
        (void)required_action;
        (void)required_component_action;
        (void)required_action_ns;
        (void)required_action_namespace;
        (void)default_custom_id;
        (void)default_custom_suffix;
        (void)default_action;
        (void)default_component_action;
        (void)default_action_ns;
        (void)default_action_namespace;
        DCC_REQUIRED_GUILD(ctx);
        DCC_REQUIRED_DM(ctx);
        DCC_REQUIRED_AUTHOR(ctx, 1ULL);
        DCC_REQUIRED_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_MESSAGES);
        DCC_REQUIRED_ANY_ROLE(ctx, roles, DCC_ARRAY_LEN(roles));
        DCC_REQUIRED_ALL_ROLES(ctx, roles, DCC_ARRAY_LEN(roles));
        DCC_REQUIRED_ANY_ROLE_IDS(ctx, 1ULL, 2ULL);
        DCC_REQUIRED_ALL_ROLE_IDS(ctx, 1ULL, 2ULL);
        DCC_REQUIRE_TEXT(ctx, "value", "Missing text.");
        DCC_REQUIRED_TEXT(ctx, "value");
        DCC_REQUIRE_CAN(ctx, DCC_PERMISSION_MANAGE_MESSAGES, "Missing permissions.");
        DCC_REQUIRE_CAN_ANY(ctx, DCC_PERMISSION_MODERATION_BASIC, "Missing permissions.");
        DCC_REQUIRE_OPTION(ctx, "name", "Missing option.");
        DCC_REQUIRED_OPTION(ctx, "name");
        DCC_REQUIRED_ARG(ctx, "name");
        DCC_REQUIRED_ARG_TEXT(ctx, "name");
        DCC_REQUIRED_ARG_STRING(ctx, "name");
        DCC_REQUIRE_FIELD(ctx, "field", "Missing field.");
        DCC_REQUIRED_FIELD(ctx, "field");
        DCC_REQUIRED_FORM(ctx, "field");
        DCC_REQUIRED_FORM_TEXT(ctx, "field");
        DCC_REQUIRED_FORM_STRING(ctx, "field");
        DCC_REQUIRE_SELECTED(ctx, "Choose at least one option.");
        DCC_REQUIRED_SELECTED(ctx);
        DCC_REQUIRED_SELECT(ctx);
        DCC_REQUIRE_ARG_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_REQUIRE_ARG_TEXT_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_REQUIRE_ARG_STRING_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_REQUIRE_OPTION_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_REQUIRE_OPTION_TEXT_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_REQUIRE_OPTION_STRING_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_REQUIRE_STRING_OPTION(required_name_alias, ctx, "name", "Missing option.");
        DCC_REQUIRE_TEXT_OPTION(required_text_alias, ctx, "text", "Missing option.");
        DCC_REQUIRE_INT_OPTION(required_limit_alias, ctx, "limit", "Missing option.");
        DCC_REQUIRE_INTEGER_OPTION(required_count_alias, ctx, "count", "Missing option.");
        DCC_REQUIRE_NUMBER_OPTION(required_score_alias, ctx, "score", "Missing option.");
        DCC_REQUIRE_BOOL_OPTION(required_public_alias, ctx, "public", "Missing option.");
        DCC_REQUIRE_BOOLEAN_OPTION(required_enabled_alias, ctx, "enabled", "Missing option.");
        DCC_REQUIRE_SNOWFLAKE_OPTION(required_id_alias, ctx, "id", "Missing option.");
        DCC_REQUIRE_USER_OPTION(required_user_alias, ctx, "user", "Missing option.");
        DCC_REQUIRE_MEMBER_OPTION(required_member_alias, ctx, "member", "Missing option.");
        DCC_REQUIRE_ROLE_OPTION(required_role_alias, ctx, "role", "Missing option.");
        DCC_REQUIRE_CHANNEL_OPTION(required_channel_alias, ctx, "channel", "Missing option.");
        DCC_REQUIRE_MESSAGE_OPTION(required_message_alias, ctx, "message", "Missing option.");
        DCC_REQUIRE_ATTACHMENT_OPTION(required_attachment_alias, ctx, "attachment", "Missing option.");
        DCC_REQUIRED_STRING_OPTION(default_name_alias, ctx, "name");
        DCC_REQUIRED_TEXT_OPTION(default_text_alias, ctx, "text");
        DCC_REQUIRED_INT_OPTION(default_limit_alias, ctx, "limit");
        DCC_REQUIRED_INTEGER_OPTION(default_count_alias, ctx, "count");
        DCC_REQUIRED_NUMBER_OPTION(default_score_alias, ctx, "score");
        DCC_REQUIRED_BOOL_OPTION(default_public_alias, ctx, "public");
        DCC_REQUIRED_BOOLEAN_OPTION(default_enabled_alias, ctx, "enabled");
        DCC_REQUIRED_SNOWFLAKE_OPTION(default_id_alias, ctx, "id");
        DCC_REQUIRED_USER_OPTION(default_user_alias, ctx, "user");
        DCC_REQUIRED_MEMBER_OPTION(default_member_alias, ctx, "member");
        DCC_REQUIRED_ROLE_OPTION(default_role_alias, ctx, "role");
        DCC_REQUIRED_CHANNEL_OPTION(default_channel_alias, ctx, "channel");
        DCC_REQUIRED_MESSAGE_OPTION(default_message_alias, ctx, "message");
        DCC_REQUIRED_ATTACHMENT_OPTION(default_attachment_alias, ctx, "attachment");
        (void)required_name_alias;
        (void)required_text_alias;
        (void)required_limit_alias;
        (void)required_count_alias;
        (void)required_score_alias;
        (void)required_public_alias;
        (void)required_enabled_alias;
        (void)required_id_alias;
        (void)required_user_alias;
        (void)required_member_alias;
        (void)required_role_alias;
        (void)required_channel_alias;
        (void)required_message_alias;
        (void)required_attachment_alias;
        (void)default_name_alias;
        (void)default_text_alias;
        (void)default_limit_alias;
        (void)default_count_alias;
        (void)default_score_alias;
        (void)default_public_alias;
        (void)default_enabled_alias;
        (void)default_id_alias;
        (void)default_user_alias;
        (void)default_member_alias;
        (void)default_role_alias;
        (void)default_channel_alias;
        (void)default_message_alias;
        (void)default_attachment_alias;
        DCC_REQUIRE_FORM_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_REQUIRE_FORM_TEXT_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_REQUIRE_FORM_STRING_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_REQUIRE_FIELD_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_REQUIRE_FIELD_TEXT_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_REQUIRE_FIELD_STRING_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_REQUIRE_STRING_FIELD(required_field_alias, ctx, "field", "Missing field.");
        DCC_REQUIRE_TEXT_FIELD(required_field_text_alias, ctx, "field", "Missing field.");
        DCC_REQUIRE_STRING_FORM(required_form_alias, ctx, "field", "Missing field.");
        DCC_REQUIRE_TEXT_FORM(required_form_text_alias, ctx, "field", "Missing field.");
        DCC_REQUIRE_BOOL_FIELD(required_field_bool_alias, ctx, "visible", "Missing field.");
        DCC_REQUIRE_BOOLEAN_FIELD(required_field_boolean_alias, ctx, "visible", "Missing field.");
        DCC_REQUIRE_CHECKBOX_FIELD(required_field_checkbox_alias, ctx, "visible", "Missing field.");
        DCC_REQUIRE_BOOL_FORM(required_form_bool_alias, ctx, "visible", "Missing field.");
        DCC_REQUIRE_BOOLEAN_FORM(required_form_boolean_alias, ctx, "visible", "Missing field.");
        DCC_REQUIRE_CHECKBOX_FORM(required_form_checkbox_alias, ctx, "visible", "Missing field.");
        DCC_REQUIRE_VALUES_FIELD(required_values_alias, required_value_count_alias, ctx, "choices", "Choose a value.");
        DCC_REQUIRE_VALUES_FORM(
            required_form_values_alias,
            required_form_value_count_alias,
            ctx,
            "choices",
            "Choose a value."
        );
        DCC_REQUIRE_FIRST_VALUE_FIELD(required_first_value_alias, ctx, "choices", "Choose a value.");
        DCC_REQUIRE_FIRST_VALUE_FORM(required_form_first_value_alias, ctx, "choices", "Choose a value.");
        DCC_REQUIRED_STRING_FIELD(default_field_alias, ctx, "field");
        DCC_REQUIRED_TEXT_FIELD(default_field_text_alias, ctx, "field");
        DCC_REQUIRED_STRING_FORM(default_form_alias, ctx, "field");
        DCC_REQUIRED_TEXT_FORM(default_form_text_alias, ctx, "field");
        DCC_REQUIRED_BOOL_FIELD(default_field_bool_alias, ctx, "visible");
        DCC_REQUIRED_BOOLEAN_FIELD(default_field_boolean_alias, ctx, "visible");
        DCC_REQUIRED_CHECKBOX_FIELD(default_field_checkbox_alias, ctx, "visible");
        DCC_REQUIRED_BOOL_FORM(default_form_bool_alias, ctx, "visible");
        DCC_REQUIRED_BOOLEAN_FORM(default_form_boolean_alias, ctx, "visible");
        DCC_REQUIRED_CHECKBOX_FORM(default_form_checkbox_alias, ctx, "visible");
        DCC_REQUIRED_VALUES_FIELD(default_values_alias, default_value_count_alias, ctx, "choices");
        DCC_REQUIRED_VALUES_FORM(default_form_values_alias, default_form_value_count_alias, ctx, "choices");
        DCC_REQUIRED_FIRST_VALUE_FIELD(default_first_value_alias, ctx, "choices");
        DCC_REQUIRED_FIRST_VALUE_FORM(default_form_first_value_alias, ctx, "choices");
        (void)required_field_alias;
        (void)required_field_text_alias;
        (void)required_form_alias;
        (void)required_form_text_alias;
        (void)required_field_bool_alias;
        (void)required_field_boolean_alias;
        (void)required_field_checkbox_alias;
        (void)required_form_bool_alias;
        (void)required_form_boolean_alias;
        (void)required_form_checkbox_alias;
        (void)required_values_alias;
        (void)required_value_count_alias;
        (void)required_form_values_alias;
        (void)required_form_value_count_alias;
        (void)required_first_value_alias;
        (void)required_form_first_value_alias;
        (void)default_field_alias;
        (void)default_field_text_alias;
        (void)default_form_alias;
        (void)default_form_text_alias;
        (void)default_field_bool_alias;
        (void)default_field_boolean_alias;
        (void)default_field_checkbox_alias;
        (void)default_form_bool_alias;
        (void)default_form_boolean_alias;
        (void)default_form_checkbox_alias;
        (void)default_values_alias;
        (void)default_value_count_alias;
        (void)default_form_values_alias;
        (void)default_form_value_count_alias;
        (void)default_first_value_alias;
        (void)default_form_first_value_alias;
        DCC_REQUIRE_SELECT_BAD_INPUT(ctx, "Choose at least one option.");
        DCC_REQUIRE_SELECTED_FIRST(required_selected_first_alias, ctx, "Choose a value.");
        DCC_REQUIRE_SELECT_FIRST(required_select_first_alias, ctx, "Choose a value.");
        DCC_REQUIRE_SELECTED_VALUE(required_selected_value_alias, ctx, 0U, "Choose a value.");
        DCC_REQUIRE_SELECT_VALUE(required_select_value_alias, ctx, 0U, "Choose a value.");
        DCC_REQUIRE_SELECTED_VALUES(
            required_selected_values_alias,
            required_selected_count_alias,
            ctx,
            "Choose a value."
        );
        DCC_REQUIRE_SELECT_VALUES(
            required_select_values_alias,
            required_select_count_alias,
            ctx,
            "Choose a value."
        );
        DCC_REQUIRED_SELECTED_FIRST(default_selected_first_alias, ctx);
        DCC_REQUIRED_SELECT_FIRST(default_select_first_alias, ctx);
        DCC_REQUIRED_SELECTED_VALUE(default_selected_value_alias, ctx, 0U);
        DCC_REQUIRED_SELECT_VALUE(default_select_value_alias, ctx, 0U);
        DCC_REQUIRED_SELECTED_VALUES(default_selected_values_alias, default_selected_count_alias, ctx);
        DCC_REQUIRED_SELECT_VALUES(default_select_values_alias, default_select_count_alias, ctx);
        (void)required_selected_first_alias;
        (void)required_select_first_alias;
        (void)required_selected_value_alias;
        (void)required_select_value_alias;
        (void)required_selected_values_alias;
        (void)required_selected_count_alias;
        (void)required_select_values_alias;
        (void)required_select_count_alias;
        (void)default_selected_first_alias;
        (void)default_select_first_alias;
        (void)default_selected_value_alias;
        (void)default_select_value_alias;
        (void)default_selected_values_alias;
        (void)default_selected_count_alias;
        (void)default_select_values_alias;
        (void)default_select_count_alias;
        DCC_REQUIRE_CUSTOM_ID(ctx, "custom.id", "Wrong component.");
        DCC_REQUIRE_CUSTOM_ID_PREFIX(ctx, "custom.", "Wrong component.");
        DCC_REQUIRED_CUSTOM_ID(ctx, "custom.id");
        DCC_REQUIRED_CUSTOM_ID_PREFIX(ctx, "custom.");
        DCC_REQUIRE_CUSTOM_ID_BAD_INPUT(ctx, "custom.id", "Wrong component.");
        DCC_REQUIRE_CUSTOM_ID_PREFIX_BAD_INPUT(ctx, "custom.", "Wrong component.");
        DCC_REQUIRE_CUSTOM_ID_VALUE(required_custom_id_alias, ctx, "Wrong component.");
        DCC_REQUIRE_CUSTOM_ID_SUFFIX(required_custom_suffix_alias, ctx, "custom.", "Wrong component.");
        DCC_REQUIRE_ACTION(required_action_alias, ctx, "custom.", "Wrong component.");
        DCC_REQUIRE_COMPONENT_ACTION(required_component_action_alias, ctx, "custom", "Wrong component.");
        DCC_REQUIRE_ACTION_IN_NS(required_action_ns_alias, ctx, "custom", "Wrong component.");
        DCC_REQUIRE_ACTION_IN_NAMESPACE(required_action_namespace_alias, ctx, "custom", "Wrong component.");
        DCC_REQUIRE_COMPONENT_IS(ctx, "custom", "id", "Wrong component.");
        DCC_REQUIRED_CUSTOM_ID_VALUE(default_custom_id_alias, ctx);
        DCC_REQUIRED_CUSTOM_ID_SUFFIX(default_custom_suffix_alias, ctx, "custom.");
        DCC_REQUIRED_ACTION(default_action_alias, ctx, "custom.");
        DCC_REQUIRED_COMPONENT_ACTION(default_component_action_alias, ctx, "custom");
        DCC_REQUIRED_ACTION_IN_NS(default_action_ns_alias, ctx, "custom");
        DCC_REQUIRED_ACTION_IN_NAMESPACE(default_action_namespace_alias, ctx, "custom");
        DCC_REQUIRED_COMPONENT_IS(ctx, "custom", "id");
        (void)required_custom_id_alias;
        (void)required_custom_suffix_alias;
        (void)required_action_alias;
        (void)required_component_action_alias;
        (void)required_action_ns_alias;
        (void)required_action_namespace_alias;
        (void)default_custom_id_alias;
        (void)default_custom_suffix_alias;
        (void)default_action_alias;
        (void)default_component_action_alias;
        (void)default_action_ns_alias;
        (void)default_action_namespace_alias;
    }
}

static dcc_status_t sugar_dummy_module_setup(dcc_app_t *app, void *user_data) {
    (void)app;
    (void)user_data;
    return DCC_OK;
}

static void sugar_dummy_cleanup(void *user_data) {
    (void)user_data;
}

typedef struct sugar_field_args {
    const char *name;
    int64_t limit;
    double ratio;
    uint8_t public_flag;
    dcc_snowflake_t user_id;
    const dcc_user_t *user;
    const dcc_member_t *member;
    const dcc_role_t *role;
    const dcc_channel_t *channel;
    const dcc_message_t *message;
    const dcc_attachment_t *attachment;
    const char *birthday;
    uint8_t visible;
    const char **tags;
    size_t tag_count;
} sugar_field_args_t;

#define SUGAR_FIELD_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, name, "name", "Name") \
    PARAM(type, INT, limit, "limit", "Limit", 0U, 10)

#define SUGAR_MODAL_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, birthday, "birthday")

#define SUGAR_COMPONENT_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_FIRST_VALUE, birthday) \
    PARAM(type, REQUIRED_VALUES, tags, tag_count)

static int run_sugar_context_display_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t client_options = DCC_CLIENT_OPTIONS("token", DCC_INTENTS_DEFAULT);
    client_options.enable_cache = 1U;
    if (dcc_client_create(&client_options, &client) != DCC_OK || client == NULL) {
        fprintf(stderr, "failed to create client for sugar context display smoke\n");
        return 1;
    }

    dcc_guild_t guild = {
        .id = 100ULL,
        .name = "Guild Name",
        .icon = "guild_icon",
    };
    dcc_channel_t channel = {
        .id = 200ULL,
        .guild_id = 100ULL,
        .name = "general",
    };
    dcc_role_t role = {
        .id = 300ULL,
        .guild_id = 100ULL,
        .name = "admin",
    };

    int failed = dcc_cache_put_guild(client, &guild) != DCC_OK ||
        dcc_cache_put_channel(client, &channel) != DCC_OK ||
        dcc_cache_put_role(client, &role) != DCC_OK;

    dcc_user_t users[] = {
        {
            .id = 10ULL,
            .username = "author",
            .global_name = "Author Global",
            .avatar = "author_avatar",
        },
        {
            .id = 11ULL,
            .username = "target",
            .global_name = "Target Global",
            .avatar = "target_avatar",
        },
        {
            .id = 12ULL,
            .username = "option",
            .global_name = "Option Global",
            .avatar = "option_avatar",
        },
    };
    dcc_member_t members[] = {
        {
            .guild_id = 100ULL,
            .user = users[0],
            .nick = "Author Nick",
            .avatar = "author_member_avatar",
        },
        {
            .guild_id = 100ULL,
            .user = users[1],
        },
        {
            .guild_id = 100ULL,
            .user = users[2],
            .nick = "Option Nick",
            .avatar = "option_member_avatar",
        },
    };
    dcc_interaction_option_t options[] = {
        {
            .name = "user",
            .value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE,
            .snowflake_value = 12ULL,
        },
        {
            .name = "role",
            .value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE,
            .snowflake_value = 300ULL,
        },
        {
            .name = "channel",
            .value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE,
            .snowflake_value = 200ULL,
        },
    };
    dcc_interaction_t interaction = {
        .guild_id = 100ULL,
        .channel_id = 200ULL,
        .user_id = 10ULL,
        .target_id = 11ULL,
        .options = options,
        .options_count = DCC_ARRAY_LEN(options),
        .resolved = {
            .users = users,
            .users_count = DCC_ARRAY_LEN(users),
            .members = members,
            .members_count = DCC_ARRAY_LEN(members),
            .roles = &role,
            .roles_count = 1U,
            .channels = &channel,
            .channels_count = 1U,
        },
    };
    dcc_ctx_t ctx = {
        .client = client,
        .interaction = &interaction,
    };

    if (!failed) {
        failed =
            strcmp(DCC_USER_USERNAME(&users[0]), "author") != 0 ||
            strcmp(DCC_USER_GLOBAL_NAME(&users[0]), "Author Global") != 0 ||
            strcmp(DCC_USER_DISPLAY_NAME(&users[0]), "Author Global") != 0 ||
            strcmp(DCC_USER_DISPLAY_NAME_OR(NULL, "fallback"), "fallback") != 0 ||
            strcmp(DCC_MEMBER_NICK(&members[0]), "Author Nick") != 0 ||
            strcmp(DCC_MEMBER_DISPLAY_NAME(&members[0]), "Author Nick") != 0 ||
            strcmp(DCC_MEMBER_DISPLAY_NAME(&members[1]), "Target Global") != 0 ||
            strcmp(DCC_GUILD_NAME(&guild), "Guild Name") != 0 ||
            strcmp(DCC_CHANNEL_NAME(&channel), "general") != 0 ||
            strcmp(DCC_ROLE_NAME(&role), "admin") != 0 ||
            strcmp(DCC_CTX_GUILD_NAME(&ctx), "Guild Name") != 0 ||
            strcmp(DCC_CTX_CHANNEL_NAME(&ctx), "general") != 0 ||
            strcmp(DCC_CTX_AUTHOR_USERNAME(&ctx), "author") != 0 ||
            strcmp(DCC_CTX_AUTHOR_GLOBAL_NAME(&ctx), "Author Global") != 0 ||
            strcmp(DCC_CTX_MEMBER_NICK(&ctx), "Author Nick") != 0 ||
            strcmp(DCC_CTX_AUTHOR_DISPLAY_NAME(&ctx), "Author Nick") != 0 ||
            strcmp(DCC_CTX_TARGET_USERNAME(&ctx), "target") != 0 ||
            strcmp(DCC_CTX_TARGET_DISPLAY_NAME(&ctx), "Target Global") != 0 ||
            strcmp(DCC_CTX_TARGET_ROLE_NAME_OR(&ctx, "missing"), "missing") != 0 ||
            strcmp(DCC_CTX_ARG_USERNAME(&ctx, "user"), "option") != 0 ||
            strcmp(DCC_CTX_ARG_USER_DISPLAY_NAME(&ctx, "user"), "Option Nick") != 0 ||
            strcmp(DCC_CTX_ARG_MEMBER_DISPLAY_NAME(&ctx, "user"), "Option Nick") != 0 ||
            strcmp(DCC_CTX_ARG_ROLE_NAME(&ctx, "role"), "admin") != 0 ||
            strcmp(DCC_CTX_ARG_CHANNEL_NAME(&ctx, "channel"), "general") != 0 ||
            strcmp(
                DCC_CTX_AUTHOR_AVATAR_URL_EXT(&ctx, "webp", 256U),
                DCC_CDN_BASE_URL
                    "/guilds/100/users/10/avatars/author_member_avatar.webp?size=256"
            ) != 0 ||
            strcmp(
                DCC_CTX_TARGET_AVATAR_URL_EXT(&ctx, "png", 128U),
                DCC_CDN_BASE_URL "/avatars/11/target_avatar.png?size=128"
            ) != 0 ||
            strcmp(
                DCC_CTX_ARG_USER_AVATAR_URL_EXT(&ctx, "user", "png", 64U),
                DCC_CDN_BASE_URL
                    "/guilds/100/users/12/avatars/option_member_avatar.png?size=64"
            ) != 0 ||
            strcmp(
                DCC_CTX_GUILD_ICON_URL_EXT(&ctx, "png", 512U),
                DCC_CDN_BASE_URL "/icons/100/guild_icon.png?size=512"
            ) != 0 ||
            strcmp(DCC_CTX_GUILD_NAME_OR(NULL, "fallback"), "fallback") != 0 ||
            strcmp(DCC_CTX_AUTHOR_DISPLAY_NAME_OR(NULL, "fallback"), "fallback") != 0 ||
            strcmp(DCC_CTX_ARG_USER_DISPLAY_NAME_OR(NULL, "user", "fallback"), "fallback") != 0 ||
            strcmp(DCC_CTX_ARG_ROLE_NAME_OR(NULL, "role", "fallback"), "fallback") != 0 ||
            strcmp(DCC_CTX_ARG_CHANNEL_NAME_OR(NULL, "channel", "fallback"), "fallback") != 0 ||
            strcmp(DCC_CTX_AUTHOR_AVATAR_URL(NULL), "") != 0 ||
            strcmp(DCC_CTX_GUILD_ICON_URL(NULL), "") != 0;
    }

    dcc_client_destroy(client);
    if (failed) {
        fprintf(stderr, "sugar context display helper mismatch\n");
        return 1;
    }
    return 0;
}

static int run_sugar_reply_smoke(void) {
    dcc_modal_builder_t modal =
        DCC_MODAL("reply.modal", "Reply modal", DCC_ACTION_ROW(DCC_MODAL_TEXT_INPUT("name", "Name", 1U)));
    dcc_modal_flow_t modal_flow =
        DCC_MODAL_FLOW(
            "reply.flow",
            "Reply flow",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            NULL,
            DCC_NO_FORM_ARGS(),
            DCC_NO_VALIDATORS(),
            DCC_ACTION_ROW(DCC_MODAL_TEXT_INPUT("name", "Name", 1U))
        );
    dcc_autocomplete_builder_t autocomplete =
        DCC_AUTOCOMPLETE_CHOICES(DCC_AUTOCOMPLETE_STRING_CHOICE("One", "one"));
    dcc_ctx_validation_error_t validation_error = {0};

    return (DCC_REPLY(NULL, DCC_MESSAGE_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_CB(NULL, DCC_MESSAGE_TEXT("pong"), NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_TEXT(NULL, "pong") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_TEXT_CB(NULL, "pong", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_PUBLIC(NULL, "pong") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_PUBLIC_CB(NULL, "pong", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_PUBLIC(NULL, "pong") == DCC_ERR_INVALID_ARG &&
        DCC_PUBLIC_CB(NULL, "pong", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_EPHEMERAL(NULL, "secret") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_EPHEMERAL_CB(NULL, "secret", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_EPHEMERAL(NULL, "secret") == DCC_ERR_INVALID_ARG &&
        DCC_EPHEMERAL_CB(NULL, "secret", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_EMBED(NULL, "Title", "Body", 0x57F287U, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_EMBED_CB(NULL, "Title", "Body", 0x57F287U, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_PUBLIC_EMBED(NULL, "Title", "Body", 0x57F287U) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_PUBLIC_EMBED_CB(NULL, "Title", "Body", 0x57F287U, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_REPLY_EPHEMERAL_EMBED(NULL, "Title", "Body", 0x57F287U) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_EPHEMERAL_EMBED_CB(NULL, "Title", "Body", 0x57F287U, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_REPLY_ERROR(NULL, "Error", "Body") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_ERROR_CB(NULL, "Error", "Body", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_ERROR(NULL, "Error", "Body") == DCC_ERR_INVALID_ARG &&
        DCC_ERROR_CB(NULL, "Error", "Body", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_VALIDATION_ERROR(NULL, DCC_ERR_INVALID_ARG) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_VALIDATION_ERROR_CB(NULL, DCC_ERR_INVALID_ARG, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_VALIDATION_RESULT(NULL, DCC_ERR_INVALID_ARG, &validation_error) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_VALIDATION_RESULT_CB(NULL, DCC_ERR_INVALID_ARG, &validation_error, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_REPLY_SUCCESS(NULL, "Done", "Body") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_SUCCESS_CB(NULL, "Done", "Body", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_SUCCESS(NULL, "Done", "Body") == DCC_ERR_INVALID_ARG &&
        DCC_SUCCESS_CB(NULL, "Done", "Body", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_DONE(NULL, "Done") == DCC_ERR_INVALID_ARG &&
        DCC_DONE(NULL, "Done") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_FAIL(NULL, "Failed") == DCC_ERR_INVALID_ARG &&
        DCC_FAIL(NULL, "Failed") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_DEFER(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_DEFER(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_DEFER_EPHEMERAL(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_DEFER_EPHEMERAL(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATE_MESSAGE(NULL, DCC_MESSAGE_TEXT("updated")) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATE_MESSAGE_CB(NULL, DCC_MESSAGE_TEXT("updated"), NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATE(NULL, DCC_MESSAGE_TEXT("updated")) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATE_CB(NULL, DCC_MESSAGE_TEXT("updated"), NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_EDIT_ORIGINAL(NULL, DCC_MESSAGE_TEXT("edited")) == DCC_ERR_INVALID_ARG &&
        DCC_EDIT_ORIGINAL_CB(NULL, DCC_MESSAGE_TEXT("edited"), NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_EDIT(NULL, DCC_MESSAGE_TEXT("edited")) == DCC_ERR_INVALID_ARG &&
        DCC_EDIT_CB(NULL, DCC_MESSAGE_TEXT("edited"), NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_DELETE_ORIGINAL(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_DELETE_ORIGINAL_CB(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP(NULL, DCC_MESSAGE_TEXT("followup")) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_CB(NULL, DCC_MESSAGE_TEXT("followup"), NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_TEXT(NULL, "followup") == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_PUBLIC(NULL, "followup") == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_PUBLIC_CB(NULL, "followup", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_EPHEMERAL(NULL, "followup") == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_EPHEMERAL_TEXT(NULL, "followup") == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_EPHEMERAL_TEXT_CB(NULL, "followup", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_SEND(NULL, DCC_MESSAGE_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_SEND_CB(NULL, DCC_MESSAGE_TEXT("send"), NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_SEND_TEXT(NULL, "send") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_V2(NULL, DCC_V2_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_V2_CB(NULL, NULL, NULL, DCC_V2_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_EPHEMERAL_V2(NULL, DCC_V2_TEXT("secret")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_V2(NULL, DCC_V2_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_V2_CB(NULL, NULL, NULL, DCC_V2_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_EPHEMERAL_V2(NULL, DCC_V2_TEXT("secret")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_PRIVATE_V2(NULL, DCC_V2_TEXT("secret")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT(NULL, DCC_MESSAGE_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_TEXT(NULL, "pong") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_PRIVATE(NULL, "secret") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_DONE(NULL, "done") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_BAD_INPUT(NULL, "bad") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP(NULL, DCC_MESSAGE_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_TEXT(NULL, "pong") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_PRIVATE(NULL, "secret") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_DONE(NULL, "done") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_V2(NULL, DCC_V2_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_PRIVATE_V2(NULL, DCC_V2_TEXT("secret")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_V2(NULL, DCC_V2_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_UI(NULL, DCC_UI_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_PRIVATE_UI(NULL, DCC_UI_TEXT("secret")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_UI(NULL, DCC_UI_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_PRIVATE_UI(NULL, DCC_UI_TEXT("secret")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_UI(NULL, DCC_UI_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_UI(NULL, DCC_UI_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_PRIVATE_UI(NULL, DCC_UI_TEXT("secret")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_EDIT_TEXT(NULL, "pong") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_EDIT_PRIVATE(NULL, "secret") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_EDIT_UI(NULL, DCC_UI_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_FOLLOWUP_TEXT(NULL, "pong") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_FOLLOWUP_UI(NULL, DCC_UI_TEXT("pong")) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATE_V2(NULL, DCC_V2_TEXT("updated")) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATE_V2_CB(NULL, NULL, NULL, DCC_V2_TEXT("updated")) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_V2(NULL, DCC_V2_TEXT("followup")) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_V2_CB(NULL, NULL, NULL, DCC_V2_TEXT("followup")) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_EPHEMERAL_V2(NULL, DCC_V2_TEXT("followup")) == DCC_ERR_INVALID_ARG &&
        DCC_SEND_V2(NULL, DCC_V2_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL(NULL, modal) == DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL_CB(NULL, modal, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL_V1(
            NULL,
            "reply.modal.v1",
            "Reply modal",
            DCC_ACTION_ROW(DCC_MODAL_TEXT_INPUT("name", "Name", 1U))
        ) == DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL_V2(
            NULL,
            "reply.modal.v2",
            "Reply modal",
            DCC_V2_LABEL("Name", DCC_MODAL_V2_TEXT_INPUT("name", "Name", 1U))
        ) == DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL_V2_CB(
            NULL,
            "reply.modal.v2.cb",
            "Reply modal",
            NULL,
            NULL,
            DCC_V2_LABEL("Name", DCC_MODAL_V2_TEXT_INPUT("name", "Name", 1U))
        ) == DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL_FLOW(NULL, modal_flow) == DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL_FLOW_CB(NULL, modal_flow, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_AUTOCOMPLETE(NULL, autocomplete) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_AUTOCOMPLETE_CB(NULL, autocomplete, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_AUTOCOMPLETE_MATCHING(
            NULL,
            DCC_AUTOCOMPLETE_STRING_CHOICE("Feral", "feral")
        ) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_AUTOCOMPLETE_MATCHING_CB(
            NULL,
            NULL,
            NULL,
            DCC_AUTOCOMPLETE_STRING_CHOICE("Feral", "feral")
        ) == DCC_ERR_INVALID_ARG)
        ? 0
        : 1;
}

static int run_sugar_message_smoke(void) {
    dcc_embed_builder_t embed =
        DCC_EMBED_WITH_FIELDS("Status", "Gateway online", DCC_EMBED_FIELD_INLINE("Shard", "0"));
    embed.footer = DCC_EMBED_FOOTER("DCC", NULL);
    embed.has_footer = 1U;
    embed.image = DCC_EMBED_IMAGE("attachment://status.png");
    embed.has_image = 1U;
    embed.author = DCC_EMBED_AUTHOR("runtime", NULL, NULL);
    embed.has_author = 1U;
    dcc_message_builder_t message = DCC_MESSAGE_TEXT_EMBEDS("ready", embed);

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar message\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"content\":\"ready\"") ||
        require_contains(json, "\"embeds\":[{\"title\":\"Status\"") ||
        require_contains(json, "\"inline\":true") ||
        require_contains(json, "\"footer\":{\"text\":\"DCC\"") ||
        require_contains(json, "\"image\":{\"url\":\"attachment://status.png\"") ||
        require_contains(json, "\"author\":{\"name\":\"runtime\"");
    dcc_message_builder_json_free(json);
    return failed;
}

static int run_sugar_legacy_component_smoke(void) {
    dcc_component_builder_t row =
        DCC_ACTION_ROW(DCC_BUTTON_PRIMARY_BUILDER("Refresh", "status.refresh"));

    char *json = NULL;
    if (dcc_component_builder_build_json(&row, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar legacy component\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"type\":1") ||
        require_contains(json, "\"type\":2") ||
        require_contains(json, "\"style\":1") ||
        require_contains(json, "\"custom_id\":\"status.refresh\"");
    dcc_component_builder_json_free(json);
    return failed;
}

static int run_sugar_component_v2_smoke(void) {
    dcc_component_v2_builder_t container =
        DCC_V2_CONTAINER_ACCENT(
            0x5865F2U,
            DCC_V2_SECTION(
                DCC_V2_BUTTON_PRIMARY("Refresh", "status.refresh"),
                DCC_V2_TEXT("# Runtime status")
            ),
            DCC_V2_SEPARATOR_SMALL()
        );

    char *json = NULL;
    if (dcc_component_v2_builder_build_json(&container, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar component v2 container\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"type\":17") ||
        require_contains(json, "\"type\":10") ||
        require_contains(json, "\"type\":9") ||
        require_contains(json, "\"type\":14") ||
        require_contains(json, "\"accent_color\":5793266") ||
        require_contains(json, "\"custom_id\":\"status.refresh\"");
    dcc_component_v2_builder_json_free(json);
    json = NULL;
    if (failed) {
        return 1;
    }

    dcc_component_v2_builder_t row =
        DCC_V2_ACTION_ROW(
            DCC_V2_BUTTON_SUCCESS("Yes", "confirm.yes"),
            DCC_V2_BUTTON_DANGER("No", "confirm.no")
        );
    if (dcc_component_v2_builder_build_json(&row, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar component v2 action row\n");
        return 1;
    }
    failed =
        require_contains(json, "\"type\":1") ||
        require_contains(json, "\"custom_id\":\"confirm.yes\"") ||
        require_contains(json, "\"custom_id\":\"confirm.no\"");
    dcc_component_v2_builder_json_free(json);
    json = NULL;
    if (failed) {
        return 1;
    }

    dcc_message_builder_t message = DCC_MESSAGE_COMPONENTS_V2(container);
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar component v2 message\n");
        return 1;
    }
    failed =
        require_contains(json, "\"flags\":32768") ||
        require_contains(json, "\"components\":[{\"type\":17");
    dcc_message_builder_json_free(json);
    json = NULL;
    if (failed) {
        return 1;
    }

    message = DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(container);
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar ephemeral component v2 message\n");
        return 1;
    }
    failed =
        require_contains(json, "\"flags\":32832") ||
        require_contains(json, "\"components\":[{\"type\":17");
    dcc_message_builder_json_free(json);
    return failed;
}

static int run_sugar_modal_smoke(void) {
    dcc_component_builder_t birthday_row =
        DCC_ACTION_ROW(DCC_MODAL_TEXT_INPUT_PLACEHOLDER("birthday", "Birthday", "YYYYMMDD", 1U));
    dcc_modal_builder_t birthday_modal =
        DCC_MODAL("birthday-modal", "Birthday", birthday_row);

    char *json = NULL;
    if (dcc_modal_builder_build_json(&birthday_modal, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar legacy modal alias\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"custom_id\":\"birthday-modal\"") ||
        require_contains(json, "\"placeholder\":\"YYYYMMDD\"") ||
        require_contains(json, "\"required\":true");
    dcc_modal_builder_json_free(json);
    if (failed) {
        return 1;
    }

    dcc_component_v2_builder_t upload = DCC_V2_FILE_UPLOAD("upload.config");
    upload.min_values = 1U;
    upload.max_values = 2U;
    upload.required = 1U;
    upload.has_min_values = 1U;
    upload.has_max_values = 1U;
    upload.has_required = 1U;

    dcc_component_v2_builder_t label = DCC_V2_LABEL("Config files", upload);
    dcc_component_v2_builder_t birthday = DCC_V2_LABEL(
        "Birthday",
        DCC_MODAL_V2_TEXT_INPUT_PLACEHOLDER("birthday", "Birthday", "YYYYMMDD", 1U)
    );
    dcc_component_v2_builder_t private_age =
        DCC_MODAL_V2_CHECKBOX("hide_age", "Hide age", 0U);
    dcc_modal_builder_t modal =
        DCC_MODAL_V2("upload-modal", "Upload config", label, birthday, private_age);

    json = NULL;
    if (dcc_modal_builder_build_json(&modal, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar component v2 modal\n");
        return 1;
    }
    failed =
        require_contains(json, "\"type\":9") ||
        require_contains(json, "\"custom_id\":\"upload-modal\"") ||
        require_contains(json, "\"components\":[{\"type\":18") ||
        require_contains(json, "\"component\":{\"type\":19") ||
        require_contains(json, "\"placeholder\":\"YYYYMMDD\"") ||
        require_contains(json, "\"type\":23");
    dcc_modal_builder_json_free(json);
    return failed;
}

static int run_sugar_command_smoke(void) {
    dcc_application_command_builder_t command =
        DCC_SLASH_COMMAND_OPTIONS("ping", "Latency check", "[]");

    char *json = NULL;
    if (dcc_application_command_builder_build_json(&command, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar slash command\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"name\":\"ping\"") ||
        require_contains(json, "\"description\":\"Latency check\"") ||
        require_contains(json, "\"type\":1") ||
        require_contains(json, "\"options\":[]");
    dcc_application_command_builder_json_free(json);
    if (failed) {
        return 1;
    }

    dcc_application_command_builder_t search =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "search",
            "Search members",
            DCC_COMMAND_STRING_OPTION("name", "Member name", 1U),
            DCC_COMMAND_INTEGER_OPTION_RANGE("limit", "Result limit", 0U, 1, 25),
            DCC_COMMAND_CHANNEL_OPTION_TYPES("channel", "Target channel", 0U, 0U, 5U)
        );

    json = NULL;
    if (dcc_application_command_builder_build_json(&search, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar slash command options\n");
        return 1;
    }
    failed =
        require_contains(json, "\"name\":\"search\"") ||
        require_contains(json, "\"options\":[") ||
        require_contains(json, "\"name\":\"name\"") ||
        require_contains(json, "\"type\":3") ||
        require_contains(json, "\"required\":true") ||
        require_contains(json, "\"name\":\"limit\"") ||
        require_contains(json, "\"min_value\":1") ||
        require_contains(json, "\"max_value\":25") ||
        require_contains(json, "\"channel_types\":[0,5]");
    dcc_application_command_builder_json_free(json);
    if (failed) {
        return 1;
    }

    dcc_application_command_builder_t moderation =
        DCC_SLASH_GROUP(
            "moderation",
            "Moderation tools",
            DCC_CMD_GROUP(
                "admin",
                "Admin actions",
                DCC_CMD_SUB(
                    "ban",
                    "Ban a member",
                    DCC_OPT_STRING("name", "Member name", 1U)
                )
            )
        );
    json = NULL;
    if (dcc_application_command_builder_build_json(&moderation, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar slash group aliases\n");
        return 1;
    }
    failed =
        require_contains(json, "\"name\":\"moderation\"") ||
        require_contains(json, "\"type\":2") ||
        require_contains(json, "\"name\":\"admin\"") ||
        require_contains(json, "\"type\":1") ||
        require_contains(json, "\"name\":\"ban\"") ||
        require_contains(json, "\"name\":\"name\"");
    dcc_application_command_builder_json_free(json);
    if (failed) {
        return 1;
    }

    dcc_application_command_builder_t management =
        DCC_CMD_TREE(
            "management",
            "Management tools",
            DCC_CMD_SUBGROUP(
                "admin",
                "Admin actions",
                DCC_SUBCOMMAND_SCHEMA(
                    "sync",
                    "Sync commands",
                    DCC_OPTIONAL_BOOL("dry-run", "Only show the plan")
                )
            )
        );
    json = NULL;
    if (dcc_application_command_builder_build_json(&management, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar command tree aliases\n");
        return 1;
    }
    failed =
        require_contains(json, "\"name\":\"management\"") ||
        require_contains(json, "\"name\":\"admin\"") ||
        require_contains(json, "\"name\":\"sync\"") ||
        require_contains(json, "\"name\":\"dry-run\"");
    dcc_application_command_builder_json_free(json);
    if (failed) {
        return 1;
    }

    dcc_application_command_option_builder_t options[] = {
        DCC_COMMAND_STRING_OPTION_AUTOCOMPLETE("query", "Search query", 1U),
        DCC_COMMAND_NUMBER_OPTION_RANGE("score", "Minimum score", 0U, 0.5, 1.0),
    };
    json = NULL;
    if (dcc_application_command_option_builder_build_array_json(
            options,
            DCC_ARRAY_LEN(options),
            &json
        ) != DCC_OK) {
        fprintf(stderr, "failed to serialize standalone command options\n");
        return 1;
    }
    failed =
        require_contains(json, "\"autocomplete\":true") ||
        require_contains(json, "\"name\":\"score\"") ||
        require_contains(json, "\"min_value\":0.5") ||
        require_contains(json, "\"max_value\":1");
    dcc_application_command_option_builder_json_free(json);
    return failed;
}

static int run_sugar_array_compat_smoke(void) {
    dcc_embed_field_t fields[] = {
        DCC_EMBED_FIELD("Mode", "array"),
        DCC_EMBED_FIELD_INLINE("Shard", "1"),
    };
    dcc_embed_builder_t embed =
        DCC_EMBED_WITH_FIELDS_ARRAY("Compat", "Explicit arrays", fields, DCC_ARRAY_LEN(fields));
    dcc_message_builder_t message =
        DCC_MESSAGE_EMBEDS_ARRAY(&embed, 1U);

    char *json = NULL;
    if (dcc_message_builder_build_json(&message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar array compat message\n");
        return 1;
    }
    int failed =
        require_contains(json, "\"title\":\"Compat\"") ||
        require_contains(json, "\"name\":\"Mode\"") ||
        require_contains(json, "\"inline\":true");
    dcc_message_builder_json_free(json);
    if (failed) {
        return 1;
    }

    dcc_component_v2_builder_t title = DCC_V2_TEXT("array compat");
    dcc_component_v2_builder_t refresh =
        DCC_V2_BUTTON_PRIMARY("Refresh", "compat.refresh");
    dcc_component_v2_builder_t section =
        DCC_V2_SECTION_ARRAY(&title, 1U, &refresh);
    dcc_component_v2_builder_t container =
        DCC_V2_CONTAINER_ACCENT_ARRAY(&section, 1U, 0x57F287U);
    dcc_message_builder_t component_message =
        DCC_MESSAGE_COMPONENTS_V2_ARRAY(&container, 1U);

    json = NULL;
    if (dcc_message_builder_build_json(&component_message, &json) != DCC_OK) {
        fprintf(stderr, "failed to serialize sugar array compat components v2 message\n");
        return 1;
    }
    failed =
        require_contains(json, "\"flags\":32768") ||
        require_contains(json, "\"custom_id\":\"compat.refresh\"");
    dcc_message_builder_json_free(json);
    return failed;
}

static int run_sugar_options_smoke(void) {
    dcc_client_options_t client = DCC_CLIENT_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_client_options_t sharded_client =
        DCC_CLIENT_SHARDED_OPTIONS("token", DCC_INTENTS_MESSAGES, 1U, 2U);
    dcc_client_options_t inferred_client =
        DCC_CLIENT_OPTIONS_WITH_GUILD_INFERENCE("token", DCC_INTENTS_ALL);
    dcc_app_options_t app_options =
        DCC_APP_OPTIONS_WITH_GUILD_INFERENCE("token", DCC_INTENTS_ALL);
    dcc_app_options_t guild_app_options =
        DCC_APP_OPTIONS_GUILD("token", DCC_INTENTS_DEFAULT, 999ULL);
    dcc_app_options_t auto_defer_app_options =
        DCC_APP_OPTIONS_AUTO_DEFER_EPHEMERAL("token", DCC_INTENTS_DEFAULT, 1500U);
    dcc_app_options_t auto_defer_default_app_options =
        DCC_APP_OPTIONS_AUTO_DEFER_DEFAULT("token", DCC_INTENTS_DEFAULT);
    dcc_app_options_t auto_defer_private_app_options =
        DCC_APP_OPTIONS_AUTO_DEFER_PRIVATE("token", DCC_INTENTS_DEFAULT, 1750U);
    dcc_app_options_t auto_defer_private_default_app_options =
        DCC_APP_OPTIONS_AUTO_DEFER_PRIVATE_DEFAULT("token", DCC_INTENTS_DEFAULT);
    dcc_app_options_t bot_alias_options =
        DCC_APP_OPTIONS_BOT("token");
    dcc_app_options_t dev_alias_options =
        DCC_APP_OPTIONS_DEV("token");
    dcc_app_options_t dev_guild_alias_options =
        DCC_APP_OPTIONS_DEV_GUILD("token", 999ULL);
    dcc_app_command_sync_options_t command_sync =
        DCC_APP_COMMAND_SYNC(123456789ULL);
    dcc_app_command_sync_options_t command_sync_auto =
        DCC_APP_COMMAND_SYNC_AUTO();
    dcc_app_command_sync_options_t command_sync_global_auto =
        DCC_APP_COMMAND_SYNC_GLOBAL_AUTO();
    dcc_app_command_sync_options_t command_sync_guild_plan =
        DCC_APP_COMMAND_SYNC_GUILD_PLAN(123456789ULL, 987654321ULL);
    dcc_app_command_sync_options_t command_sync_guild_auto_plan =
        DCC_APP_COMMAND_SYNC_GUILD_AUTO_PLAN(987654321ULL);
    dcc_app_options_t command_sync_app_options =
        DCC_APP_OPTIONS_WITH_COMMAND_SYNC("token", DCC_INTENTS_DEFAULT, 123456789ULL);
    dcc_app_options_t auto_sync_app_options =
        DCC_APP_OPTIONS_WITH_AUTO_SYNC("token", DCC_INTENTS_DEFAULT);
    dcc_app_options_t command_sync_guild_app_options =
        DCC_APP_OPTIONS_GUILD_WITH_COMMAND_SYNC(
            "token",
            DCC_INTENTS_DEFAULT,
            123456789ULL,
            987654321ULL
        );
    dcc_status_t invalid_run_status =
        DCC_RUN_APP_OPTIONS(app_options, ((dcc_app_definition_t){0}));
    const char *bound_string = NULL;
    dcc_app_module_t module =
        DCC_APP_MODULE_WITH_CLEANUP("sugar.module", sugar_dummy_module_setup, &bound_string, sugar_dummy_cleanup);
    dcc_app_extension_middleware_t extension_middleware =
        DCC_EXTENSION_MIDDLEWARE(sugar_dummy_middleware, &bound_string);
    dcc_app_route_policy_t route_policy =
        DCC_ROUTE_POLICY(
            DCC_ROUTE_MIDDLEWARE(DCC_EXTENSION_MIDDLEWARE(sugar_dummy_middleware, &bound_string)),
            DCC_ROUTE_GUILD_ONLY(),
            DCC_ROUTE_CHANNEL_TYPES(DCC_CHANNEL_TEXT, DCC_CHANNEL_PUBLIC_THREAD),
            DCC_ROUTE_OWNERS(123ULL, 456ULL),
            DCC_ROUTE_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_MESSAGES),
            DCC_ROUTE_REQUIRE_ROLES(789ULL, 790ULL),
            DCC_ROUTE_REQUIRE_ANY_ROLE(791ULL, 792ULL),
            DCC_ROUTE_CHECK_MESSAGE(sugar_dummy_check, &bound_string, "Blocked", "This route is blocked."),
            DCC_ROUTE_COOLDOWN_USER(2U, 1000U)
        );
    dcc_app_route_policy_t dm_route_policy =
        DCC_ROUTE_POLICY(DCC_ROUTE_DM_ONLY());
    dcc_app_route_policy_t nsfw_route_policy =
        DCC_ROUTE_POLICY(DCC_ROUTE_NSFW_ONLY());
    dcc_app_extension_slash_command_t extension_slash =
        DCC_EXTENSION_SLASH("sugar_ext", "Sugar extension", sugar_dummy_handler, &bound_string);
    dcc_app_extension_slash_command_t policy_slash =
        DCC_SLASH_DATA_POLICY(
            "sugar_policy",
            "Sugar policy",
            sugar_dummy_handler,
            &bound_string,
            route_policy
        );
    dcc_app_extension_slash_command_t command_alias =
        DCC_COMMAND_DATA_POLICY(
            "sugar_command",
            "Sugar command alias",
            sugar_dummy_handler,
            &bound_string,
            route_policy
        );
    dcc_app_extension_subcommand_t extension_subcommand =
        DCC_EXTENSION_SUBCOMMAND("sugar", "admin/reload", sugar_dummy_handler, &bound_string);
    dcc_app_extension_subcommand_t policy_subcommand =
        DCC_SUBCOMMAND_DATA_POLICY("sugar", "admin/policy", sugar_dummy_handler, &bound_string, route_policy);
    dcc_app_extension_autocomplete_t extension_autocomplete =
        DCC_EXTENSION_AUTOCOMPLETE("sugar_ext", sugar_dummy_handler, &bound_string);
    dcc_app_extension_autocomplete_t policy_autocomplete =
        DCC_AUTOCOMPLETE_HANDLER_DATA_POLICY("sugar_policy", sugar_dummy_handler, &bound_string, route_policy);
    dcc_app_extension_component_route_t extension_button =
        DCC_EXTENSION_BUTTON("sugar.button", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t policy_button =
        DCC_BUTTON_HANDLER_DATA_POLICY("sugar.policy.button", sugar_dummy_handler, &bound_string, route_policy);
    dcc_app_extension_component_route_t button_alias =
        DCC_BUTTON_DATA("sugar.alias.button", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t button_prefix_alias =
        DCC_BUTTON_PREFIX_DATA("sugar.alias.button:", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t button_ns_alias =
        DCC_ON_BUTTON_NS_DATA("sugar", "ns.button", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t button_ns_prefix_alias =
        DCC_ON_BUTTON_NS_PREFIX_DATA_POLICY("sugar", "ns.page:", sugar_dummy_handler, &bound_string, route_policy);
    dcc_app_extension_component_route_t select_alias =
        DCC_SELECT_DATA("sugar.alias.select", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t select_prefix_alias =
        DCC_SELECT_PREFIX_DATA("sugar.alias.select:", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t select_ns_alias =
        DCC_ON_SELECT_NS_POLICY("sugar", "ns.select", sugar_dummy_handler, route_policy);
    dcc_app_extension_component_route_t select_ns_prefix_alias =
        DCC_ON_SELECT_NS_PREFIX_DATA("sugar", "ns.filter:", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t extension_modal =
        DCC_EXTENSION_MODAL_PREFIX("sugar.modal:", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t modal_alias =
        DCC_MODAL_ROUTE_DATA("sugar.alias.modal", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t modal_prefix_alias =
        DCC_MODAL_PREFIX_ROUTE_DATA("sugar.alias.modal:", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t modal_ns_alias =
        DCC_ON_MODAL_NS_DATA("sugar", "ns.modal", sugar_dummy_handler, &bound_string);
    dcc_app_extension_component_route_t modal_ns_prefix_alias =
        DCC_ON_MODAL_NS_PREFIX_POLICY("sugar", "ns.edit:", sugar_dummy_handler, route_policy);
    dcc_app_extension_event_t extension_event =
        DCC_EXTENSION_EVENT(DCC_EVENT_RESUMED, sugar_dummy_event_handler, &bound_string);
    dcc_app_extension_event_t extension_ready =
        DCC_EXTENSION_READY(sugar_dummy_ready_handler, &bound_string);
    dcc_app_extension_event_t extension_message =
        DCC_EXTENSION_MESSAGE_CREATE(sugar_dummy_message_handler, &bound_string);
    dcc_app_extension_message_command_t extension_message_command =
        DCC_EXTENSION_MESSAGE_COMMAND("!", "ping", sugar_dummy_message_command_handler, &bound_string);
    dcc_app_extension_message_command_t prefix_message_command =
        DCC_PREFIX_COMMAND_DATA("?", "alias", sugar_dummy_message_command_handler, &bound_string);
    dcc_app_extension_message_command_t prefix_message_command_fn =
        DCC_ON_PREFIX_COMMAND_FN("!", sugar_prefix_command_alias_handler);
    dcc_app_extension_message_command_t text_message_command_fn =
        DCC_ON_TEXT_COMMAND_FN_DATA("~", sugar_prefix_command_alias_handler, &bound_string);
    dcc_app_extension_task_t extension_task =
        DCC_EXTENSION_TASK_KST("00:00", sugar_dummy_task_handler, &bound_string);
    dcc_app_extension_t cog =
        DCC_COG(
            "sugar.cog",
            DCC_COG_MIDDLEWARE(DCC_EXTENSION_MIDDLEWARE(sugar_dummy_middleware, &bound_string)),
            DCC_COG_COMMANDS(
                DCC_ON_SLASH("cog_ping", "Cog ping", sugar_dummy_handler),
                DCC_ON_SLASH_DATA("cog_state", "Cog state", sugar_dummy_handler, &bound_string)
            ),
            DCC_COG_SUBCOMMANDS(DCC_SUBCOMMAND("cog", "admin/reload", sugar_dummy_handler)),
            DCC_COG_AUTOCOMPLETES(DCC_ON_AUTOCOMPLETE("cog_ping", sugar_dummy_handler)),
            DCC_COG_COMPONENTS(
                DCC_ON_BUTTON("cog.button", sugar_dummy_handler),
                DCC_ON_BUTTON_PREFIX_DATA("cog.page:", sugar_dummy_handler, &bound_string),
                DCC_ON_SELECT("cog.select", sugar_dummy_handler),
                DCC_ON_SELECT_PREFIX_DATA("cog.filter:", sugar_dummy_handler, &bound_string),
                DCC_ON_MODAL("cog.modal", sugar_dummy_handler),
                DCC_ON_MODAL_PREFIX_DATA("cog.edit:", sugar_dummy_handler, &bound_string)
            ),
            DCC_COG_EVENTS(
                DCC_ON_READY(sugar_dummy_ready_handler),
                DCC_ON_MESSAGE_DATA(sugar_dummy_message_handler, &bound_string),
                DCC_ON_MESSAGE_UPDATE(sugar_dummy_message_handler),
                DCC_ON_MESSAGE_DELETE_DATA(sugar_dummy_message_handler, &bound_string),
                DCC_ON_EVENT(DCC_EVENT_RESUMED, sugar_dummy_event_handler)
            ),
            DCC_COG_MESSAGE_COMMANDS(
                DCC_MESSAGE_COMMAND("!", "ping", sugar_dummy_message_command_handler),
                DCC_MESSAGE_COMMAND_DATA("?", "state", sugar_dummy_message_command_handler, &bound_string)
            ),
            DCC_COG_TASKS(
                DCC_TASK_LOOP_MS(100U, sugar_dummy_task_handler),
                DCC_TASK_LOOP_SECONDS_DATA(5U, sugar_dummy_task_handler, &bound_string),
                DCC_TASK_LOOP_KST("00:00", sugar_dummy_task_handler),
                DCC_TASK_LOOP_DAY_AT_KST_DATA(9U, 30U, sugar_dummy_task_handler, &bound_string)
            )
        );
    dcc_app_extension_t extension = DCC_APP_EXTENSION("sugar.extension");
    dcc_app_route_group_t route_group =
        DCC_APP_ROUTE_GROUP(NULL, "settings:", &bound_string);
    dcc_app_wait_filter_t wait_user_filter = DCC_WAIT_FILTER_USER(123ULL);
    dcc_app_wait_filter_t wait_component_prefix_filter =
        DCC_WAIT_FILTER_COMPONENT_PREFIX("page:");
    const char *wait_values[] = { "one", "two" };
    dcc_interaction_t wait_interaction = {
        .id = 456ULL,
        .guild_id = 789ULL,
        .channel_id = 321ULL,
        .user_id = 654ULL,
        .custom_id = "page:next",
        .values = wait_values,
        .values_count = DCC_ARRAY_LEN(wait_values)
    };
    dcc_message_t wait_message = {
        .id = 987ULL,
        .guild_id = 789ULL,
        .channel_id = 321ULL,
        .author = { .id = 654ULL },
        .content = "waited"
    };
    dcc_app_wait_result_t wait_interaction_result = {
        .size = sizeof(wait_interaction_result),
        .snapshot = { .type = DCC_EVENT_SELECT_CLICK },
        .interaction = &wait_interaction
    };
    dcc_app_wait_result_t wait_message_result = {
        .size = sizeof(wait_message_result),
        .snapshot = { .type = DCC_EVENT_MESSAGE_CREATE },
        .message = &wait_message
    };
    int64_t bound_int = 0;
    double bound_number = 0.0;
    uint8_t bound_bool = 0U;
    dcc_snowflake_t bound_snowflake = 0U;
    dcc_ctx_option_binding_t bind_string =
        DCC_CTX_OPTION_STRING("name", &bound_string, "fallback");
    dcc_ctx_option_binding_t bind_int =
        DCC_CTX_OPTION_REQUIRED_INT("limit", &bound_int);
    dcc_ctx_option_binding_t bind_number =
        DCC_CTX_OPTION_NUMBER("ratio", &bound_number, 0.25);
    dcc_ctx_option_binding_t bind_bool =
        DCC_CTX_OPTION_BOOL("public", &bound_bool, 1U);
    dcc_ctx_option_binding_t bind_snowflake =
        DCC_CTX_OPTION_SNOWFLAKE("user", &bound_snowflake, 123ULL);
    const dcc_user_t *bound_user = NULL;
    const dcc_member_t *bound_member = NULL;
    const dcc_role_t *bound_role = NULL;
    const dcc_channel_t *bound_channel = NULL;
    const dcc_message_t *bound_message = NULL;
    const dcc_attachment_t *bound_attachment = NULL;
    dcc_ctx_option_binding_t bind_user =
        DCC_CTX_BIND_REQUIRED_USER("user", &bound_user);
    dcc_ctx_option_binding_t bind_member =
        DCC_CTX_BIND_MEMBER("user", &bound_member);
    dcc_ctx_option_binding_t bind_role =
        DCC_CTX_BIND_REQUIRED_ROLE("role", &bound_role);
    dcc_ctx_option_binding_t bind_channel =
        DCC_CTX_BIND_CHANNEL("channel", &bound_channel);
    dcc_ctx_option_binding_t bind_message =
        DCC_CTX_BIND_REQUIRED_MESSAGE("message", &bound_message);
    dcc_ctx_option_binding_t bind_attachment =
        DCC_CTX_BIND_ATTACHMENT("attachment", &bound_attachment);
    const char *form_string = NULL;
    uint8_t form_bool = 0U;
    const char **form_values = NULL;
    size_t form_value_count = 0U;
    dcc_ctx_form_binding_t form_bindings[] = {
        DCC_CTX_FORM_REQUIRED_STRING("name", &form_string),
        DCC_CTX_FORM_BOOL("visible", &form_bool, 1U),
        DCC_CTX_FORM_REQUIRED_VALUES("tags", &form_values, &form_value_count),
    };
    dcc_ctx_option_field_binding_t option_field_bindings[] = {
        DCC_CTX_OPTION_FIELD_REQUIRED_STRING(sugar_field_args_t, name, "name"),
        DCC_CTX_OPTION_FIELD_INT(sugar_field_args_t, limit, "limit", 10),
        DCC_CTX_OPTION_FIELD_NUMBER(sugar_field_args_t, ratio, "ratio", 0.25),
        DCC_CTX_OPTION_FIELD_BOOL(sugar_field_args_t, public_flag, "public", 1U),
        DCC_CTX_OPTION_FIELD_SNOWFLAKE(sugar_field_args_t, user_id, "user", 123ULL),
        DCC_CTX_OPTION_FIELD_REQUIRED_USER(sugar_field_args_t, user, "user"),
        DCC_CTX_OPTION_FIELD_MEMBER(sugar_field_args_t, member, "user"),
        DCC_CTX_OPTION_FIELD_REQUIRED_ROLE(sugar_field_args_t, role, "role"),
        DCC_CTX_OPTION_FIELD_CHANNEL(sugar_field_args_t, channel, "channel"),
        DCC_CTX_OPTION_FIELD_REQUIRED_MESSAGE(sugar_field_args_t, message, "message"),
        DCC_CTX_OPTION_FIELD_ATTACHMENT(sugar_field_args_t, attachment, "attachment"),
    };
    dcc_ctx_form_field_binding_t form_field_bindings[] = {
        DCC_CTX_FORM_FIELD_REQUIRED_STRING(sugar_field_args_t, birthday, "birthday"),
        DCC_CTX_FORM_FIELD_BOOL(sugar_field_args_t, visible, "visible", 1U),
        DCC_CTX_FORM_FIELD_REQUIRED_VALUES(sugar_field_args_t, tags, tag_count, "tags"),
    };
    dcc_ctx_component_field_binding_t component_field_bindings[] = {
        DCC_CTX_COMPONENT_FIELD_REQUIRED_CUSTOM_ID_SUFFIX(sugar_field_args_t, name, "typed.button:"),
        DCC_CTX_COMPONENT_FIELD_REQUIRED_FIRST_VALUE(sugar_field_args_t, birthday),
        DCC_CTX_COMPONENT_FIELD_REQUIRED_VALUES(sugar_field_args_t, tags, tag_count),
    };
    dcc_ctx_field_validator_t validators[] = {
        DCC_VALIDATE_STRING_REQUIRED(sugar_field_args_t, name, "name"),
        DCC_VALIDATE_STRING_LENGTH(sugar_field_args_t, name, "name", 1U, 16U),
        DCC_VALIDATE_INT_RANGE(sugar_field_args_t, limit, "limit", 1, 100),
        DCC_VALIDATE_NUMBER_RANGE(sugar_field_args_t, ratio, "ratio", 0.0, 1.0),
        DCC_VALIDATE_BIRTHDAY(sugar_field_args_t, birthday, "birthday"),
    };
    sugar_field_args_t valid_args = {
        .name = "feral",
        .limit = 25,
        .ratio = 0.5,
        .birthday = "0229",
    };
    sugar_field_args_t invalid_args = valid_args;
    invalid_args.birthday = "2007";
    dcc_ctx_validation_error_t validation_error;
    dcc_ctx_validation_error_t invalid_error;
    dcc_status_t validation_status =
        DCC_CTX_VALIDATE_FIELDS(
            &valid_args,
            &validation_error,
            DCC_VALIDATE_STRING_REQUIRED(sugar_field_args_t, name, "name"),
            DCC_VALIDATE_INT_RANGE(sugar_field_args_t, limit, "limit", 1, 100),
            DCC_VALIDATE_BIRTHDAY(sugar_field_args_t, birthday, "birthday")
        );
    dcc_status_t invalid_status =
        DCC_CTX_VALIDATE_FIELDS(
            &invalid_args,
            &invalid_error,
            DCC_VALIDATE_BIRTHDAY(sugar_field_args_t, birthday, "birthday")
        );
    dcc_application_command_builder_t typed_command =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "typed",
            "Typed command",
            DCC_COMMAND_STRING_OPTION("name", "Name", 1U),
            DCC_COMMAND_INTEGER_OPTION("limit", "Limit", 0U)
        );
    dcc_ctx_option_field_binding_list_t typed_bindings =
        DCC_CTX_OPTION_FIELD_BINDINGS(
            DCC_CTX_OPTION_FIELD_REQUIRED_STRING(sugar_field_args_t, name, "name"),
            DCC_CTX_OPTION_FIELD_INT(sugar_field_args_t, limit, "limit", 10)
        );
    dcc_ctx_option_field_binding_list_t short_typed_bindings =
        DCC_ARGS(
            DCC_ARG_REQUIRED_STRING(sugar_field_args_t, name, "name"),
            DCC_ARG_INT(sugar_field_args_t, limit, "limit", 10),
            DCC_ARG_REQUIRED_USER(sugar_field_args_t, user, "user"),
            DCC_ARG_MEMBER(sugar_field_args_t, member, "user"),
            DCC_ARG_REQUIRED_ROLE(sugar_field_args_t, role, "role"),
            DCC_ARG_CHANNEL(sugar_field_args_t, channel, "channel"),
            DCC_ARG_REQUIRED_MESSAGE(sugar_field_args_t, message, "message"),
            DCC_ARG_ATTACHMENT(sugar_field_args_t, attachment, "attachment")
        );
    dcc_ctx_form_field_binding_list_t typed_form_bindings =
        DCC_CTX_FORM_FIELD_BINDINGS(
            DCC_CTX_FORM_FIELD_REQUIRED_STRING(sugar_field_args_t, birthday, "birthday")
        );
    dcc_ctx_form_field_binding_list_t short_form_bindings =
        DCC_FORM_ARGS(
            DCC_FORM_ARG_REQUIRED_STRING(sugar_field_args_t, birthday, "birthday"),
            DCC_FORM_ARG_BOOL(sugar_field_args_t, visible, "visible", 1U)
        );
    dcc_ctx_form_field_binding_list_t readable_form_bindings =
        DCC_FORM_ARGS(
            DCC_FORM_REQUIRED_TEXT(sugar_field_args_t, birthday, "birthday"),
            DCC_FORM_CHECKBOX(sugar_field_args_t, visible, "visible", 1U),
            DCC_FORM_REQUIRED_VALUES(sugar_field_args_t, tags, tag_count, "tags")
        );
    dcc_ctx_component_field_binding_list_t typed_component_bindings =
        DCC_CTX_COMPONENT_FIELD_BINDINGS(
            DCC_CTX_COMPONENT_FIELD_REQUIRED_CUSTOM_ID_SUFFIX(sugar_field_args_t, name, "typed.button:"),
            DCC_CTX_COMPONENT_FIELD_REQUIRED_FIRST_VALUE(sugar_field_args_t, birthday)
        );
    dcc_ctx_component_field_binding_list_t short_component_bindings =
        DCC_COMPONENT_ARGS(
            DCC_COMPONENT_ARG_REQUIRED_CUSTOM_ID_SUFFIX(sugar_field_args_t, name, "typed.button:"),
            DCC_COMPONENT_ARG_REQUIRED_FIRST_VALUE(sugar_field_args_t, birthday)
        );
    dcc_ctx_component_field_binding_list_t readable_component_bindings =
        DCC_COMPONENT_ARGS(
            DCC_COMPONENT_REQUIRED_ACTION(sugar_field_args_t, name, "typed.button:"),
            DCC_COMPONENT_REQUIRED_VALUE(sugar_field_args_t, birthday),
            DCC_COMPONENT_REQUIRED_VALUES(sugar_field_args_t, tags, tag_count)
        );
    dcc_ctx_field_validator_list_t typed_validators =
        DCC_CTX_FIELD_VALIDATORS(
            DCC_VALIDATE_STRING_REQUIRED(sugar_field_args_t, name, "name"),
            DCC_VALIDATE_INT_RANGE(sugar_field_args_t, limit, "limit", 1, 100)
        );
    dcc_ctx_field_validator_list_t short_validators =
        DCC_VALIDATORS(
            DCC_VALIDATE_STRING_REQUIRED(sugar_field_args_t, name, "name"),
            DCC_VALIDATE_INT_RANGE(sugar_field_args_t, limit, "limit", 1, 100)
        );
    dcc_command_route_t typed_slash =
        DCC_TYPED_SLASH(
            &typed_command,
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_bindings,
            typed_validators
        );
    dcc_command_route_t policy_typed_slash =
        DCC_TYPED_SLASH_POLICY(
            &typed_command,
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_bindings,
            typed_validators,
            route_policy
        );
    dcc_command_route_t guarded_typed_slash =
        DCC_TYPED_SLASH_GUARDED(
            &typed_command,
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_bindings,
            typed_validators,
            DCC_ONLY_GUILD(),
            DCC_CAN(DCC_PERMISSION_MANAGE_MESSAGES)
        );
    dcc_command_route_t inline_typed_slash =
        DCC_TYPED_SLASH_OPTIONS(
            "typed_inline",
            "Typed inline command",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            short_typed_bindings,
            short_validators,
            DCC_OPT_STRING("name", "Name", 1U),
            DCC_OPT_INT_RANGE("limit", "Limit", 0U, 1, 25)
        );
    dcc_command_route_t inline_typed_slash_data =
        DCC_TYPED_SLASH_OPTIONS_DATA(
            "typed_inline_data",
            "Typed inline command with data",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            short_typed_bindings,
            DCC_NO_VALIDATORS(),
            DCC_OPT_BOOL("public", "Show publicly", 0U)
        );
    dcc_command_route_t inline_typed_slash_no_options =
        DCC_TYPED_SLASH_NO_OPTIONS(
            "typed_empty",
            "Typed command without options",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            DCC_NO_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_command_route_t no_options_command_route =
        DCC_COMMAND_ROUTE_NO_OPTIONS_DATA(
            "typed_empty_route",
            "Route-first command without options",
            sugar_dummy_typed_handler,
            &bound_string
        );
    dcc_command_route_t guarded_params_slash =
        DCC_COMMAND_ROUTE_DATA_GUARDED(
            "typed_guarded_params",
            "Typed guarded params command",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_FIELD_PARAMS,
            DCC_ONLY_GUILD(),
            DCC_CAN(DCC_PERMISSION_MANAGE_MESSAGES)
        );
    dcc_application_command_builder_t params_command_fn_schema =
        DCC_COMMAND_PARAMS_FN(
            sugar_dummy_typed_handler,
            "Typed function-name schema",
            sugar_field_args_t,
            SUGAR_FIELD_PARAMS
        );
    dcc_command_route_t params_slash_fn =
        DCC_COMMAND_ROUTE_FN_DATA_GUARDED(
            sugar_dummy_typed_handler,
            "Typed function-name params command",
            sugar_field_args_t,
            &bound_string,
            SUGAR_FIELD_PARAMS,
            DCC_ONLY_GUILD()
        );
    dcc_app_route_id_t typed_route = DCC_APP_ROUTE_INVALID;
    dcc_command_route_t typed_slash_route =
        DCC_TYPED_SLASH_ROUTE(
            &typed_command,
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            &typed_route,
            typed_bindings,
            typed_validators
        );
    dcc_application_command_builder_t typed_subcommand_parent =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "typed_parent",
            "Typed parent command",
            DCC_COMMAND_SUBCOMMAND_GROUP(
                "admin",
                "Admin",
                DCC_COMMAND_SUBCOMMAND(
                    "reload",
                    "Reload",
                    DCC_COMMAND_STRING_OPTION_AUTOCOMPLETE("name", "Name", 1U)
                )
            )
        );
    dcc_app_route_id_t typed_subcommand_route_id = DCC_APP_ROUTE_INVALID;
    dcc_subcommand_route_t typed_subcommand =
        DCC_TYPED_SUBCOMMAND_BUILDER_ROUTE_POLICY(
            &typed_subcommand_parent,
            "admin/reload",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            &typed_subcommand_route_id,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(sugar_field_args_t, name, "name")
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_STRING_REQUIRED(sugar_field_args_t, name, "name")
            ),
            route_policy
        );
    dcc_subcommand_route_t named_typed_subcommand =
        DCC_TYPED_SUBCOMMAND(
            "typed_parent",
            "admin/inspect",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            NULL,
            DCC_NO_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_subcommand_route_t guarded_params_subcommand =
        DCC_SUBCOMMAND_ROUTE_DATA_GUARDED(
            "typed_parent",
            "admin/guarded",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_FIELD_PARAMS,
            DCC_HAS_ROLE(123ULL)
        );
    dcc_subcommand_route_t params_subcommand_fn =
        DCC_SUBCOMMAND_ROUTE_FN_DATA_GUARDED(
            "typed_parent",
            sugar_dummy_typed_handler,
            sugar_field_args_t,
            &bound_string,
            SUGAR_FIELD_PARAMS,
            DCC_ONLY_GUILD()
        );
    dcc_subcommand_route_t in_params_subcommand_fn =
        DCC_SUBCOMMAND_ROUTE_IN_FN_DATA_GUARDED(
            "typed_parent",
            "admin",
            sugar_dummy_typed_handler,
            sugar_field_args_t,
            &bound_string,
            SUGAR_FIELD_PARAMS,
            DCC_ONLY_GUILD()
        );
    dcc_subcommand_route_t in_params_subcommand =
        DCC_SUBCOMMAND_ROUTE_IN_DATA_POLICY(
            "typed_parent",
            "admin",
            "policy",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_FIELD_PARAMS,
            route_policy
        );
    dcc_subcommand_route_t builder_in_subcommand =
        DCC_ON_SUBCOMMAND_BUILDER_PARAMS_IN_DATA_VALIDATED(
            &typed_subcommand_parent,
            "admin",
            "sync",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_FIELD_PARAMS,
            short_validators
        );
    dcc_app_route_id_t typed_autocomplete_route_id = DCC_APP_ROUTE_INVALID;
    dcc_autocomplete_route_t typed_autocomplete =
        DCC_TYPED_AUTOCOMPLETE_BUILDER_ROUTE_POLICY(
            &typed_command,
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            &typed_autocomplete_route_id,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(sugar_field_args_t, name, "name")
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_STRING_REQUIRED(sugar_field_args_t, name, "name")
            ),
            route_policy
        );
    dcc_autocomplete_route_t named_typed_autocomplete =
        DCC_TYPED_AUTOCOMPLETE(
            "typed_parent",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            NULL,
            DCC_NO_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_autocomplete_route_t guarded_params_autocomplete =
        DCC_AUTOCOMPLETE_PARAMS_DATA_GUARDED(
            "typed_guarded_params",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_FIELD_PARAMS,
            DCC_RATE_LIMIT_USER(1U, 500U)
        );
    dcc_autocomplete_route_t params_autocomplete_fn =
        DCC_AUTOCOMPLETE_PARAMS_FN_DATA_GUARDED(
            sugar_dummy_typed_handler,
            sugar_field_args_t,
            &bound_string,
            SUGAR_FIELD_PARAMS,
            DCC_ONLY_GUILD()
        );
    dcc_autocomplete_route_t typed_subcommand_autocomplete =
        DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_BUILDER(
            &typed_subcommand_parent,
            "admin/reload",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(sugar_field_args_t, name, "name")
            ),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_autocomplete_route_t in_subcommand_autocomplete =
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_IN_DATA(
            "typed_parent",
            "admin",
            "auto",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_FIELD_PARAMS
        );
    dcc_autocomplete_route_t builder_in_subcommand_autocomplete =
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_IN_DATA(
            &typed_subcommand_parent,
            "admin",
            "complete",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_FIELD_PARAMS
        );
    dcc_modal_route_t typed_modal =
        DCC_TYPED_MODAL(
            "typed.modal",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_form_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_modal_route_t policy_typed_modal =
        DCC_TYPED_MODAL_POLICY(
            "typed.policy.modal",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_form_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS(),
            route_policy
        );
    dcc_modal_route_t guarded_typed_modal =
        DCC_TYPED_MODAL_GUARDED(
            "typed.guarded.modal",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_form_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS(),
            DCC_ONLY_DM()
        );
    dcc_modal_route_t typed_modal_ns =
        DCC_TYPED_MODAL_NS(
            "typed.ns",
            "modal",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_form_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_modal_route_t typed_modal_ns_prefix =
        DCC_TYPED_MODAL_NS_PREFIX_GUARDED(
            "typed.ns",
            "modal:",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_form_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS(),
            DCC_ONLY_GUILD()
        );
    dcc_modal_route_t params_modal_ns =
        DCC_MODAL_NS_PARAMS_DATA(
            "typed.ns",
            "modal.params",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_MODAL_PARAMS
        );
    dcc_modal_route_t params_modal_ns_prefix =
        DCC_MODAL_NS_PREFIX_PARAMS_DATA_POLICY(
            "typed.ns",
            "modal.params:",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_MODAL_PARAMS,
            route_policy
        );
    dcc_modal_route_t params_modal_ns_fn =
        DCC_MODAL_NS_PARAMS_FN_DATA(
            "typed.fn.modal",
            sugar_dummy_typed_handler,
            sugar_field_args_t,
            &bound_string,
            SUGAR_MODAL_PARAMS
        );
    dcc_modal_route_t params_modal_ns_prefix_fn =
        DCC_MODAL_NS_PREFIX_PARAMS_FN_DATA_GUARDED(
            "typed.fn.modal",
            sugar_dummy_typed_handler,
            sugar_field_args_t,
            &bound_string,
            SUGAR_MODAL_PARAMS,
            DCC_ONLY_GUILD()
        );
    dcc_modal_route_t typed_modal_prefix =
        DCC_TYPED_MODAL_PREFIX(
            "typed:",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_form_bindings,
            typed_validators
        );
    dcc_modal_flow_t params_modal_flow_ns =
        DCC_MODAL_V2_FLOW_NS_PARAMS_DATA(
            "typed.ns",
            "modal.flow",
            "Typed namespace flow",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_MODAL_PARAMS,
            DCC_V2_LABEL(
                "Birthday",
                DCC_UI_INPUT_NS("typed.ns", "birthday", "Birthday", 1U)
            )
        );
    dcc_modal_flow_t params_modal_flow_ns_fn =
        DCC_MODAL_FLOW_NS_PARAMS_FN_DATA(
            "typed.fn.flow",
            "Typed namespace function flow",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_MODAL_PARAMS,
            DCC_ACTION_ROW(DCC_MODAL_TEXT_INPUT("birthday", "Birthday", 1U))
        );
    dcc_modal_flow_t params_modal_v2_flow_ns_fn =
        DCC_MODAL_V2_FLOW_NS_PARAMS_FN_DATA(
            "typed.fn.v2.flow",
            "Typed namespace function v2 flow",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_MODAL_PARAMS,
            DCC_V2_LABEL(
                "Birthday",
                DCC_UI_INPUT_NS("typed.fn.v2.flow", "birthday", "Birthday", 1U)
            )
        );
    dcc_modal_flow_t modal_flow =
        DCC_MODAL_FLOW(
            "typed.flow",
            "Typed flow",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_form_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS(),
            DCC_ACTION_ROW(DCC_MODAL_TEXT_INPUT("birthday", "Birthday", 1U))
        );
    dcc_modal_flow_t modal_v2_flow =
        DCC_MODAL_V2_FLOW(
            "typed.v2.flow",
            "Typed v2 flow",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_form_bindings,
            typed_validators,
            DCC_V2_LABEL(
                "Birthday",
                DCC_MODAL_V2_TEXT_INPUT("birthday", "Birthday", 1U)
            )
        );
    dcc_button_route_t typed_button =
        DCC_TYPED_BUTTON_PREFIX(
            "typed.button:",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_component_bindings,
            typed_validators
        );
    dcc_button_route_t typed_button_ns =
        DCC_TYPED_BUTTON_NS(
            "typed.ns",
            "button",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_component_bindings,
            typed_validators
        );
    dcc_button_route_t typed_button_ns_prefix =
        DCC_TYPED_BUTTON_NS_PREFIX_GUARDED(
            "typed.ns",
            "button:",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            readable_component_bindings,
            typed_validators,
            DCC_HAS_ANY_ROLE(900ULL, 901ULL)
        );
    dcc_button_route_t params_button_ns =
        DCC_BUTTON_NS_PARAMS_DATA(
            "typed.ns",
            "button.params",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_COMPONENT_PARAMS
        );
    dcc_button_route_t params_button_ns_prefix =
        DCC_BUTTON_NS_PREFIX_PARAMS_DATA_POLICY(
            "typed.ns",
            "button.params:",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_COMPONENT_PARAMS,
            route_policy
        );
    dcc_button_route_t params_button_ns_fn =
        DCC_BUTTON_NS_PARAMS_FN_DATA(
            "typed.fn.button",
            sugar_dummy_typed_handler,
            sugar_field_args_t,
            &bound_string,
            SUGAR_COMPONENT_PARAMS
        );
    dcc_button_route_t params_button_ns_prefix_fn =
        DCC_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED(
            "typed.fn.button",
            sugar_dummy_typed_handler,
            sugar_field_args_t,
            &bound_string,
            SUGAR_COMPONENT_PARAMS,
            DCC_ONLY_GUILD()
        );
    dcc_button_route_t policy_typed_button =
        DCC_TYPED_BUTTON_POLICY(
            "typed.policy.button",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_component_bindings,
            typed_validators,
            route_policy
        );
    dcc_button_route_t guarded_typed_button =
        DCC_TYPED_BUTTON_PREFIX_GUARDED(
            "typed.guarded.button:",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            readable_component_bindings,
            typed_validators,
            DCC_HAS_ANY_ROLE(900ULL, 901ULL),
            DCC_RATE_LIMIT_USER(1U, 500U)
        );
    dcc_select_route_t typed_select =
        DCC_TYPED_SELECT(
            "typed.select",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_component_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_select_route_t typed_select_ns =
        DCC_TYPED_SELECT_NS(
            "typed.ns",
            "select",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_component_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_select_route_t typed_select_ns_prefix =
        DCC_TYPED_SELECT_NS_PREFIX_GUARDED(
            "typed.ns",
            "select:",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            readable_component_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS(),
            DCC_OWNER_ONLY(902ULL)
        );
    dcc_select_route_t params_select_ns =
        DCC_SELECT_NS_PARAMS_DATA(
            "typed.ns",
            "select.params",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_COMPONENT_PARAMS
        );
    dcc_select_route_t params_select_ns_prefix =
        DCC_SELECT_NS_PREFIX_PARAMS_DATA_POLICY(
            "typed.ns",
            "select.params:",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            SUGAR_COMPONENT_PARAMS,
            route_policy
        );
    dcc_select_route_t params_select_ns_fn =
        DCC_SELECT_NS_PARAMS_FN_DATA(
            "typed.fn.select",
            sugar_dummy_typed_handler,
            sugar_field_args_t,
            &bound_string,
            SUGAR_COMPONENT_PARAMS
        );
    dcc_select_route_t params_select_ns_prefix_fn =
        DCC_SELECT_NS_PREFIX_PARAMS_FN_DATA_GUARDED(
            "typed.fn.select",
            sugar_dummy_typed_handler,
            sugar_field_args_t,
            &bound_string,
            SUGAR_COMPONENT_PARAMS,
            DCC_ONLY_GUILD()
        );
    dcc_select_route_t policy_typed_select =
        DCC_TYPED_SELECT_POLICY(
            "typed.policy.select",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            typed_component_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS(),
            route_policy
        );
    dcc_select_route_t guarded_typed_select =
        DCC_TYPED_SELECT_GUARDED(
            "typed.guarded.select",
            sugar_field_args_t,
            sugar_dummy_typed_handler,
            &bound_string,
            readable_component_bindings,
            DCC_CTX_NO_FIELD_VALIDATORS(),
            DCC_OWNER_ONLY(902ULL)
        );
    dcc_app_extension_t typed_cog =
        DCC_COG(
            "sugar.typed_cog",
            DCC_COG_COMMAND_ROUTES(inline_typed_slash_no_options),
            DCC_COG_SUBCOMMAND_ROUTES(named_typed_subcommand),
            DCC_COG_AUTOCOMPLETE_ROUTES(named_typed_autocomplete),
            DCC_COG_MODAL_ROUTES(typed_modal),
            DCC_COG_BUTTON_ROUTES(typed_button),
            DCC_COG_SELECT_ROUTES(typed_select)
        );
    dcc_app_view_t app_view =
        DCC_PERSISTENT_VIEW(
            DCC_VIEW_BUTTON("app.persistent.button", sugar_dummy_handler, &bound_string),
            DCC_VIEW_SELECT_PREFIX("app.persistent.select:", sugar_dummy_handler, &bound_string)
        );
    dcc_component_session_t sugar_component_session = {
        .size = sizeof(sugar_component_session),
    };
    dcc_app_definition_t app_definition =
        DCC_APP(
            "sugar.app",
            DCC_APP_MODULES(module),
            DCC_APP_EXTENSIONS(cog, typed_cog),
            DCC_APP_COMMAND_ROUTES(typed_slash),
            DCC_APP_SUBCOMMAND_ROUTES(typed_subcommand),
            DCC_APP_AUTOCOMPLETE_ROUTES(typed_autocomplete, typed_subcommand_autocomplete),
            DCC_APP_MODAL_ROUTES(typed_modal),
            DCC_APP_BUTTON_ROUTES(typed_button),
            DCC_APP_SELECT_ROUTES(typed_select),
            DCC_APP_MESSAGE_COMMANDS(
                DCC_MESSAGE_COMMAND_DATA("!", "top", sugar_dummy_message_command_handler, &bound_string)
            ),
            DCC_APP_EVENTS(
                DCC_ON_READY_DATA(sugar_dummy_ready_handler, &bound_string),
                DCC_ON_MESSAGE_UPDATE(sugar_dummy_message_handler)
            ),
            DCC_APP_TASKS(
                DCC_TASK_LOOP_MINUTES(10U, sugar_dummy_task_handler),
                DCC_TASK_LOOP_HOURS_DATA(1U, sugar_dummy_task_handler, &bound_string)
            ),
            DCC_APP_VIEWS(app_view),
            DCC_APP_COMPONENT_SESSIONS(
                DCC_COMPONENT_SESSION_ROUTE(&sugar_component_session, sugar_dummy_handler, &bound_string)
            ),
            DCC_APP_MIDDLEWARE(extension_middleware),
            DCC_APP_GUILD_ONLY(),
            DCC_APP_CHANNEL_TYPES(DCC_CHANNEL_TEXT, DCC_CHANNEL_PUBLIC_THREAD),
            DCC_APP_OWNERS(123ULL, 456ULL),
            DCC_APP_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_MESSAGES),
            DCC_APP_REQUIRE_ROLES(789ULL, 790ULL),
            DCC_APP_REQUIRE_ANY_ROLE(791ULL, 792ULL),
            DCC_APP_CHECK_MESSAGE(sugar_dummy_check, &bound_string, "App blocked", "This app is blocked."),
            DCC_APP_COOLDOWN_USER(3U, 2000U),
            DCC_APP_AUTO_SYNC(),
            DCC_APP_AUTO_DEFER_EPHEMERAL(1500U),
            DCC_APP_DEFAULT_ERRORS(),
            DCC_APP_ERROR_HANDLER(sugar_error_handler, &bound_string)
        );
    dcc_app_definition_t auto_defer_definition =
        DCC_APP("sugar.auto_defer", DCC_APP_AUTO_DEFER_DEFAULT());
    dcc_app_definition_t auto_defer_private_definition =
        DCC_APP("sugar.auto_defer_private", DCC_APP_AUTO_DEFER_PRIVATE(1750U));
    dcc_app_definition_t dm_app_definition = DCC_APP("sugar.dm_app", DCC_APP_DM_ONLY());
    dcc_app_definition_t nsfw_app_definition = DCC_APP("sugar.nsfw_app", DCC_APP_NSFW_ONLY());
    dcc_app_definition_t dev_app_definition = DCC_APP("sugar.dev_app", DCC_APP_DEV_MODE());
    dcc_app_definition_t dev_plan_app_definition = DCC_APP("sugar.dev_plan_app", DCC_APP_DEV_MODE_PLAN_MS(2500U));
    dcc_app_view_t view =
        DCC_APP_VIEW(
            DCC_VIEW_BUTTON("app.ok", sugar_dummy_handler, NULL),
            DCC_VIEW_SELECT_PREFIX("app.select:", sugar_dummy_handler, NULL),
            DCC_VIEW_MODAL("app.modal", sugar_dummy_handler, NULL)
        );
    dcc_command_registry_options_t registry =
        DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DRY_RUN(123ULL);
    dcc_command_registry_options_t registry_delete_stale =
        DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DELETE_STALE();
    dcc_application_command_registration_options_t registration =
        DCC_APPLICATION_COMMAND_REGISTRATION_GLOBAL();
    dcc_application_command_registration_options_t guild_registration =
        DCC_APPLICATION_COMMAND_REGISTRATION_GUILD(456ULL);
    dcc_autocomplete_builder_t autocomplete =
        DCC_AUTOCOMPLETE_CHOICES(
            DCC_AUTOCOMPLETE_STRING_CHOICE("Feral", "feral"),
            DCC_AUTOCOMPLETE_INTEGER_CHOICE("Limit", 25),
            DCC_AUTOCOMPLETE_NUMBER_CHOICE("Ratio", 0.5)
        );
    dcc_component_session_options_t session_options =
        DCC_COMPONENT_SESSION_OPTIONS_USER("secret", strlen("secret"), 789ULL);
    dcc_component_session_check_t session_check =
        DCC_COMPONENT_SESSION_CHECK("dcc:v1:session:next:nonce:sig", 1000U, 789ULL, 111ULL, 222ULL);
    dcc_component_session_listener_options_t listener =
        DCC_COMPONENT_SESSION_LISTENER_OPTIONS();
    dcc_hot_reload_canary_options_t canary =
        DCC_HOT_RELOAD_CANARY_OPTIONS_DEFAULT();
    dcc_hot_reload_options_t hot_reload =
        DCC_HOT_RELOAD_ISOLATED_CANARY_OPTIONS("dcc_hot_reload_worker", canary);
    dcc_rest_firewall_options_t firewall = DCC_REST_FIREWALL_OPTIONS();
    dcc_rest_firewall_request_t firewall_request =
        DCC_REST_FIREWALL_REQUEST("POST", "/interactions/1/token/callback", 1000U, 1U);
    dcc_replay_record_t replay =
        DCC_REPLAY_GATEWAY_RECORD("READY", 1000U, "{\"op\":0}", strlen("{\"op\":0}"));
    dcc_replay_record_t interaction_replay =
        DCC_REPLAY_INTERACTION_RECORD(1001U, "{\"type\":2}", strlen("{\"type\":2}"));
    dcc_interaction_flow_t flow = DCC_FLOW_AUTO_DEFER_EPHEMERAL(NULL, NULL, 1500U);
    dcc_message_builder_t stickers = DCC_MESSAGE_STICKERS(111ULL, 222ULL);
    dcc_poll_builder_t poll =
        DCC_POLL("Deploy?", 24U, DCC_POLL_ANSWER("Now"), DCC_POLL_ANSWER("Later"));
    dcc_component_builder_t legacy_select =
        DCC_STRING_SELECT_BUILDER(
            "legacy.select",
            DCC_SELECT_OPTION("One", "one"),
            DCC_SELECT_OPTION_DEFAULT("Two", "two")
        );
    dcc_component_builder_t legacy_button_ns =
        DCC_BUTTON_PRIMARY_BUILDER_NS("Register", "birthday", "register");
    dcc_component_builder_t legacy_select_ns =
        DCC_STRING_SELECT_BUILDER_NS(
            "birthday",
            "schedule",
            DCC_SELECT_OPTION("June", "06")
        );
    dcc_component_builder_t legacy_input_ns =
        DCC_TEXT_INPUT_PLACEHOLDER_NS(
            "birthday",
            "date",
            "Birthday",
            DCC_TEXT_INPUT_SHORT,
            "YYYYMMDD",
            1U
        );
    dcc_component_v2_builder_t gallery =
        DCC_V2_MEDIA_GALLERY(
            DCC_V2_MEDIA("attachment://one.png", "One"),
            DCC_V2_MEDIA_SPOILER("attachment://two.png", "Two")
        );
    dcc_component_v2_builder_t thumbnail =
        DCC_V2_THUMBNAIL(DCC_V2_MEDIA("attachment://thumb.png", "Thumb"));
    dcc_component_v2_builder_t file =
        DCC_V2_FILE(DCC_V2_MEDIA("attachment://report.txt", "Report"));
    dcc_component_v2_builder_t string_select =
        DCC_V2_STRING_SELECT(
            "v2.string",
            DCC_SELECT_OPTION("Alpha", "alpha"),
            DCC_SELECT_OPTION("Beta", "beta")
        );
    dcc_component_v2_builder_t v2_button_ns =
        DCC_V2_BUTTON_PRIMARY_NS("Register", "birthday", "register");
    dcc_component_v2_builder_t string_select_ns =
        DCC_V2_STRING_SELECT_NS(
            "birthday",
            "schedule",
            DCC_SELECT_OPTION("June", "06")
        );
    dcc_component_v2_builder_t user_select_ns =
        DCC_V2_USER_SELECT_NS("birthday", "user");
    dcc_component_v2_builder_t channel_select =
        DCC_V2_CHANNEL_SELECT(
            "v2.channel",
            DCC_V2_DEFAULT_VALUES(DCC_V2_DEFAULT_CHANNEL(333ULL)),
            DCC_V2_CHANNEL_TYPES(0U, 5U)
        );
    dcc_component_v2_builder_t channel_select_ns =
        DCC_V2_CHANNEL_SELECT_NS(
            "birthday",
            "channel",
            DCC_V2_DEFAULT_VALUES(DCC_V2_DEFAULT_CHANNEL(333ULL)),
            DCC_V2_CHANNEL_TYPES(0U, 5U)
        );
    dcc_component_v2_builder_t radio =
        DCC_V2_RADIO_GROUP(
            "v2.radio",
            DCC_SELECT_OPTION("Fast", "fast"),
            DCC_SELECT_OPTION("Safe", "safe")
        );
    dcc_component_v2_builder_t checkbox_group =
        DCC_V2_CHECKBOX_GROUP(
            "v2.checks",
            DCC_SELECT_OPTION("Logs", "logs"),
            DCC_SELECT_OPTION("Metrics", "metrics")
        );
    dcc_component_v2_builder_t text_input_ns =
        DCC_V2_TEXT_INPUT_PLACEHOLDER_NS(
            "birthday",
            "date",
            "Birthday",
            DCC_TEXT_INPUT_SHORT,
            "YYYYMMDD",
            1U
        );
    dcc_component_v2_builder_t file_upload_ns =
        DCC_V2_FILE_UPLOAD_NS("birthday", "proof");
    dcc_component_v2_builder_t radio_ns =
        DCC_V2_RADIO_GROUP_NS("birthday", "visibility", DCC_SELECT_OPTION("Public", "public"));
    dcc_component_v2_builder_t checkbox_ns =
        DCC_V2_CHECKBOX_NS("birthday", "public_age", "Show age", 1U);
    dcc_component_v2_builder_t ui_button_ns =
        DCC_UI_PRIMARY_NS("Schedule", "birthday", "schedule");
    dcc_component_v2_builder_t ui_select_ns =
        DCC_UI_STRING_SELECT_NS("birthday", "month", DCC_SELECT_OPTION("June", "06"));
    dcc_component_v2_builder_t ui_input_ns =
        DCC_UI_INPUT_PLACEHOLDER_NS("birthday", "date", "Birthday", "YYYYMMDD", 1U);
    dcc_message_builder_t managed_message = DCC_MESSAGE_TEXT("managed");
    dcc_managed_message_options_t managed =
        DCC_MANAGED_MESSAGE_OPTIONS(333ULL, &managed_message, NULL, NULL, NULL);
    dcc_managed_message_options_t managed_keep =
        DCC_MANAGED_MESSAGE_KEEP_PREVIOUS_OPTIONS(444ULL, &managed_message, NULL, NULL, NULL);
    dcc_thread_params_t thread_params = {
        .size = sizeof(thread_params),
        .name = "sugar-thread",
    };
    dcc_channel_params_t channel_params = {
        .size = sizeof(channel_params),
        .guild_id = 222ULL,
        .channel_id = 333ULL,
        .type = DCC_CHANNEL_TEXT,
        .name = "sugar-channel",
    };
    dcc_channel_position_t channel_positions[] = {
        { .channel_id = 333ULL, .position = 1U },
    };
    dcc_channel_positions_params_t channel_positions_params = {
        .size = sizeof(channel_positions_params),
        .guild_id = 222ULL,
        .positions = channel_positions,
        .position_count = DCC_ARRAY_LEN(channel_positions),
    };
    dcc_invite_params_t invite_params = {
        .size = sizeof(invite_params),
        .channel_id = 333ULL,
        .max_age = 3600U,
        .max_uses = 5U,
        .unique = 1U,
    };
    dcc_channel_permission_overwrite_t permission_overwrite = {
        .id = 444ULL,
        .allow = DCC_PERMISSION_VIEW_CHANNEL,
        .deny = 0U,
        .type = DCC_CHANNEL_OVERWRITE_ROLE,
    };
    dcc_role_params_t role_params = {
        .size = sizeof(role_params),
        .guild_id = 222ULL,
        .role_id = 444ULL,
        .name = "sugar-role",
    };
    dcc_role_position_t role_positions[] = {
        { .role_id = 444ULL, .position = 2U },
    };
    dcc_snowflake_t prune_roles[] = { 444ULL };
    dcc_guild_ban_params_t ban_params = {
        .size = sizeof(ban_params),
        .guild_id = 222ULL,
        .user_id = 333ULL,
        .delete_message_seconds = 60U,
    };
    dcc_guild_prune_params_t prune_params = {
        .size = sizeof(prune_params),
        .guild_id = 222ULL,
        .days = 7U,
        .include_roles = prune_roles,
        .include_role_count = DCC_ARRAY_LEN(prune_roles),
        .compute_prune_count = 1U,
    };
    dcc_current_guild_member_params_t current_member_params = {
        .size = sizeof(current_member_params),
        .guild_id = 222ULL,
        .nick = "sugar-nick",
    };
    dcc_guild_member_nickname_params_t nickname_params = {
        .size = sizeof(nickname_params),
        .guild_id = 222ULL,
        .nick = "sugar-nick",
    };
    dcc_scheduled_event_params_t scheduled_event_params = {
        .size = sizeof(scheduled_event_params),
        .guild_id = 222ULL,
        .event_id = 555ULL,
        .channel_id = 333ULL,
        .name = "sugar-event",
        .scheduled_start_time = "2026-06-23T12:00:00Z",
        .privacy_level = DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY,
        .entity_type = DCC_SCHEDULED_EVENT_ENTITY_VOICE,
    };
    dcc_webhook_params_t webhook_params = {
        .size = sizeof(webhook_params),
        .webhook_id = 666ULL,
        .channel_id = 333ULL,
        .name = "sugar-webhook",
    };
    dcc_webhook_params_t webhook_token_params = {
        .size = sizeof(webhook_token_params),
        .webhook_id = 666ULL,
        .channel_id = 333ULL,
        .name = "sugar-webhook",
        .token = "token",
    };
    dcc_current_user_params_t user_params = {
        .size = sizeof(user_params),
        .nickname = "sugar-user",
    };
    dcc_dm_channel_params_t dm_params = {
        .size = sizeof(dm_params),
        .user_id = 333ULL,
    };
    dcc_stage_instance_params_t stage_params = {
        .size = sizeof(stage_params),
        .channel_id = 333ULL,
        .topic = "sugar-stage",
        .privacy_level = DCC_STAGE_PRIVACY_GUILD_ONLY,
    };
    dcc_template_params_t template_params = {
        .size = sizeof(template_params),
        .guild_id = 222ULL,
        .code = "template-code",
        .name = "sugar-template",
        .description = "Sugar template",
    };
    dcc_guild_widget_params_t widget_params = {
        .size = sizeof(widget_params),
        .channel_id = 333ULL,
        .enabled = 1U,
    };
    dcc_onboarding_params_t onboarding_params = {
        .size = sizeof(onboarding_params),
        .guild_id = 222ULL,
        .enabled = 1U,
        .mode = DCC_ONBOARDING_MODE_DEFAULT,
    };
    dcc_welcome_screen_params_t welcome_params = {
        .size = sizeof(welcome_params),
        .description = "Welcome to DCC",
        .enabled = 1U,
    };
    dcc_auto_moderation_action_params_t auto_mod_action = {
        .size = sizeof(auto_mod_action),
        .type = DCC_AUTO_MODERATION_ACTION_BLOCK_MESSAGE,
        .custom_message = "Blocked by DCC",
    };
    dcc_auto_moderation_rule_params_t auto_mod_rule_params = {
        .size = sizeof(auto_mod_rule_params),
        .rule_id = 444ULL,
        .name = "sugar-auto-mod",
        .event_type = DCC_AUTO_MODERATION_EVENT_MESSAGE_SEND,
        .trigger_type = DCC_AUTO_MODERATION_TRIGGER_SPAM,
        .actions = &auto_mod_action,
        .action_count = 1U,
        .enabled = 1U,
    };
    dcc_emoji_params_t emoji_params = {
        .size = sizeof(emoji_params),
        .emoji_id = 555ULL,
        .name = "sugar_emoji",
        .image = "data:image/png;base64,AA==",
        .roles = prune_roles,
        .role_count = DCC_ARRAY_LEN(prune_roles),
    };
    const unsigned char sticker_data[] = { 0x89U, 'P', 'N', 'G' };
    dcc_guild_sticker_params_t sticker_params = {
        .size = sizeof(sticker_params),
        .guild_id = 222ULL,
        .name = "sugar-sticker",
        .description = "Sugar sticker",
        .tags = "sugar",
        .filename = "sugar.png",
        .content_type = "image/png",
        .data = sticker_data,
        .data_len = sizeof(sticker_data),
    };
    dcc_rest_multipart_field_t sticker_fields[] = {
        { .name = "name", .value = "sugar-sticker" },
        { .name = "description", .value = "Sugar sticker" },
        { .name = "tags", .value = "sugar" },
    };
    dcc_rest_multipart_file_t sticker_file = {
        .field_name = "file",
        .filename = "sugar.png",
        .content_type = "image/png",
        .data = sticker_data,
        .data_len = sizeof(sticker_data),
    };
    dcc_message_search_params_t message_search_params = {
        .size = sizeof(message_search_params),
        .content = "sugar",
        .channel_ids = &channel_params.channel_id,
        .channel_id_count = 1U,
        .has_limit = 1U,
        .limit = 25U,
    };
    dcc_group_dm_recipient_params_t group_dm_recipient_params = {
        .size = sizeof(group_dm_recipient_params),
        .channel_id = 333ULL,
        .user_id = 444ULL,
        .access_token = "access-token",
        .nick = "sugar-group",
    };
    dcc_voice_state_params_t voice_state_params = {
        .size = sizeof(voice_state_params),
        .guild_id = 222ULL,
        .user_id = 333ULL,
        .channel_id = 444ULL,
        .suppress = 1U,
        .request_to_speak_timestamp = "2026-06-23T12:00:00Z",
    };
    dcc_test_entitlement_params_t test_entitlement_params = {
        .size = sizeof(test_entitlement_params),
        .application_id = 111ULL,
        .sku_id = 222ULL,
        .owner_id = 333ULL,
        .owner_type = 1U,
    };
    dcc_snowflake_t entitlement_sku_ids[] = { 222ULL };

    if (client.size != sizeof(client) ||
        client.intents != DCC_INTENT_GUILDS ||
        sharded_client.shard_id != 1U ||
        sharded_client.shard_count != 2U ||
        inferred_client.enable_cache != 1U ||
        inferred_client.infer_guild_id_from_channel != 1U ||
        (inferred_client.intents & DCC_INTENT_MESSAGE_CONTENT) == 0U ||
        (inferred_client.intents & DCC_INTENT_GUILD_PRESENCES) == 0U ||
        (inferred_client.intents & DCC_INTENT_DIRECT_MESSAGE_POLLS) == 0U ||
        app_options.client.enable_cache != 1U ||
        app_options.client.infer_guild_id_from_channel != 1U ||
        app_options.command_registry.guild_id != 0U ||
        guild_app_options.command_registry.guild_id != 999ULL ||
        auto_defer_app_options.auto_defer_after_ms != 1500U ||
        auto_defer_app_options.auto_defer_ephemeral != 1U ||
        auto_defer_default_app_options.auto_defer_after_ms != 1500U ||
        auto_defer_default_app_options.auto_defer_ephemeral != 0U ||
        auto_defer_private_app_options.auto_defer_after_ms != 1750U ||
        auto_defer_private_app_options.auto_defer_ephemeral != 1U ||
        auto_defer_private_default_app_options.auto_defer_after_ms != 1500U ||
        auto_defer_private_default_app_options.auto_defer_ephemeral != 1U ||
        bot_alias_options.client.enable_cache != 1U ||
        bot_alias_options.client.infer_guild_id_from_channel != 1U ||
        dev_alias_options.auto_defer_after_ms != 1500U ||
        dev_alias_options.auto_defer_ephemeral != 1U ||
        dev_alias_options.command_sync_on_ready != 1U ||
        dev_guild_alias_options.command_registry.guild_id != 999ULL ||
        dev_guild_alias_options.command_sync.command_registry.guild_id != 999ULL ||
        dev_guild_alias_options.auto_defer_ephemeral != 1U ||
        command_sync.application_id != 123456789ULL ||
        command_sync.apply != 1U ||
        command_sync.once != 1U ||
        command_sync.command_registry.guild_id != 0U ||
        command_sync_auto.application_id != 0U ||
        command_sync_auto.command_registry.size != 0U ||
        command_sync_auto.apply != 1U ||
        command_sync_auto.infer_application_id != 1U ||
        command_sync_global_auto.command_registry.size != sizeof(dcc_command_registry_options_t) ||
        command_sync_global_auto.command_registry.guild_id != 0U ||
        command_sync_global_auto.apply != 1U ||
        command_sync_global_auto.infer_application_id != 1U ||
        command_sync_guild_plan.application_id != 123456789ULL ||
        command_sync_guild_plan.apply != 0U ||
        command_sync_guild_plan.command_registry.guild_id != 987654321ULL ||
        command_sync_guild_plan.command_registry.dry_run != 1U ||
        command_sync_guild_auto_plan.application_id != 0U ||
        command_sync_guild_auto_plan.infer_application_id != 1U ||
        command_sync_guild_auto_plan.command_registry.guild_id != 987654321ULL ||
        command_sync_guild_auto_plan.command_registry.dry_run != 1U ||
        command_sync_app_options.command_sync_on_ready != 1U ||
        command_sync_app_options.command_sync.application_id != 123456789ULL ||
        auto_sync_app_options.command_sync_on_ready != 1U ||
        auto_sync_app_options.command_sync.application_id != 0U ||
        auto_sync_app_options.command_sync.command_registry.size != 0U ||
        auto_sync_app_options.command_sync.infer_application_id != 1U ||
        command_sync_guild_app_options.command_registry.guild_id != 987654321ULL ||
        command_sync_guild_app_options.command_sync.command_registry.guild_id != 987654321ULL ||
        invalid_run_status != DCC_ERR_INVALID_ARG ||
        strcmp(module.name, "sugar.module") != 0 ||
        module.setup != sugar_dummy_module_setup ||
        module.user_data != &bound_string ||
        module.cleanup != sugar_dummy_cleanup ||
        extension_middleware.middleware != sugar_dummy_middleware ||
        extension_middleware.user_data != &bound_string ||
        route_policy.size != sizeof(route_policy) ||
        route_policy.middleware_count != 1U ||
        route_policy.middlewares == NULL ||
        route_policy.middlewares[0].middleware != sugar_dummy_middleware ||
        route_policy.owner_user_id_count != 2U ||
        route_policy.owner_user_ids == NULL ||
        route_policy.owner_user_ids[0] != 123ULL ||
        route_policy.owner_user_ids[1] != 456ULL ||
        route_policy.guild_only != 1U ||
        route_policy.channel_type_count != 2U ||
        route_policy.channel_types == NULL ||
        route_policy.channel_types[0] != DCC_CHANNEL_TEXT ||
        route_policy.channel_types[1] != DCC_CHANNEL_PUBLIC_THREAD ||
        route_policy.required_permissions != DCC_PERMISSION_MANAGE_MESSAGES ||
        route_policy.required_role_id_count != 2U ||
        route_policy.required_role_ids == NULL ||
        route_policy.required_role_ids[0] != 789ULL ||
        route_policy.required_role_ids[1] != 790ULL ||
        route_policy.any_role_id_count != 2U ||
        route_policy.any_role_ids == NULL ||
        route_policy.any_role_ids[0] != 791ULL ||
        route_policy.any_role_ids[1] != 792ULL ||
        route_policy.check_count != 1U ||
        route_policy.checks == NULL ||
        route_policy.checks[0].check != sugar_dummy_check ||
        route_policy.checks[0].user_data != &bound_string ||
        strcmp(route_policy.checks[0].title, "Blocked") != 0 ||
        strcmp(route_policy.checks[0].description, "This route is blocked.") != 0 ||
        route_policy.cooldown.bucket != DCC_APP_COOLDOWN_USER ||
        route_policy.cooldown.limit != 2U ||
        route_policy.cooldown.window_ms != 1000U ||
        dm_route_policy.dm_only != 1U ||
        dm_route_policy.guild_only != 0U ||
        nsfw_route_policy.nsfw_only != 1U ||
        nsfw_route_policy.dm_only != 0U ||
        strcmp(extension_slash.name, "sugar_ext") != 0 ||
        extension_slash.handler != sugar_dummy_handler ||
        extension_slash.user_data != &bound_string ||
        strcmp(policy_slash.name, "sugar_policy") != 0 ||
        policy_slash.policy.guild_only != 1U ||
        policy_slash.policy.middleware_count != 1U ||
        strcmp(command_alias.name, "sugar_command") != 0 ||
        command_alias.handler != sugar_dummy_handler ||
        command_alias.user_data != &bound_string ||
        command_alias.policy.guild_only != 1U ||
        strcmp(extension_subcommand.command_name, "sugar") != 0 ||
        strcmp(extension_subcommand.subcommand_path, "admin/reload") != 0 ||
        extension_subcommand.handler != sugar_dummy_handler ||
        strcmp(policy_subcommand.subcommand_path, "admin/policy") != 0 ||
        policy_subcommand.policy.required_permissions != DCC_PERMISSION_MANAGE_MESSAGES ||
        strcmp(extension_autocomplete.name, "sugar_ext") != 0 ||
        policy_autocomplete.policy.owner_user_id_count != 2U ||
        extension_button.type != DCC_APP_EXTENSION_BUTTON ||
        strcmp(extension_button.custom_id, "sugar.button") != 0 ||
        policy_button.policy.guild_only != 1U ||
        button_alias.type != DCC_APP_EXTENSION_BUTTON ||
        strcmp(button_alias.custom_id, "sugar.alias.button") != 0 ||
        button_alias.user_data != &bound_string ||
        button_prefix_alias.type != DCC_APP_EXTENSION_BUTTON_PREFIX ||
        strcmp(button_prefix_alias.custom_id, "sugar.alias.button:") != 0 ||
        button_ns_alias.type != DCC_APP_EXTENSION_BUTTON ||
        strcmp(button_ns_alias.custom_id, "sugar.ns.button") != 0 ||
        button_ns_alias.user_data != &bound_string ||
        button_ns_prefix_alias.type != DCC_APP_EXTENSION_BUTTON_PREFIX ||
        strcmp(button_ns_prefix_alias.custom_id, "sugar.ns.page:") != 0 ||
        button_ns_prefix_alias.policy.guild_only != 1U ||
        select_alias.type != DCC_APP_EXTENSION_SELECT ||
        strcmp(select_alias.custom_id, "sugar.alias.select") != 0 ||
        select_prefix_alias.type != DCC_APP_EXTENSION_SELECT_PREFIX ||
        strcmp(select_prefix_alias.custom_id, "sugar.alias.select:") != 0 ||
        select_ns_alias.type != DCC_APP_EXTENSION_SELECT ||
        strcmp(select_ns_alias.custom_id, "sugar.ns.select") != 0 ||
        select_ns_alias.policy.guild_only != 1U ||
        select_ns_prefix_alias.type != DCC_APP_EXTENSION_SELECT_PREFIX ||
        strcmp(select_ns_prefix_alias.custom_id, "sugar.ns.filter:") != 0 ||
        select_ns_prefix_alias.user_data != &bound_string ||
        extension_modal.type != DCC_APP_EXTENSION_MODAL_PREFIX ||
        modal_alias.type != DCC_APP_EXTENSION_MODAL ||
        strcmp(modal_alias.custom_id, "sugar.alias.modal") != 0 ||
        modal_prefix_alias.type != DCC_APP_EXTENSION_MODAL_PREFIX ||
        strcmp(modal_prefix_alias.custom_id, "sugar.alias.modal:") != 0 ||
        modal_ns_alias.type != DCC_APP_EXTENSION_MODAL ||
        strcmp(modal_ns_alias.custom_id, "sugar.ns.modal") != 0 ||
        modal_ns_alias.user_data != &bound_string ||
        modal_ns_prefix_alias.type != DCC_APP_EXTENSION_MODAL_PREFIX ||
        strcmp(modal_ns_prefix_alias.custom_id, "sugar.ns.edit:") != 0 ||
        modal_ns_prefix_alias.policy.guild_only != 1U ||
        strcmp(DCC_COMPONENT_ID("sugar", "literal"), "sugar.literal") != 0 ||
        strcmp(DCC_COMPONENT_PREFIX("sugar"), "sugar.") != 0 ||
        strcmp(DCC_COMPONENT_ACTION_PREFIX("sugar", "literal:"), "sugar.literal:") != 0 ||
        strcmp(typed_modal_ns.custom_id, "typed.ns.modal") != 0 ||
        strcmp(typed_modal_ns_prefix.custom_id, "typed.ns.modal:") != 0 ||
        typed_modal_ns_prefix.prefix != 1U ||
        typed_modal_ns_prefix.policy.guild_only != 1U ||
        strcmp(params_modal_ns.custom_id, "typed.ns.modal.params") != 0 ||
        params_modal_ns.bindings.count != 1U ||
        strcmp(params_modal_ns_prefix.custom_id, "typed.ns.modal.params:") != 0 ||
        params_modal_ns_prefix.prefix != 1U ||
        params_modal_ns_prefix.policy.guild_only != 1U ||
        strcmp(params_modal_ns_fn.custom_id, "typed.fn.modal.sugar_dummy_typed_handler") != 0 ||
        params_modal_ns_fn.user_data != &bound_string ||
        params_modal_ns_fn.bindings.count != 1U ||
        strcmp(params_modal_ns_prefix_fn.custom_id, "typed.fn.modal.sugar_dummy_typed_handler") != 0 ||
        params_modal_ns_prefix_fn.prefix != 1U ||
        params_modal_ns_prefix_fn.policy.guild_only != 1U ||
        strcmp(params_modal_flow_ns.modal.custom_id, "typed.ns.modal.flow") != 0 ||
        strcmp(params_modal_flow_ns.route.custom_id, "typed.ns.modal.flow") != 0 ||
        params_modal_flow_ns.modal.components_v2_count != 1U ||
        strcmp(params_modal_flow_ns_fn.modal.custom_id, "typed.fn.flow.sugar_dummy_typed_handler") != 0 ||
        strcmp(params_modal_flow_ns_fn.route.custom_id, "typed.fn.flow.sugar_dummy_typed_handler") != 0 ||
        params_modal_flow_ns_fn.modal.components_count != 1U ||
        params_modal_flow_ns_fn.route.user_data != &bound_string ||
        strcmp(params_modal_v2_flow_ns_fn.modal.custom_id, "typed.fn.v2.flow.sugar_dummy_typed_handler") != 0 ||
        strcmp(params_modal_v2_flow_ns_fn.route.custom_id, "typed.fn.v2.flow.sugar_dummy_typed_handler") != 0 ||
        params_modal_v2_flow_ns_fn.modal.components_v2_count != 1U ||
        params_modal_v2_flow_ns_fn.route.user_data != &bound_string ||
        strcmp(typed_button_ns.custom_id, "typed.ns.button") != 0 ||
        strcmp(typed_button_ns_prefix.custom_id, "typed.ns.button:") != 0 ||
        typed_button_ns_prefix.prefix != 1U ||
        typed_button_ns_prefix.policy.any_role_id_count != 2U ||
        strcmp(params_button_ns.custom_id, "typed.ns.button.params") != 0 ||
        params_button_ns.bindings.count != 2U ||
        strcmp(params_button_ns_prefix.custom_id, "typed.ns.button.params:") != 0 ||
        params_button_ns_prefix.prefix != 1U ||
        params_button_ns_prefix.policy.guild_only != 1U ||
        strcmp(params_button_ns_fn.custom_id, "typed.fn.button.sugar_dummy_typed_handler") != 0 ||
        params_button_ns_fn.user_data != &bound_string ||
        params_button_ns_fn.bindings.count != 2U ||
        strcmp(params_button_ns_prefix_fn.custom_id, "typed.fn.button.sugar_dummy_typed_handler") != 0 ||
        params_button_ns_prefix_fn.prefix != 1U ||
        params_button_ns_prefix_fn.policy.guild_only != 1U ||
        strcmp(typed_select_ns.custom_id, "typed.ns.select") != 0 ||
        strcmp(typed_select_ns_prefix.custom_id, "typed.ns.select:") != 0 ||
        typed_select_ns_prefix.prefix != 1U ||
        typed_select_ns_prefix.policy.owner_user_id_count != 1U ||
        strcmp(params_select_ns.custom_id, "typed.ns.select.params") != 0 ||
        params_select_ns.bindings.count != 2U ||
        strcmp(params_select_ns_prefix.custom_id, "typed.ns.select.params:") != 0 ||
        params_select_ns_prefix.prefix != 1U ||
        params_select_ns_prefix.policy.guild_only != 1U ||
        strcmp(params_select_ns_fn.custom_id, "typed.fn.select.sugar_dummy_typed_handler") != 0 ||
        params_select_ns_fn.user_data != &bound_string ||
        params_select_ns_fn.bindings.count != 2U ||
        strcmp(params_select_ns_prefix_fn.custom_id, "typed.fn.select.sugar_dummy_typed_handler") != 0 ||
        params_select_ns_prefix_fn.prefix != 1U ||
        params_select_ns_prefix_fn.policy.guild_only != 1U ||
        extension_event.type != DCC_APP_EXTENSION_EVENT ||
        extension_event.event_type != DCC_EVENT_RESUMED ||
        extension_event.event_handler != sugar_dummy_event_handler ||
        extension_ready.type != DCC_APP_EXTENSION_READY ||
        extension_ready.ready_handler != sugar_dummy_ready_handler ||
        extension_message.type != DCC_APP_EXTENSION_MESSAGE_CREATE ||
        extension_message.message_handler != sugar_dummy_message_handler ||
        strcmp(extension_message_command.prefix, "!") != 0 ||
        strcmp(extension_message_command.name, "ping") != 0 ||
        extension_message_command.handler != sugar_dummy_message_command_handler ||
        extension_message_command.user_data != &bound_string ||
        strcmp(prefix_message_command.prefix, "?") != 0 ||
        strcmp(prefix_message_command.name, "alias") != 0 ||
        prefix_message_command.handler != sugar_dummy_message_command_handler ||
        prefix_message_command.user_data != &bound_string ||
        strcmp(prefix_message_command_fn.prefix, "!") != 0 ||
        strcmp(prefix_message_command_fn.name, "sugar_prefix_command_alias_handler") != 0 ||
        prefix_message_command_fn.handler != sugar_prefix_command_alias_handler ||
        prefix_message_command_fn.user_data != NULL ||
        strcmp(text_message_command_fn.prefix, "~") != 0 ||
        strcmp(text_message_command_fn.name, "sugar_prefix_command_alias_handler") != 0 ||
        text_message_command_fn.handler != sugar_prefix_command_alias_handler ||
        text_message_command_fn.user_data != &bound_string ||
        extension_task.type != DCC_APP_EXTENSION_TASK_EVERY_KST ||
        strcmp(extension_task.hhmm, "00:00") != 0 ||
        extension_task.handler != sugar_dummy_task_handler ||
        strcmp(cog.name, "sugar.cog") != 0 ||
        cog.middleware_count != 1U ||
        cog.middlewares == NULL ||
        cog.middlewares[0].middleware != sugar_dummy_middleware ||
        cog.slash_command_count != 2U ||
        cog.slash_commands == NULL ||
        strcmp(cog.slash_commands[0].name, "cog_ping") != 0 ||
        cog.slash_commands[0].user_data != NULL ||
        strcmp(cog.slash_commands[1].name, "cog_state") != 0 ||
        cog.slash_commands[1].user_data != &bound_string ||
        cog.subcommand_count != 1U ||
        cog.subcommands == NULL ||
        strcmp(cog.subcommands[0].subcommand_path, "admin/reload") != 0 ||
        cog.autocomplete_count != 1U ||
        cog.autocompletes == NULL ||
        strcmp(cog.autocompletes[0].name, "cog_ping") != 0 ||
        cog.component_route_count != 6U ||
        cog.component_routes == NULL ||
        cog.component_routes[0].type != DCC_APP_EXTENSION_BUTTON ||
        cog.component_routes[1].type != DCC_APP_EXTENSION_BUTTON_PREFIX ||
        cog.component_routes[1].user_data != &bound_string ||
        cog.component_routes[2].type != DCC_APP_EXTENSION_SELECT ||
        cog.component_routes[3].type != DCC_APP_EXTENSION_SELECT_PREFIX ||
        cog.component_routes[4].type != DCC_APP_EXTENSION_MODAL ||
        cog.component_routes[5].type != DCC_APP_EXTENSION_MODAL_PREFIX ||
        cog.component_routes[5].user_data != &bound_string ||
        cog.event_count != 5U ||
        cog.events == NULL ||
        cog.events[0].type != DCC_APP_EXTENSION_READY ||
        cog.events[0].user_data != NULL ||
        cog.events[1].type != DCC_APP_EXTENSION_MESSAGE_CREATE ||
        cog.events[1].user_data != &bound_string ||
        cog.events[2].type != DCC_APP_EXTENSION_MESSAGE_UPDATE ||
        cog.events[3].type != DCC_APP_EXTENSION_MESSAGE_DELETE ||
        cog.events[3].user_data != &bound_string ||
        cog.events[4].type != DCC_APP_EXTENSION_EVENT ||
        cog.events[4].event_type != DCC_EVENT_RESUMED ||
        cog.message_command_count != 2U ||
        cog.message_commands == NULL ||
        strcmp(cog.message_commands[0].prefix, "!") != 0 ||
        strcmp(cog.message_commands[0].name, "ping") != 0 ||
        cog.message_commands[0].user_data != NULL ||
        strcmp(cog.message_commands[1].prefix, "?") != 0 ||
        strcmp(cog.message_commands[1].name, "state") != 0 ||
        cog.message_commands[1].user_data != &bound_string ||
        cog.task_count != 4U ||
        cog.tasks == NULL ||
        cog.tasks[0].type != DCC_APP_EXTENSION_TASK_EVERY_MS ||
        cog.tasks[0].interval != 100U ||
        cog.tasks[1].type != DCC_APP_EXTENSION_TASK_EVERY_SECONDS ||
        cog.tasks[1].user_data != &bound_string ||
        cog.tasks[2].type != DCC_APP_EXTENSION_TASK_EVERY_KST ||
        strcmp(cog.tasks[2].hhmm, "00:00") != 0 ||
        cog.tasks[3].type != DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST ||
        cog.tasks[3].hour != 9U ||
        cog.tasks[3].minute != 30U ||
        cog.tasks[3].user_data != &bound_string ||
        strcmp(extension.name, "sugar.extension") != 0 ||
        route_group.size != sizeof(route_group) ||
        route_group.app != NULL ||
        strcmp(route_group.custom_id_prefix, "settings:") != 0 ||
        route_group.user_data != &bound_string ||
        wait_user_filter.size != sizeof(wait_user_filter) ||
        wait_user_filter.user_id != 123ULL ||
        wait_component_prefix_filter.size != sizeof(wait_component_prefix_filter) ||
        strcmp(wait_component_prefix_filter.custom_id_prefix, "page:") != 0 ||
        DCC_WAIT_FOR_BUTTON(NULL, "button", 1U, &(dcc_app_wait_result_t){0}) !=
            DCC_ERR_INVALID_ARG ||
        DCC_COLLECT_SELECT(NULL, "select", 1U, &(dcc_app_wait_result_t){0}) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_WAIT_FOR_COMPONENT(NULL, 1U, &(dcc_app_wait_result_t){0}) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_WAIT_FOR_MESSAGE_CREATE(NULL, 1U, &(dcc_app_wait_result_t){0}) !=
            DCC_ERR_INVALID_ARG ||
        DCC_WAIT_TYPE(&wait_interaction_result) != DCC_EVENT_SELECT_CLICK ||
        DCC_WAIT_ID(&wait_interaction_result) != 456ULL ||
        DCC_WAIT_USER_ID(&wait_interaction_result) != 654ULL ||
        DCC_WAIT_CHANNEL_ID(&wait_interaction_result) != 321ULL ||
        DCC_WAIT_GUILD_ID(&wait_interaction_result) != 789ULL ||
        strcmp(DCC_WAIT_CUSTOM_ID(&wait_interaction_result), "page:next") != 0 ||
        DCC_WAIT_VALUE_COUNT(&wait_interaction_result) != 2U ||
        strcmp(DCC_WAIT_VALUE(&wait_interaction_result, 1U), "two") != 0 ||
        DCC_WAIT_VALUE(&wait_interaction_result, 2U) != NULL ||
        DCC_WAIT_TYPE(&wait_message_result) != DCC_EVENT_MESSAGE_CREATE ||
        DCC_WAIT_ID(&wait_message_result) != 987ULL ||
        strcmp(DCC_WAIT_MESSAGE_CONTENT(&wait_message_result, ""), "waited") != 0 ||
        DCC_WAIT_CUSTOM_ID(&wait_message_result) != NULL ||
        bind_string.type != DCC_CTX_OPTION_BIND_STRING ||
        bind_string.out != &bound_string ||
        strcmp(bind_string.fallback_string, "fallback") != 0 ||
        bind_int.type != DCC_CTX_OPTION_BIND_INTEGER ||
        bind_int.required != 1U ||
        bind_int.out != &bound_int ||
        bind_number.type != DCC_CTX_OPTION_BIND_NUMBER ||
        bind_number.fallback_number < 0.24 ||
        bind_number.fallback_number > 0.26 ||
        bind_bool.type != DCC_CTX_OPTION_BIND_BOOLEAN ||
        bind_bool.fallback_boolean != 1U ||
        bind_snowflake.type != DCC_CTX_OPTION_BIND_SNOWFLAKE ||
        bind_snowflake.fallback_snowflake != 123ULL ||
        bind_user.type != DCC_CTX_OPTION_BIND_USER ||
        bind_user.required != 1U ||
        bind_user.out != &bound_user ||
        bind_member.type != DCC_CTX_OPTION_BIND_MEMBER ||
        bind_member.required != 0U ||
        bind_member.out != &bound_member ||
        bind_role.type != DCC_CTX_OPTION_BIND_ROLE ||
        bind_role.required != 1U ||
        bind_channel.type != DCC_CTX_OPTION_BIND_CHANNEL ||
        bind_message.type != DCC_CTX_OPTION_BIND_MESSAGE ||
        bind_message.required != 1U ||
        bind_attachment.type != DCC_CTX_OPTION_BIND_ATTACHMENT ||
        DCC_CTX_HAS_ROLE(NULL, 1ULL) != 0U ||
        DCC_CTX_HAS_ANY_ROLE_IDS(NULL, 1ULL, 2ULL) != 0U ||
        DCC_CTX_HAS_ALL_ROLE_IDS(NULL, 1ULL, 2ULL) != 0U ||
        form_bindings[0].type != DCC_CTX_FORM_BIND_STRING ||
        form_bindings[0].required != 1U ||
        form_bindings[0].out != &form_string ||
        form_bindings[1].type != DCC_CTX_FORM_BIND_BOOLEAN ||
        form_bindings[1].fallback_boolean != 1U ||
        form_bindings[2].type != DCC_CTX_FORM_BIND_VALUES ||
        form_bindings[2].out_count != &form_value_count ||
        option_field_bindings[0].type != DCC_CTX_OPTION_BIND_STRING ||
        option_field_bindings[0].required != 1U ||
        option_field_bindings[0].field_offset != offsetof(sugar_field_args_t, name) ||
        option_field_bindings[1].fallback_integer != 10 ||
        option_field_bindings[2].fallback_number < 0.24 ||
        option_field_bindings[2].fallback_number > 0.26 ||
        option_field_bindings[3].fallback_boolean != 1U ||
        option_field_bindings[4].fallback_snowflake != 123ULL ||
        option_field_bindings[5].type != DCC_CTX_OPTION_BIND_USER ||
        option_field_bindings[5].required != 1U ||
        option_field_bindings[5].field_offset != offsetof(sugar_field_args_t, user) ||
        option_field_bindings[6].type != DCC_CTX_OPTION_BIND_MEMBER ||
        option_field_bindings[6].required != 0U ||
        option_field_bindings[7].type != DCC_CTX_OPTION_BIND_ROLE ||
        option_field_bindings[7].required != 1U ||
        option_field_bindings[8].type != DCC_CTX_OPTION_BIND_CHANNEL ||
        option_field_bindings[9].type != DCC_CTX_OPTION_BIND_MESSAGE ||
        option_field_bindings[9].required != 1U ||
        option_field_bindings[10].type != DCC_CTX_OPTION_BIND_ATTACHMENT ||
        form_field_bindings[0].type != DCC_CTX_FORM_BIND_STRING ||
        form_field_bindings[0].field_offset != offsetof(sugar_field_args_t, birthday) ||
        form_field_bindings[1].fallback_boolean != 1U ||
        form_field_bindings[2].field_offset != offsetof(sugar_field_args_t, tags) ||
        form_field_bindings[2].count_offset != offsetof(sugar_field_args_t, tag_count) ||
        form_field_bindings[2].has_count_offset != 1U ||
        component_field_bindings[0].type != DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX ||
        component_field_bindings[0].field_offset != offsetof(sugar_field_args_t, name) ||
        strcmp(component_field_bindings[0].custom_id_prefix, "typed.button:") != 0 ||
        component_field_bindings[1].type != DCC_CTX_COMPONENT_BIND_FIRST_VALUE ||
        component_field_bindings[1].required != 1U ||
        component_field_bindings[2].field_offset != offsetof(sugar_field_args_t, tags) ||
        component_field_bindings[2].count_offset != offsetof(sugar_field_args_t, tag_count) ||
        component_field_bindings[2].has_count_offset != 1U ||
        validators[0].type != DCC_CTX_FIELD_VALIDATE_STRING_REQUIRED ||
        validators[0].field_offset != offsetof(sugar_field_args_t, name) ||
        validators[1].min_length != 1U ||
        validators[1].max_length != 16U ||
        validators[2].min_integer != 1 ||
        validators[2].max_integer != 100 ||
        validators[3].min_number != 0.0 ||
        validators[3].max_number != 1.0 ||
        validators[4].type != DCC_CTX_FIELD_VALIDATE_DATE_MMDD_OR_YYYYMMDD ||
        validation_status != DCC_OK ||
        validation_error.status != DCC_OK ||
        invalid_status != DCC_ERR_INVALID_ARG ||
        strcmp(invalid_error.field, "birthday") != 0 ||
        typed_bindings.count != 2U ||
        typed_bindings.items == NULL ||
        short_typed_bindings.count != 8U ||
        short_typed_bindings.items == NULL ||
        short_typed_bindings.items[0].required != 1U ||
        short_typed_bindings.items[1].fallback_integer != 10 ||
        short_typed_bindings.items[2].type != DCC_CTX_OPTION_BIND_USER ||
        short_typed_bindings.items[2].required != 1U ||
        short_typed_bindings.items[3].type != DCC_CTX_OPTION_BIND_MEMBER ||
        short_typed_bindings.items[4].type != DCC_CTX_OPTION_BIND_ROLE ||
        short_typed_bindings.items[4].required != 1U ||
        short_typed_bindings.items[5].type != DCC_CTX_OPTION_BIND_CHANNEL ||
        short_typed_bindings.items[6].type != DCC_CTX_OPTION_BIND_MESSAGE ||
        short_typed_bindings.items[6].required != 1U ||
        short_typed_bindings.items[7].type != DCC_CTX_OPTION_BIND_ATTACHMENT ||
        typed_form_bindings.count != 1U ||
        short_form_bindings.count != 2U ||
        short_form_bindings.items[1].fallback_boolean != 1U ||
        readable_form_bindings.count != 3U ||
        readable_form_bindings.items[0].required != 1U ||
        readable_form_bindings.items[1].type != DCC_CTX_FORM_BIND_BOOLEAN ||
        readable_form_bindings.items[2].type != DCC_CTX_FORM_BIND_VALUES ||
        typed_component_bindings.count != 2U ||
        short_component_bindings.count != 2U ||
        short_component_bindings.items[0].type != DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX ||
        readable_component_bindings.count != 3U ||
        readable_component_bindings.items[0].type != DCC_CTX_COMPONENT_BIND_CUSTOM_ID_SUFFIX ||
        readable_component_bindings.items[1].type != DCC_CTX_COMPONENT_BIND_FIRST_VALUE ||
        readable_component_bindings.items[2].has_count_offset != 1U ||
        typed_validators.count != 2U ||
        short_validators.count != 2U ||
        typed_slash.command != &typed_command ||
        typed_slash.args_size != sizeof(sugar_field_args_t) ||
        typed_slash.bindings.count != 2U ||
        typed_slash.validators.count != 2U ||
        typed_slash.handler != sugar_dummy_typed_handler ||
        typed_slash.user_data != &bound_string ||
        policy_typed_slash.policy.required_permissions != DCC_PERMISSION_MANAGE_MESSAGES ||
        guarded_typed_slash.policy.guild_only != 1U ||
        guarded_typed_slash.policy.required_permissions != DCC_PERMISSION_MANAGE_MESSAGES ||
        inline_typed_slash.command == NULL ||
        strcmp(inline_typed_slash.command->name, "typed_inline") != 0 ||
        inline_typed_slash.command->options_count != 2U ||
        inline_typed_slash.command->options[0].type != DCC_APPLICATION_COMMAND_OPTION_STRING ||
        inline_typed_slash.command->options[0].required != 1U ||
        inline_typed_slash.command->options[1].type != DCC_APPLICATION_COMMAND_OPTION_INTEGER ||
        inline_typed_slash.command->options[1].min_integer_value != 1 ||
        inline_typed_slash.command->options[1].max_integer_value != 25 ||
        inline_typed_slash.bindings.count != short_typed_bindings.count ||
        inline_typed_slash.validators.count != 2U ||
        inline_typed_slash.user_data != NULL ||
        inline_typed_slash_data.command == NULL ||
        strcmp(inline_typed_slash_data.command->name, "typed_inline_data") != 0 ||
        inline_typed_slash_data.command->options_count != 1U ||
        inline_typed_slash_data.command->options[0].type != DCC_APPLICATION_COMMAND_OPTION_BOOLEAN ||
        inline_typed_slash_data.user_data != &bound_string ||
        inline_typed_slash_data.validators.count != 0U ||
        inline_typed_slash_no_options.command == NULL ||
        strcmp(inline_typed_slash_no_options.command->name, "typed_empty") != 0 ||
        inline_typed_slash_no_options.command->options_count != 0U ||
        inline_typed_slash_no_options.bindings.count != 0U ||
        inline_typed_slash_no_options.validators.count != 0U ||
        no_options_command_route.command == NULL ||
        strcmp(no_options_command_route.command->name, "typed_empty_route") != 0 ||
        no_options_command_route.command->options_count != 0U ||
        no_options_command_route.args_size != sizeof(dcc_no_args_t) ||
        no_options_command_route.bindings.count != 0U ||
        no_options_command_route.validators.count != 0U ||
        no_options_command_route.handler != sugar_dummy_typed_handler ||
        no_options_command_route.user_data != &bound_string ||
        guarded_params_slash.command == NULL ||
        guarded_params_slash.command->options_count != 2U ||
        guarded_params_slash.bindings.count != 2U ||
        guarded_params_slash.policy.guild_only != 1U ||
        guarded_params_slash.policy.required_permissions != DCC_PERMISSION_MANAGE_MESSAGES ||
        strcmp(params_command_fn_schema.name, "sugar_dummy_typed_handler") != 0 ||
        params_command_fn_schema.options_count != 2U ||
        params_slash_fn.command == NULL ||
        strcmp(params_slash_fn.command->name, "sugar_dummy_typed_handler") != 0 ||
        params_slash_fn.command->options_count != 2U ||
        params_slash_fn.bindings.count != 2U ||
        params_slash_fn.user_data != &bound_string ||
        params_slash_fn.policy.guild_only != 1U ||
        typed_slash_route.out_route != &typed_route ||
        typed_subcommand.command != &typed_subcommand_parent ||
        typed_subcommand.command_name != NULL ||
        strcmp(typed_subcommand.subcommand_path, "admin/reload") != 0 ||
        typed_subcommand.args_size != sizeof(sugar_field_args_t) ||
        typed_subcommand.bindings.count != 1U ||
        typed_subcommand.validators.count != 1U ||
        typed_subcommand.out_route != &typed_subcommand_route_id ||
        typed_subcommand.policy.required_permissions != DCC_PERMISSION_MANAGE_MESSAGES ||
        strcmp(named_typed_subcommand.command_name, "typed_parent") != 0 ||
        strcmp(named_typed_subcommand.subcommand_path, "admin/inspect") != 0 ||
        named_typed_subcommand.bindings.count != 0U ||
        strcmp(guarded_params_subcommand.command_name, "typed_parent") != 0 ||
        strcmp(guarded_params_subcommand.subcommand_path, "admin/guarded") != 0 ||
        guarded_params_subcommand.bindings.count != 2U ||
        guarded_params_subcommand.policy.required_role_id_count != 1U ||
        guarded_params_subcommand.policy.required_role_ids[0] != 123ULL ||
        strcmp(params_subcommand_fn.command_name, "typed_parent") != 0 ||
        strcmp(params_subcommand_fn.subcommand_path, "sugar_dummy_typed_handler") != 0 ||
        params_subcommand_fn.bindings.count != 2U ||
        params_subcommand_fn.policy.guild_only != 1U ||
        strcmp(in_params_subcommand_fn.command_name, "typed_parent") != 0 ||
        strcmp(in_params_subcommand_fn.subcommand_path, "admin/sugar_dummy_typed_handler") != 0 ||
        in_params_subcommand_fn.bindings.count != 2U ||
        in_params_subcommand_fn.policy.guild_only != 1U ||
        strcmp(DCC_SUB_PATH("admin", "sync"), "admin/sync") != 0 ||
        strcmp(DCC_CMD_PATH("admin", "sync"), "admin/sync") != 0 ||
        strcmp(in_params_subcommand.command_name, "typed_parent") != 0 ||
        strcmp(in_params_subcommand.subcommand_path, "admin/policy") != 0 ||
        in_params_subcommand.bindings.count != 2U ||
        in_params_subcommand.policy.guild_only != 1U ||
        builder_in_subcommand.command != &typed_subcommand_parent ||
        strcmp(builder_in_subcommand.subcommand_path, "admin/sync") != 0 ||
        builder_in_subcommand.bindings.count != 2U ||
        builder_in_subcommand.validators.count != 2U ||
        typed_autocomplete.command != &typed_command ||
        typed_autocomplete.command_name != NULL ||
        typed_autocomplete.subcommand_path != NULL ||
        typed_autocomplete.args_size != sizeof(sugar_field_args_t) ||
        typed_autocomplete.bindings.count != 1U ||
        typed_autocomplete.validators.count != 1U ||
        typed_autocomplete.out_route != &typed_autocomplete_route_id ||
        typed_autocomplete.policy.middleware_count != 1U ||
        strcmp(named_typed_autocomplete.command_name, "typed_parent") != 0 ||
        named_typed_autocomplete.bindings.count != 0U ||
        strcmp(guarded_params_autocomplete.command_name, "typed_guarded_params") != 0 ||
        guarded_params_autocomplete.bindings.count != 2U ||
        guarded_params_autocomplete.policy.cooldown.bucket != DCC_APP_COOLDOWN_USER ||
        guarded_params_autocomplete.policy.cooldown.limit != 1U ||
        strcmp(params_autocomplete_fn.command_name, "sugar_dummy_typed_handler") != 0 ||
        params_autocomplete_fn.bindings.count != 2U ||
        params_autocomplete_fn.user_data != &bound_string ||
        params_autocomplete_fn.policy.guild_only != 1U ||
        typed_subcommand_autocomplete.command != &typed_subcommand_parent ||
        strcmp(typed_subcommand_autocomplete.subcommand_path, "admin/reload") != 0 ||
        typed_subcommand_autocomplete.bindings.count != 1U ||
        strcmp(in_subcommand_autocomplete.command_name, "typed_parent") != 0 ||
        strcmp(in_subcommand_autocomplete.subcommand_path, "admin/auto") != 0 ||
        in_subcommand_autocomplete.bindings.count != 2U ||
        in_subcommand_autocomplete.user_data != &bound_string ||
        builder_in_subcommand_autocomplete.command != &typed_subcommand_parent ||
        strcmp(builder_in_subcommand_autocomplete.subcommand_path, "admin/complete") != 0 ||
        builder_in_subcommand_autocomplete.bindings.count != 2U ||
        builder_in_subcommand_autocomplete.user_data != &bound_string ||
        strcmp(typed_modal.custom_id, "typed.modal") != 0 ||
        typed_modal.bindings.count != 1U ||
        typed_modal.validators.count != 0U ||
        policy_typed_modal.policy.guild_only != 1U ||
        guarded_typed_modal.policy.dm_only != 1U ||
        typed_modal_prefix.prefix != 1U ||
        strcmp(typed_modal_prefix.custom_id, "typed:") != 0 ||
        modal_flow.size != sizeof(modal_flow) ||
        strcmp(modal_flow.modal.custom_id, "typed.flow") != 0 ||
        modal_flow.modal.components_count != 1U ||
        strcmp(modal_flow.route.custom_id, "typed.flow") != 0 ||
        modal_flow.route.args_size != sizeof(sugar_field_args_t) ||
        modal_flow.route.bindings.count != typed_form_bindings.count ||
        modal_v2_flow.size != sizeof(modal_v2_flow) ||
        strcmp(modal_v2_flow.modal.custom_id, "typed.v2.flow") != 0 ||
        modal_v2_flow.modal.components_v2_count != 1U ||
        strcmp(modal_v2_flow.route.custom_id, "typed.v2.flow") != 0 ||
        modal_v2_flow.route.validators.count != typed_validators.count ||
        strcmp(typed_button.custom_id, "typed.button:") != 0 ||
        typed_button.prefix != 1U ||
        typed_button.bindings.count != 2U ||
        typed_button.validators.count != 2U ||
        policy_typed_button.policy.middleware_count != 1U ||
        guarded_typed_button.prefix != 1U ||
        guarded_typed_button.policy.any_role_id_count != 2U ||
        guarded_typed_button.policy.cooldown.limit != 1U ||
        strcmp(typed_select.custom_id, "typed.select") != 0 ||
        typed_select.prefix != 0U ||
        typed_select.bindings.count != 2U ||
        policy_typed_select.policy.owner_user_id_count != 2U ||
        guarded_typed_select.policy.owner_user_id_count != 1U ||
        guarded_typed_select.policy.owner_user_ids[0] != 902ULL ||
        strcmp(typed_cog.name, "sugar.typed_cog") != 0 ||
        typed_cog.typed_slash_command_count != 1U ||
        typed_cog.typed_slash_commands == NULL ||
        strcmp(typed_cog.typed_slash_commands[0].command->name, "typed_empty") != 0 ||
        typed_cog.typed_subcommand_count != 1U ||
        typed_cog.typed_subcommands == NULL ||
        strcmp(typed_cog.typed_subcommands[0].subcommand_path, "admin/inspect") != 0 ||
        typed_cog.typed_autocomplete_count != 1U ||
        typed_cog.typed_autocompletes == NULL ||
        strcmp(typed_cog.typed_autocompletes[0].command_name, "typed_parent") != 0 ||
        typed_cog.typed_modal_count != 1U ||
        typed_cog.typed_modals == NULL ||
        strcmp(typed_cog.typed_modals[0].custom_id, "typed.modal") != 0 ||
        typed_cog.typed_button_count != 1U ||
        typed_cog.typed_buttons == NULL ||
        strcmp(typed_cog.typed_buttons[0].custom_id, "typed.button:") != 0 ||
        typed_cog.typed_select_count != 1U ||
        typed_cog.typed_selects == NULL ||
        strcmp(typed_cog.typed_selects[0].custom_id, "typed.select") != 0 ||
        strcmp(app_definition.name, "sugar.app") != 0 ||
        app_definition.module_count != 1U ||
        app_definition.modules == NULL ||
        app_definition.extension_count != 2U ||
        app_definition.extensions == NULL ||
        strcmp(app_definition.extensions[0].name, "sugar.cog") != 0 ||
        strcmp(app_definition.extensions[1].name, "sugar.typed_cog") != 0 ||
        app_definition.extensions[0].slash_command_count != 2U ||
        app_definition.extensions[1].typed_slash_command_count != 1U ||
        app_definition.extensions[1].typed_subcommand_count != 1U ||
        app_definition.extensions[1].typed_autocomplete_count != 1U ||
        app_definition.typed_slash_command_count != 1U ||
        app_definition.typed_subcommand_count != 1U ||
        app_definition.typed_autocomplete_count != 2U ||
        app_definition.typed_modal_count != 1U ||
        app_definition.typed_button_count != 1U ||
        app_definition.typed_select_count != 1U ||
        app_definition.message_command_count != 1U ||
        app_definition.message_commands == NULL ||
        strcmp(app_definition.message_commands[0].prefix, "!") != 0 ||
        strcmp(app_definition.message_commands[0].name, "top") != 0 ||
        app_definition.message_commands[0].user_data != &bound_string ||
        app_definition.event_count != 2U ||
        app_definition.events == NULL ||
        app_definition.events[0].type != DCC_APP_EXTENSION_READY ||
        app_definition.events[0].ready_handler != sugar_dummy_ready_handler ||
        app_definition.events[0].user_data != &bound_string ||
        app_definition.events[1].type != DCC_APP_EXTENSION_MESSAGE_UPDATE ||
        app_definition.events[1].message_handler != sugar_dummy_message_handler ||
        app_definition.task_count != 2U ||
        app_definition.tasks == NULL ||
        app_definition.tasks[0].type != DCC_APP_EXTENSION_TASK_EVERY_MINUTES ||
        app_definition.tasks[0].interval != 10U ||
        app_definition.tasks[0].user_data != NULL ||
        app_definition.tasks[1].type != DCC_APP_EXTENSION_TASK_EVERY_HOURS ||
        app_definition.tasks[1].interval != 1U ||
        app_definition.tasks[1].user_data != &bound_string ||
        app_definition.view_count != 1U ||
        app_definition.views == NULL ||
        app_definition.views[0].item_count != 2U ||
        app_definition.views[0].items == NULL ||
        strcmp(app_definition.views[0].items[0].custom_id, "app.persistent.button") != 0 ||
        app_definition.views[0].items[1].type != DCC_APP_VIEW_SELECT_PREFIX ||
        app_definition.component_session_count != 1U ||
        app_definition.component_sessions == NULL ||
        app_definition.component_sessions[0].session != &sugar_component_session ||
        app_definition.component_sessions[0].store != NULL ||
        app_definition.component_sessions[0].handler != sugar_dummy_handler ||
        app_definition.component_sessions[0].user_data != &bound_string ||
        app_definition.middleware_count != 1U ||
        app_definition.middlewares == NULL ||
        app_definition.middlewares[0].middleware != sugar_dummy_middleware ||
        app_definition.guild_only != 1U ||
        app_definition.channel_type_count != 2U ||
        app_definition.channel_types == NULL ||
        app_definition.channel_types[0] != DCC_CHANNEL_TEXT ||
        app_definition.channel_types[1] != DCC_CHANNEL_PUBLIC_THREAD ||
        app_definition.owner_user_id_count != 2U ||
        app_definition.owner_user_ids == NULL ||
        app_definition.owner_user_ids[0] != 123ULL ||
        app_definition.owner_user_ids[1] != 456ULL ||
        app_definition.required_permissions != DCC_PERMISSION_MANAGE_MESSAGES ||
        app_definition.required_role_id_count != 2U ||
        app_definition.required_role_ids == NULL ||
        app_definition.required_role_ids[0] != 789ULL ||
        app_definition.required_role_ids[1] != 790ULL ||
        app_definition.any_role_id_count != 2U ||
        app_definition.any_role_ids == NULL ||
        app_definition.any_role_ids[0] != 791ULL ||
        app_definition.any_role_ids[1] != 792ULL ||
        app_definition.check_count != 1U ||
        app_definition.checks == NULL ||
        app_definition.checks[0].check != sugar_dummy_check ||
        app_definition.checks[0].user_data != &bound_string ||
        strcmp(app_definition.checks[0].title, "App blocked") != 0 ||
        strcmp(app_definition.checks[0].description, "This app is blocked.") != 0 ||
        app_definition.cooldown.bucket != DCC_APP_COOLDOWN_USER ||
        app_definition.cooldown.limit != 3U ||
        app_definition.cooldown.window_ms != 2000U ||
        app_definition.command_sync == NULL ||
        app_definition.command_sync->application_id != 0U ||
        app_definition.command_sync->command_registry.size != 0U ||
        app_definition.command_sync->infer_application_id != 1U ||
        app_definition.command_sync->apply != 1U ||
        app_definition.auto_defer_after_ms != 1500U ||
        app_definition.auto_defer_ephemeral != 1U ||
        auto_defer_definition.auto_defer_after_ms != 1500U ||
        auto_defer_definition.auto_defer_ephemeral != 0U ||
        auto_defer_private_definition.auto_defer_after_ms != 1750U ||
        auto_defer_private_definition.auto_defer_ephemeral != 1U ||
        app_definition.use_default_error_responses != 1U ||
        app_definition.error_handler != sugar_error_handler ||
        app_definition.error_user_data != &bound_string ||
        dev_app_definition.command_sync == NULL ||
        dev_app_definition.command_sync->infer_application_id != 1U ||
        dev_app_definition.command_sync->apply != 1U ||
        dev_app_definition.auto_defer_after_ms != 1500U ||
        dev_app_definition.auto_defer_ephemeral != 1U ||
        dev_app_definition.use_default_error_responses != 1U ||
        dev_plan_app_definition.command_sync == NULL ||
        dev_plan_app_definition.command_sync->infer_application_id != 1U ||
        dev_plan_app_definition.command_sync->apply != 0U ||
        dev_plan_app_definition.auto_defer_after_ms != 2500U ||
        dev_plan_app_definition.auto_defer_ephemeral != 1U ||
        dev_plan_app_definition.use_default_error_responses != 1U ||
        dm_app_definition.dm_only != 1U ||
        dm_app_definition.guild_only != 0U ||
        nsfw_app_definition.nsfw_only != 1U ||
        nsfw_app_definition.dm_only != 0U ||
        view.item_count != 3U ||
        view.items == NULL ||
        view.items[0].type != DCC_APP_VIEW_BUTTON ||
        view.items[1].type != DCC_APP_VIEW_SELECT_PREFIX ||
        view.items[2].type != DCC_APP_VIEW_MODAL ||
        registry.guild_id != 123ULL ||
        registry.dry_run != 1U ||
        registry_delete_stale.delete_stale != 1U ||
        registration.guild_id != 0U ||
        guild_registration.guild_id != 456ULL ||
        autocomplete.choices_count != 3U ||
        autocomplete.choices[0].value_type != DCC_AUTOCOMPLETE_CHOICE_STRING ||
        strcmp(autocomplete.choices[0].value_string, "feral") != 0 ||
        autocomplete.choices[1].value_type != DCC_AUTOCOMPLETE_CHOICE_INTEGER ||
        autocomplete.choices[1].value_integer != 25 ||
        autocomplete.choices[2].value_type != DCC_AUTOCOMPLETE_CHOICE_NUMBER ||
        autocomplete.choices[2].value_number < 0.49 ||
        autocomplete.choices[2].value_number > 0.51 ||
        session_options.lock_user != 1U ||
        session_options.user_id != 789ULL ||
        session_check.guild_id != 222ULL ||
        listener.listen_buttons != 1U ||
        listener.listen_selects != 1U ||
        listener.listen_forms != 1U ||
        canary.canary_percent != 5U ||
        hot_reload.backend != DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER ||
        hot_reload.worker_canary_enabled != 1U ||
        firewall.invalid_request_hard_limit != DCC_REST_FIREWALL_DEFAULT_INVALID_REQUEST_HARD_LIMIT ||
        firewall_request.critical != 1U ||
        replay.kind != DCC_REPLAY_GATEWAY ||
        interaction_replay.kind != DCC_REPLAY_INTERACTION ||
        flow.auto_defer_ephemeral != 1U ||
        stickers.sticker_ids_count != 2U ||
        stickers.sticker_ids[1] != 222ULL ||
        poll.answer_count != 2U ||
        poll.duration_hours != 24U ||
        legacy_select.options_count != 2U ||
        legacy_select.options[1].has_default != 1U ||
        legacy_button_ns.type != DCC_COMPONENT_BUTTON ||
        strcmp(legacy_button_ns.custom_id, "birthday.register") != 0 ||
        legacy_select_ns.type != DCC_COMPONENT_STRING_SELECT ||
        strcmp(legacy_select_ns.custom_id, "birthday.schedule") != 0 ||
        legacy_input_ns.type != DCC_COMPONENT_TEXT_INPUT ||
        strcmp(legacy_input_ns.custom_id, "birthday.date") != 0 ||
        gallery.media_count != 2U ||
        gallery.media[1].has_spoiler != 1U ||
        thumbnail.media_count != 1U ||
        file.media_count != 1U ||
        string_select.options_count != 2U ||
        v2_button_ns.type != DCC_COMPONENT_V2_BUTTON ||
        strcmp(v2_button_ns.custom_id, "birthday.register") != 0 ||
        string_select_ns.type != DCC_COMPONENT_V2_STRING_SELECT ||
        strcmp(string_select_ns.custom_id, "birthday.schedule") != 0 ||
        user_select_ns.type != DCC_COMPONENT_V2_USER_SELECT ||
        strcmp(user_select_ns.custom_id, "birthday.user") != 0 ||
        channel_select.default_value_count != 1U ||
        channel_select.channel_type_count != 2U ||
        channel_select_ns.type != DCC_COMPONENT_V2_CHANNEL_SELECT ||
        strcmp(channel_select_ns.custom_id, "birthday.channel") != 0 ||
        radio.options_count != 2U ||
        checkbox_group.options_count != 2U ||
        text_input_ns.type != DCC_COMPONENT_V2_TEXT_INPUT ||
        strcmp(text_input_ns.custom_id, "birthday.date") != 0 ||
        file_upload_ns.type != DCC_COMPONENT_V2_FILE_UPLOAD ||
        strcmp(file_upload_ns.custom_id, "birthday.proof") != 0 ||
        radio_ns.type != DCC_COMPONENT_V2_RADIO_GROUP ||
        strcmp(radio_ns.custom_id, "birthday.visibility") != 0 ||
        checkbox_ns.type != DCC_COMPONENT_V2_CHECKBOX ||
        strcmp(checkbox_ns.custom_id, "birthday.public_age") != 0 ||
        ui_button_ns.type != DCC_COMPONENT_V2_BUTTON ||
        strcmp(ui_button_ns.custom_id, "birthday.schedule") != 0 ||
        ui_select_ns.type != DCC_COMPONENT_V2_STRING_SELECT ||
        strcmp(ui_select_ns.custom_id, "birthday.month") != 0 ||
        ui_input_ns.type != DCC_COMPONENT_V2_TEXT_INPUT ||
        strcmp(ui_input_ns.custom_id, "birthday.date") != 0 ||
        managed.channel_id != 333ULL ||
        managed.message != &managed_message ||
        managed_keep.keep_previous != 1U ||
        DCC_APP_PUBLISH_LATEST(NULL, &managed, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_PUBLISH_LATEST(NULL, &managed, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_SEND(NULL, 333ULL, DCC_MESSAGE_TEXT("send")) != DCC_ERR_INVALID_ARG ||
        DCC_APP_SEND_TEXT(NULL, 333ULL, "send") != DCC_ERR_INVALID_ARG ||
        dcc_app_get_channel(NULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_channels(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_channel(NULL, 222ULL, "{\"name\":\"general\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_channel_params(NULL, &channel_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_channel(NULL, 333ULL, "{\"name\":\"general\"}", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_channel_params(NULL, &channel_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_channel(NULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_trigger_channel_typing(NULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_follow_news_channel(NULL, 333ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_set_channel_voice_status(NULL, 333ULL, "Streaming", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_current_user_voice_state(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_current_user_voice_state(NULL, 222ULL, "{\"suppress\":true}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_current_user_voice_state_params(NULL, &voice_state_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_get_user_voice_state(NULL, 222ULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_user_voice_state(NULL, 222ULL, 333ULL, "{\"suppress\":true}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_user_voice_state_params(NULL, &voice_state_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_voice_regions(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_voice_regions(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_channel_positions(NULL, 222ULL, "[{\"id\":\"333\",\"position\":1}]", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_channel_positions_params(NULL, &channel_positions_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_get_channel_invites(NULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_channel_invite(NULL, 333ULL, "{\"max_age\":3600}", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_channel_invite_params(NULL, &invite_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_channel_permission(NULL, 333ULL, 444ULL, "{\"allow\":\"1024\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_channel_permission_params(NULL, 333ULL, &permission_overwrite, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_delete_channel_permission(NULL, 333ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_CHANNEL(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_CHANNELS(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_CHANNEL(NULL, 222ULL, "{\"name\":\"general\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_CHANNEL_PARAMS(NULL, &channel_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_CHANNEL(NULL, 333ULL, "{\"name\":\"general\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_CHANNEL_PARAMS(NULL, &channel_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_CHANNEL(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_TRIGGER_CHANNEL_TYPING(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_FOLLOW_NEWS_CHANNEL(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_SET_CHANNEL_VOICE_STATUS(NULL, 333ULL, "Streaming") != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_CURRENT_USER_VOICE_STATE(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_MY_VOICE_STATE(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_CURRENT_USER_VOICE_STATE(NULL, 222ULL, "{\"suppress\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_MY_VOICE_STATE(NULL, 222ULL, "{\"suppress\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_CURRENT_USER_VOICE_STATE_PARAMS(NULL, &voice_state_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_MY_VOICE_STATE_PARAMS(NULL, &voice_state_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_USER_VOICE_STATE(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_USER_VOICE_STATE(NULL, 222ULL, 333ULL, "{\"suppress\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_USER_VOICE_STATE_PARAMS(NULL, &voice_state_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_VOICE_REGIONS(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_VOICE_REGIONS(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_REORDER_GUILD_CHANNELS(NULL, 222ULL, "[{\"id\":\"333\",\"position\":1}]") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_REORDER_GUILD_CHANNELS_PARAMS(NULL, &channel_positions_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_CHANNEL_INVITES(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_CHANNEL_INVITE(NULL, 333ULL, "{\"max_age\":3600}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_CHANNEL_INVITE_PARAMS(NULL, &invite_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_CHANNEL_PERMISSION(NULL, 333ULL, 444ULL, "{\"allow\":\"1024\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_CHANNEL_PERMISSION_PARAMS(NULL, 333ULL, &permission_overwrite) != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_CHANNEL_PERMISSION(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_SEND(NULL, 333ULL, DCC_MESSAGE_TEXT("send")) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_SEND_TEXT(NULL, 333ULL, "send") != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_SEND_SAFE(NULL, 333ULL, "@everyone safe") != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_SEND_UI(NULL, 333ULL, DCC_UI_TEXT("send")) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_FETCH(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_EDIT(NULL, 333ULL, "{\"name\":\"general\"}") != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_EDIT_PARAMS(NULL, &channel_params) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_DELETE(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_TYPING(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_FOLLOW(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_SET_VOICE_STATUS(NULL, 333ULL, "Streaming") != DCC_ERR_INVALID_ARG ||
        DCC_MY_VOICE_STATE_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MY_VOICE_STATE_EDIT(NULL, 222ULL, "{\"suppress\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_MY_VOICE_STATE_EDIT_PARAMS(NULL, &voice_state_params) != DCC_ERR_INVALID_ARG ||
        DCC_USER_VOICE_STATE_FETCH(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_USER_VOICE_STATE_EDIT(NULL, 222ULL, 333ULL, "{\"suppress\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_USER_VOICE_STATE_EDIT_PARAMS(NULL, &voice_state_params) != DCC_ERR_INVALID_ARG ||
        DCC_VOICE_REGIONS_FETCH(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_VOICE_REGIONS_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_INVITES_FETCH(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_INVITE_CREATE(NULL, 333ULL, "{\"max_age\":3600}") != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_INVITE_CREATE_PARAMS(NULL, &invite_params) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_PERMISSION_EDIT(NULL, 333ULL, 444ULL, "{\"allow\":\"1024\"}") != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_PERMISSION_EDIT_PARAMS(NULL, 333ULL, &permission_overwrite) != DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_PERMISSION_DELETE(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_CHANNELS_REORDER(NULL, 222ULL, "[{\"id\":\"333\",\"position\":1}]") !=
            DCC_ERR_INVALID_ARG ||
        DCC_GUILD_CHANNELS_REORDER_PARAMS(NULL, &channel_positions_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_CHANNELS_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_CHANNEL_CREATE(NULL, 222ULL, "{\"name\":\"general\"}") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_CHANNEL_CREATE_PARAMS(NULL, &channel_params) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_roles(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_role(NULL, 222ULL, "{\"name\":\"role\"}", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_role_params(NULL, &role_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_role(NULL, 222ULL, 444ULL, "{\"name\":\"role\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_role_params(NULL, &role_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_role_positions(
            NULL,
            222ULL,
            role_positions,
            DCC_ARRAY_LEN(role_positions),
            NULL,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_guild_role(NULL, 222ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_ROLES(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_ROLE(NULL, 222ULL, "{\"name\":\"role\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_ROLE_PARAMS(NULL, &role_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_ROLE(NULL, 222ULL, 444ULL, "{\"name\":\"role\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_ROLE_PARAMS(NULL, &role_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_REORDER_GUILD_ROLES(NULL, 222ULL, role_positions, DCC_ARRAY_LEN(role_positions)) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_GUILD_ROLE(NULL, 222ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_ROLES_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_ROLES_REORDER(NULL, 222ULL, role_positions, DCC_ARRAY_LEN(role_positions)) !=
            DCC_ERR_INVALID_ARG ||
        DCC_GUILD_ROLE_CREATE(NULL, 222ULL, "{\"name\":\"role\"}") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_ROLE_CREATE_PARAMS(NULL, &role_params) != DCC_ERR_INVALID_ARG ||
        DCC_ROLE_EDIT(NULL, 222ULL, 444ULL, "{\"name\":\"role\"}") != DCC_ERR_INVALID_ARG ||
        DCC_ROLE_EDIT_PARAMS(NULL, &role_params) != DCC_ERR_INVALID_ARG ||
        DCC_ROLE_DELETE(NULL, 222ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_MESSAGE(NULL, 333ULL, 444ULL, DCC_MESSAGE_TEXT("edit")) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_TEXT(NULL, 333ULL, 444ULL, "edit") != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_MESSAGE(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_EDIT(NULL, 333ULL, 444ULL, DCC_MESSAGE_TEXT("edit")) != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_EDIT_TEXT(NULL, 333ULL, 444ULL, "edit") != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_EDIT_UI(NULL, 333ULL, 444ULL, DCC_UI_TEXT("edit")) != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_DELETE(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_crosspost_message(NULL, 333ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_pin_message(NULL, 333ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_unpin_message(NULL, 333ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_add_message_reaction(NULL, 333ULL, 444ULL, "thumbsup", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_own_message_reaction(NULL, 333ULL, 444ULL, "thumbsup", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_delete_user_message_reaction(NULL, 333ULL, 444ULL, "thumbsup", 555ULL, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_delete_all_message_reactions(NULL, 333ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_all_message_reactions_for_emoji(NULL, 333ULL, 444ULL, "thumbsup", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_CROSSPOST_MESSAGE(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_PIN_MESSAGE(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_UNPIN_MESSAGE(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_ADD_MESSAGE_REACTION(NULL, 333ULL, 444ULL, "thumbsup") != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_OWN_MESSAGE_REACTION(NULL, 333ULL, 444ULL, "thumbsup") != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_USER_MESSAGE_REACTION(NULL, 333ULL, 444ULL, "thumbsup", 555ULL) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_CLEAR_MESSAGE_REACTIONS(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI(NULL, 333ULL, 444ULL, "thumbsup") !=
            DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_CROSSPOST(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_PIN(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_UNPIN(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_REACT(NULL, 333ULL, 444ULL, "thumbsup") != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_UNREACT(NULL, 333ULL, 444ULL, "thumbsup") != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_REMOVE_REACTION(NULL, 333ULL, 444ULL, "thumbsup", 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_CLEAR_REACTIONS(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_CLEAR_REACTION_EMOJI(NULL, 333ULL, 444ULL, "thumbsup") != DCC_ERR_INVALID_ARG ||
        DCC_APP_THREAD_FROM_MESSAGE(NULL, 333ULL, 444ULL, "thread") != DCC_ERR_INVALID_ARG ||
        DCC_APP_THREAD_FROM_MESSAGE_PARAMS(NULL, 333ULL, 444ULL, &thread_params) !=
            DCC_ERR_INVALID_ARG ||
        DCC_MESSAGE_THREAD(NULL, 333ULL, 444ULL, "thread") != DCC_ERR_INVALID_ARG ||
        DCC_THREAD_FROM_MESSAGE_PARAMS(NULL, 333ULL, 444ULL, &thread_params) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_ARCHIVE_THREAD(NULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_LOCK_THREAD(NULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_UNLOCK_THREAD(NULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_THREAD_ARCHIVE(NULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_THREAD_LOCK(NULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_THREAD_UNLOCK(NULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_ADD_MEMBER_ROLE(NULL, 222ULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_REMOVE_MEMBER_ROLE(NULL, 222ULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MEMBER_ADD_ROLE(NULL, 222ULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MEMBER_REMOVE_ROLE(NULL, 222ULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_MEMBER_ADD_ROLE(NULL, 222ULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_MEMBER_REMOVE_ROLE(NULL, 222ULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_member(NULL, 222ULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_set_member_timeout(NULL, 222ULL, 333ULL, "2026-06-23T00:00:00Z", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_clear_member_timeout(NULL, 222ULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_move_member(NULL, 222ULL, 333ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_remove_member(NULL, 222ULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_MEMBER_FETCH(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MEMBER_TIMEOUT(NULL, 222ULL, 333ULL, "2026-06-23T00:00:00Z") != DCC_ERR_INVALID_ARG ||
        DCC_MEMBER_CLEAR_TIMEOUT(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MEMBER_MOVE(NULL, 222ULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MEMBER_KICK(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_MEMBER_FETCH(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_MEMBER_TIMEOUT(NULL, 222ULL, 333ULL, "2026-06-23T00:00:00Z") !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_bans(NULL, 222ULL, NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_bans_page(NULL, 222ULL, 0U, 0U, 50U, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_ban(NULL, 222ULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_ban(NULL, 222ULL, 333ULL, "{\"delete_message_seconds\":60}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_ban_seconds(NULL, 222ULL, 333ULL, 60U, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_ban_params(NULL, &ban_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_guild_ban(NULL, 222ULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_audit_log(NULL, 222ULL, NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_audit_log_page(NULL, 222ULL, 333ULL, 20U, 0U, 0U, 50U, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_current_guild_member(NULL, 222ULL, "{\"nick\":\"sugar\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_current_guild_member_params(NULL, &current_member_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_set_current_guild_member_nickname(NULL, 222ULL, "{\"nick\":\"sugar\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_set_current_guild_member_nickname_params(NULL, &nickname_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_prune_count_options(NULL, 222ULL, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles), NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_begin_guild_prune_options(NULL, 222ULL, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles), 1U, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_begin_guild_prune_params(NULL, &prune_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_integrations(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_integration(NULL, 222ULL, 333ULL, "{\"expire_behavior\":0}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_delete_guild_integration(NULL, 222ULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_sync_guild_integration(NULL, 222ULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_widget(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_widget(NULL, 222ULL, "{\"enabled\":true}", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_widget_params(NULL, 222ULL, &widget_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_vanity_url(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_onboarding(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_onboarding(NULL, 222ULL, "{\"enabled\":true}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_onboarding_params(NULL, &onboarding_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_welcome_screen(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_welcome_screen(NULL, 222ULL, "{\"enabled\":true}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_welcome_screen_params(NULL, 222ULL, &welcome_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_get_auto_moderation_rules(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_auto_moderation_rule(NULL, 222ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_auto_moderation_rule(NULL, 222ULL, "{\"name\":\"spam\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_auto_moderation_rule_params(NULL, 222ULL, &auto_mod_rule_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_auto_moderation_rule(NULL, 222ULL, 444ULL, "{\"enabled\":false}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_auto_moderation_rule_params(NULL, 222ULL, &auto_mod_rule_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_delete_auto_moderation_rule(NULL, 222ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_emojis(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_emoji(NULL, 222ULL, 555ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_emoji(NULL, 222ULL, "{\"name\":\"sugar\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_emoji_params(NULL, 222ULL, &emoji_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_emoji(NULL, 222ULL, 555ULL, "{\"name\":\"sugar\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_emoji_params(NULL, 222ULL, &emoji_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_guild_emoji(NULL, 222ULL, 555ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_stickers(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_sticker(NULL, 222ULL, 666ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_sticker_multipart(
            NULL,
            222ULL,
            sticker_fields,
            DCC_ARRAY_LEN(sticker_fields),
            &sticker_file,
            NULL,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_sticker_params(NULL, &sticker_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_sticker(NULL, 222ULL, 666ULL, "{\"name\":\"sugar\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_delete_guild_sticker(NULL, 222ULL, 666ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_sticker(NULL, 666ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_sticker_packs(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_guild_message_search(NULL, 222ULL, "content=sugar", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_guild_message_search_params(NULL, 222ULL, &message_search_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_BANS(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_BANS_QUERY(NULL, 222ULL, "limit=50") != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_BANS_PAGE(NULL, 222ULL, 0U, 0U, 50U) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_BAN(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_BAN_MEMBER(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_BAN_MEMBER_SECONDS(NULL, 222ULL, 333ULL, 60U) != DCC_ERR_INVALID_ARG ||
        DCC_APP_BAN_MEMBER_JSON(NULL, 222ULL, 333ULL, "{\"delete_message_seconds\":60}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_BAN_MEMBER_PARAMS(NULL, &ban_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_UNBAN_MEMBER(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_CURRENT_MEMBER(NULL, 222ULL, "{\"nick\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_CURRENT_MEMBER_PARAMS(NULL, &current_member_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_SET_MY_NICK_JSON(NULL, 222ULL, "{\"nick\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_SET_MY_NICK_PARAMS(NULL, &nickname_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_SET_MY_NICK(NULL, 222ULL, "sugar") != DCC_ERR_INVALID_ARG ||
        DCC_CURRENT_MEMBER_EDIT(NULL, 222ULL, "{\"nick\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_CURRENT_MEMBER_EDIT_PARAMS(NULL, &current_member_params) != DCC_ERR_INVALID_ARG ||
        DCC_CURRENT_MEMBER_SET_NICK(NULL, 222ULL, "sugar") != DCC_ERR_INVALID_ARG ||
        DCC_ME_EDIT(NULL, 222ULL, "{\"nick\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_ME_EDIT_PARAMS(NULL, &current_member_params) != DCC_ERR_INVALID_ARG ||
        DCC_ME_SET_NICK(NULL, 222ULL, "sugar") != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_PRUNE_COUNT(NULL, 222ULL, 7U) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_PRUNE_COUNT_OPTIONS(NULL, 222ULL, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles)) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_BEGIN_GUILD_PRUNE(NULL, 222ULL, 7U, 1U) != DCC_ERR_INVALID_ARG ||
        DCC_APP_BEGIN_GUILD_PRUNE_OPTIONS(NULL, 222ULL, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles), 1U) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_BEGIN_GUILD_PRUNE_PARAMS(NULL, &prune_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_AUDIT_LOG(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_AUDIT_LOG_QUERY(NULL, 222ULL, "limit=50") != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_AUDIT_LOG_PAGE(NULL, 222ULL, 333ULL, 20U, 0U, 0U, 50U) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_INTEGRATIONS(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_INTEGRATION(NULL, 222ULL, 333ULL, "{\"expire_behavior\":0}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_GUILD_INTEGRATION(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_SYNC_GUILD_INTEGRATION(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_WIDGET(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_WIDGET(NULL, 222ULL, "{\"enabled\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_WIDGET_PARAMS(NULL, 222ULL, &widget_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_VANITY_URL(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_ONBOARDING(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_ONBOARDING(NULL, 222ULL, "{\"enabled\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_ONBOARDING_PARAMS(NULL, &onboarding_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_WELCOME_SCREEN(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_WELCOME_SCREEN(NULL, 222ULL, "{\"enabled\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_WELCOME_SCREEN_PARAMS(NULL, 222ULL, &welcome_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_AUTO_MOD_RULES(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_AUTO_MOD_RULE(NULL, 222ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_AUTO_MOD_RULE(NULL, 222ULL, "{\"name\":\"spam\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_AUTO_MOD_RULE_PARAMS(NULL, 222ULL, &auto_mod_rule_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_AUTO_MOD_RULE(NULL, 222ULL, 444ULL, "{\"enabled\":false}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_AUTO_MOD_RULE_PARAMS(NULL, 222ULL, &auto_mod_rule_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_AUTO_MOD_RULE(NULL, 222ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_EMOJIS(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_EMOJI(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_EMOJI(NULL, 222ULL, "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_EMOJI_PARAMS(NULL, 222ULL, &emoji_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_EMOJI(NULL, 222ULL, 555ULL, "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_EMOJI_PARAMS(NULL, 222ULL, &emoji_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_GUILD_EMOJI(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_STICKERS(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_STICKER(NULL, 222ULL, 666ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_STICKER_MULTIPART(
            NULL,
            222ULL,
            sticker_fields,
            DCC_ARRAY_LEN(sticker_fields),
            &sticker_file
        ) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_STICKER_PARAMS(NULL, &sticker_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_STICKER(NULL, 222ULL, 666ULL, "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_GUILD_STICKER(NULL, 222ULL, 666ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_STICKER(NULL, 666ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_STICKER_PACKS(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GUILD_MESSAGE_SEARCH(NULL, 222ULL, "content=sugar") != DCC_ERR_INVALID_ARG ||
        DCC_APP_GUILD_MESSAGE_SEARCH_PARAMS(NULL, 222ULL, &message_search_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_BANS_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_BANS_FETCH_PAGE(NULL, 222ULL, 0U, 0U, 50U) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_BAN_FETCH(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_BAN(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_BAN_SECONDS(NULL, 222ULL, 333ULL, 60U) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_BAN_PARAMS(NULL, &ban_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_UNBAN(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MEMBER_BAN(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_MEMBER_BAN_SECONDS(NULL, 222ULL, 333ULL, 60U) != DCC_ERR_INVALID_ARG ||
        DCC_MEMBER_UNBAN(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_AUDIT_LOG_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_AUDIT_LOG_FETCH_QUERY(NULL, 222ULL, "limit=50") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_AUDIT_LOG_FETCH_PAGE(NULL, 222ULL, 333ULL, 20U, 0U, 0U, 50U) !=
            DCC_ERR_INVALID_ARG ||
        DCC_GUILD_INTEGRATIONS_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_INTEGRATION_EDIT(NULL, 222ULL, 333ULL, "{\"expire_behavior\":0}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_GUILD_INTEGRATION_DELETE(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_INTEGRATION_SYNC(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_WIDGET_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_WIDGET_EDIT(NULL, 222ULL, "{\"enabled\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_WIDGET_EDIT_PARAMS(NULL, 222ULL, &widget_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_VANITY_URL_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_ONBOARDING_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_ONBOARDING_EDIT(NULL, 222ULL, "{\"enabled\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_ONBOARDING_EDIT_PARAMS(NULL, &onboarding_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_WELCOME_SCREEN_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_WELCOME_SCREEN_EDIT(NULL, 222ULL, "{\"enabled\":true}") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_WELCOME_SCREEN_EDIT_PARAMS(NULL, 222ULL, &welcome_params) != DCC_ERR_INVALID_ARG ||
        DCC_AUTO_MOD_RULES_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_AUTO_MODERATION_RULES_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_AUTO_MOD_RULE_FETCH(NULL, 222ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_AUTO_MODERATION_RULE_FETCH(NULL, 222ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_AUTO_MOD_RULE_CREATE(NULL, 222ULL, "{\"name\":\"spam\"}") != DCC_ERR_INVALID_ARG ||
        DCC_AUTO_MOD_RULE_CREATE_PARAMS(NULL, 222ULL, &auto_mod_rule_params) != DCC_ERR_INVALID_ARG ||
        DCC_AUTO_MOD_RULE_EDIT(NULL, 222ULL, 444ULL, "{\"enabled\":false}") != DCC_ERR_INVALID_ARG ||
        DCC_AUTO_MOD_RULE_EDIT_PARAMS(NULL, 222ULL, &auto_mod_rule_params) != DCC_ERR_INVALID_ARG ||
        DCC_AUTO_MOD_RULE_DELETE(NULL, 222ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_EMOJIS_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_EMOJI_FETCH(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_EMOJI_CREATE(NULL, 222ULL, "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_EMOJI_CREATE_PARAMS(NULL, 222ULL, &emoji_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_EMOJI_EDIT(NULL, 222ULL, 555ULL, "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_EMOJI_EDIT_PARAMS(NULL, 222ULL, &emoji_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_EMOJI_DELETE(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_STICKERS_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_STICKER_FETCH(NULL, 222ULL, 666ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_STICKER_CREATE_MULTIPART(
            NULL,
            222ULL,
            sticker_fields,
            DCC_ARRAY_LEN(sticker_fields),
            &sticker_file
        ) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_STICKER_CREATE_PARAMS(NULL, &sticker_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_STICKER_EDIT(NULL, 222ULL, 666ULL, "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_STICKER_DELETE(NULL, 222ULL, 666ULL) != DCC_ERR_INVALID_ARG ||
        DCC_STICKER_FETCH(NULL, 666ULL) != DCC_ERR_INVALID_ARG ||
        DCC_STICKER_PACKS_FETCH(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_MESSAGES_SEARCH(NULL, 222ULL, "content=sugar") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_MESSAGES_SEARCH_PARAMS(NULL, 222ULL, &message_search_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_PRUNE_COUNT(NULL, 222ULL, 7U) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_PRUNE_COUNT_OPTIONS(NULL, 222ULL, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles)) !=
            DCC_ERR_INVALID_ARG ||
        DCC_GUILD_PRUNE_BEGIN(NULL, 222ULL, 7U, 1U) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_PRUNE_BEGIN_OPTIONS(NULL, 222ULL, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles), 1U) !=
            DCC_ERR_INVALID_ARG ||
        DCC_GUILD_PRUNE_BEGIN_PARAMS(NULL, &prune_params) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_scheduled_events(NULL, 222ULL, NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_scheduled_events_with_user_count(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_scheduled_event(NULL, 222ULL, 555ULL, NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_scheduled_event_with_user_count(NULL, 222ULL, 555ULL, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_scheduled_event(NULL, 222ULL, "{\"name\":\"event\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_scheduled_event_params(NULL, &scheduled_event_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_scheduled_event(NULL, 222ULL, 555ULL, "{\"name\":\"event\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_scheduled_event_params(NULL, &scheduled_event_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_delete_guild_scheduled_event(NULL, 222ULL, 555ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_scheduled_event_users(NULL, 222ULL, 555ULL, NULL, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_scheduled_event_users_page(NULL, 222ULL, 555ULL, 25U, 0U, 0U, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_SCHEDULED_EVENTS(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_SCHEDULED_EVENTS_QUERY(NULL, 222ULL, "with_user_count=true") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_SCHEDULED_EVENTS_WITH_USER_COUNT(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_SCHEDULED_EVENT(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_SCHEDULED_EVENT_QUERY(NULL, 222ULL, 555ULL, "with_user_count=true") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_SCHEDULED_EVENT_WITH_USER_COUNT(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_SCHEDULED_EVENT(NULL, 222ULL, "{\"name\":\"event\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_SCHEDULED_EVENT_PARAMS(NULL, &scheduled_event_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_SCHEDULED_EVENT(NULL, 222ULL, 555ULL, "{\"name\":\"event\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_SCHEDULED_EVENT_PARAMS(NULL, &scheduled_event_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_GUILD_SCHEDULED_EVENT(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS_QUERY(NULL, 222ULL, 555ULL, "limit=25") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS_PAGE(NULL, 222ULL, 555ULL, 25U, 0U, 0U) !=
            DCC_ERR_INVALID_ARG ||
        DCC_GUILD_SCHEDULED_EVENTS_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_SCHEDULED_EVENTS_FETCH_WITH_USER_COUNT(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_SCHEDULED_EVENT_FETCH(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_SCHEDULED_EVENT_FETCH_WITH_USER_COUNT(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_SCHEDULED_EVENT_CREATE(NULL, 222ULL, "{\"name\":\"event\"}") != DCC_ERR_INVALID_ARG ||
        DCC_SCHEDULED_EVENT_CREATE_PARAMS(NULL, &scheduled_event_params) != DCC_ERR_INVALID_ARG ||
        DCC_SCHEDULED_EVENT_EDIT(NULL, 222ULL, 555ULL, "{\"name\":\"event\"}") != DCC_ERR_INVALID_ARG ||
        DCC_SCHEDULED_EVENT_EDIT_PARAMS(NULL, &scheduled_event_params) != DCC_ERR_INVALID_ARG ||
        DCC_SCHEDULED_EVENT_DELETE(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_SCHEDULED_EVENT_USERS_FETCH(NULL, 222ULL, 555ULL) != DCC_ERR_INVALID_ARG ||
        DCC_SCHEDULED_EVENT_USERS_FETCH_PAGE(NULL, 222ULL, 555ULL, 25U, 0U, 0U) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_channel_webhooks(NULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_webhooks(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_webhook(NULL, 666ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_webhook_with_token(NULL, 666ULL, "token", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_webhook(NULL, 333ULL, "{\"name\":\"sugar\"}", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_webhook_params(NULL, &webhook_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_webhook(NULL, 666ULL, "{\"name\":\"sugar\"}", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_webhook_params(NULL, &webhook_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_webhook_with_token(NULL, 666ULL, "token", "{\"name\":\"sugar\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_webhook_with_token_params(NULL, &webhook_token_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_delete_webhook(NULL, 666ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_webhook_with_token(NULL, 666ULL, "token", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_execute_webhook(NULL, 666ULL, "token", NULL, "{\"content\":\"hello\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_execute_webhook_options(NULL, 666ULL, "token", 1U, 0U, 0U, "{\"content\":\"hello\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_execute_webhook_message(
            NULL,
            666ULL,
            "token",
            1U,
            0U,
            0U,
            &managed_message,
            NULL,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_app_execute_webhook_text(NULL, 666ULL, "token", 1U, 0U, "hello", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_execute_webhook_multipart(NULL, 666ULL, "token", NULL, "{}", NULL, 0U, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_execute_webhook_multipart_options(
            NULL,
            666ULL,
            "token",
            1U,
            0U,
            0U,
            "{}",
            NULL,
            0U,
            NULL,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_webhook_message(NULL, 666ULL, "token", 777ULL, NULL, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_get_webhook_message_thread(NULL, 666ULL, "token", 777ULL, 888ULL, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_webhook_message(NULL, 666ULL, "token", 777ULL, NULL, "{\"content\":\"edit\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_webhook_message_thread(
            NULL,
            666ULL,
            "token",
            777ULL,
            888ULL,
            "{\"content\":\"edit\"}",
            NULL,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_webhook_message_builder(NULL, 666ULL, "token", 777ULL, NULL, &managed_message, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_webhook_message_builder_thread(
            NULL,
            666ULL,
            "token",
            777ULL,
            888ULL,
            &managed_message,
            NULL,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_webhook_message_multipart(NULL, 666ULL, "token", 777ULL, NULL, "{}", NULL, 0U, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_webhook_message_multipart_thread(
            NULL,
            666ULL,
            "token",
            777ULL,
            888ULL,
            "{}",
            NULL,
            0U,
            NULL,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_webhook_message(NULL, 666ULL, "token", 777ULL, NULL, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_delete_webhook_message_thread(NULL, 666ULL, "token", 777ULL, 888ULL, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_CHANNEL_WEBHOOKS(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_WEBHOOKS(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_WEBHOOK(NULL, 666ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_WEBHOOK_TOKEN(NULL, 666ULL, "token") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_WEBHOOK(NULL, 333ULL, "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_WEBHOOK_PARAMS(NULL, &webhook_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK(NULL, 666ULL, "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK_PARAMS(NULL, &webhook_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK_TOKEN(NULL, 666ULL, "token", "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK_TOKEN_PARAMS(NULL, &webhook_token_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_WEBHOOK(NULL, 666ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_WEBHOOK_TOKEN(NULL, 666ULL, "token") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EXECUTE_WEBHOOK(NULL, 666ULL, "token", NULL, "{\"content\":\"hello\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EXECUTE_WEBHOOK_OPTIONS(NULL, 666ULL, "token", 1U, 0U, 0U, "{\"content\":\"hello\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EXECUTE_WEBHOOK_MESSAGE(NULL, 666ULL, "token", 1U, 0U, 0U, DCC_MESSAGE_TEXT("hello")) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EXECUTE_WEBHOOK_TEXT(NULL, 666ULL, "token", 1U, 0U, "hello") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EXECUTE_WEBHOOK_MULTIPART(NULL, 666ULL, "token", NULL, "{}", NULL, 0U) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EXECUTE_WEBHOOK_MULTIPART_OPTIONS(NULL, 666ULL, "token", 1U, 0U, 0U, "{}", NULL, 0U) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_WEBHOOK_MESSAGE(NULL, 666ULL, "token", 777ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_WEBHOOK_MESSAGE_QUERY(NULL, 666ULL, "token", 777ULL, "thread_id=888") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_WEBHOOK_MESSAGE_THREAD(NULL, 666ULL, "token", 777ULL, 888ULL) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK_MESSAGE(NULL, 666ULL, "token", 777ULL, "{\"content\":\"edit\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK_MESSAGE_QUERY(NULL, 666ULL, "token", 777ULL, "thread_id=888", "{\"content\":\"edit\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK_MESSAGE_THREAD(NULL, 666ULL, "token", 777ULL, 888ULL, "{\"content\":\"edit\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK_MESSAGE_BUILDER(NULL, 666ULL, "token", 777ULL, DCC_MESSAGE_TEXT("edit")) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK_MESSAGE_BUILDER_THREAD(NULL, 666ULL, "token", 777ULL, 888ULL, DCC_MESSAGE_TEXT("edit")) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK_MESSAGE_MULTIPART(NULL, 666ULL, "token", 777ULL, NULL, "{}", NULL, 0U) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_WEBHOOK_MESSAGE_MULTIPART_THREAD(NULL, 666ULL, "token", 777ULL, 888ULL, "{}", NULL, 0U) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_WEBHOOK_MESSAGE(NULL, 666ULL, "token", 777ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_WEBHOOK_MESSAGE_QUERY(NULL, 666ULL, "token", 777ULL, "thread_id=888") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_WEBHOOK_MESSAGE_THREAD(NULL, 666ULL, "token", 777ULL, 888ULL) !=
            DCC_ERR_INVALID_ARG ||
        DCC_CHANNEL_WEBHOOKS_FETCH(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_WEBHOOKS_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_FETCH(NULL, 666ULL) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_FETCH_TOKEN(NULL, 666ULL, "token") != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_CREATE(NULL, 333ULL, "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_CREATE_PARAMS(NULL, &webhook_params) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_EDIT(NULL, 666ULL, "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_EDIT_PARAMS(NULL, &webhook_params) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_EDIT_TOKEN(NULL, 666ULL, "token", "{\"name\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_EDIT_TOKEN_PARAMS(NULL, &webhook_token_params) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_DELETE(NULL, 666ULL) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_DELETE_TOKEN(NULL, 666ULL, "token") != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_SEND_JSON(NULL, 666ULL, "token", "{\"content\":\"hello\"}") != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_SEND_JSON_QUERY(NULL, 666ULL, "token", "wait=true", "{\"content\":\"hello\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_SEND_OPTIONS(NULL, 666ULL, "token", 1U, 0U, 0U, "{\"content\":\"hello\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_SEND(NULL, 666ULL, "token", DCC_MESSAGE_TEXT("hello")) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_SEND_WAIT(NULL, 666ULL, "token", DCC_MESSAGE_TEXT("hello")) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_SEND_THREAD(NULL, 666ULL, "token", 888ULL, DCC_MESSAGE_TEXT("hello")) !=
            DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_SEND_TEXT(NULL, 666ULL, "token", "hello") != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_SEND_TEXT_WAIT(NULL, 666ULL, "token", "hello") != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_SEND_UI(NULL, 666ULL, "token", DCC_UI_TEXT("hello")) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_SEND_UI_WAIT(NULL, 666ULL, "token", DCC_UI_TEXT("hello")) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_MESSAGE_FETCH(NULL, 666ULL, "token", 777ULL) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_MESSAGE_FETCH_THREAD(NULL, 666ULL, "token", 777ULL, 888ULL) !=
            DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_MESSAGE_EDIT_JSON(NULL, 666ULL, "token", 777ULL, "{\"content\":\"edit\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_MESSAGE_EDIT(NULL, 666ULL, "token", 777ULL, DCC_MESSAGE_TEXT("edit")) !=
            DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_MESSAGE_EDIT_THREAD(NULL, 666ULL, "token", 777ULL, 888ULL, DCC_MESSAGE_TEXT("edit")) !=
            DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_MESSAGE_DELETE(NULL, 666ULL, "token", 777ULL) != DCC_ERR_INVALID_ARG ||
        DCC_WEBHOOK_MESSAGE_DELETE_THREAD(NULL, 666ULL, "token", 777ULL, 888ULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_get_current_application(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_current_user(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_current_user(NULL, "{\"username\":\"sugar\"}", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_current_user_params(NULL, &user_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_user(NULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_current_user_connections(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_current_user_guilds(NULL, NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_current_user_dms(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_dm_channel(NULL, "{\"recipient_id\":\"333\"}", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_dm_channel_params(NULL, &dm_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_add_group_dm_recipient(NULL, 333ULL, 444ULL, "{\"access_token\":\"token\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_add_group_dm_recipient_params(NULL, &group_dm_recipient_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_remove_group_dm_recipient(NULL, 333ULL, 444ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_direct_message(NULL, 333ULL, "{\"content\":\"hello\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_direct_message_builder(NULL, 333ULL, &managed_message, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_direct_message_text(NULL, 333ULL, "hello", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_leave_guild(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_invites(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_invite(NULL, "abc", NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_invite_full(NULL, "abc", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_invite(NULL, "abc", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_stage_instance(NULL, "{\"channel_id\":\"333\",\"topic\":\"sugar\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_stage_instance_params(NULL, &stage_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_stage_instance(NULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_stage_instance(NULL, 333ULL, "{\"topic\":\"sugar\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_stage_instance_params(NULL, &stage_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_stage_instance(NULL, 333ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_template(NULL, "abc", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_from_template(NULL, "abc", "{\"name\":\"guild\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_from_template_params(NULL, &template_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_get_guild_templates(NULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_template(NULL, 222ULL, "{\"name\":\"template\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_guild_template_params(NULL, &template_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_template(NULL, 222ULL, "abc", "{\"name\":\"template\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_modify_guild_template_params(NULL, &template_params, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_sync_guild_template(NULL, 222ULL, "abc", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_delete_guild_template(NULL, 222ULL, "abc", NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_gateway_bot(NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_skus(NULL, 111ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_entitlements(NULL, 111ULL, NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_get_entitlements_page(
            NULL,
            111ULL,
            333ULL,
            entitlement_sku_ids,
            DCC_ARRAY_LEN(entitlement_sku_ids),
            0U,
            0U,
            25U,
            222ULL,
            1U,
            NULL,
            NULL
        ) != DCC_ERR_INVALID_ARG ||
        dcc_app_create_test_entitlement(NULL, 111ULL, "{\"sku_id\":\"222\"}", NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_create_test_entitlement_params(NULL, &test_entitlement_params, NULL, NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_app_delete_test_entitlement(NULL, 111ULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_app_consume_entitlement(NULL, 111ULL, 222ULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_CURRENT_APPLICATION(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GATEWAY_BOT(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_SKUS(NULL, 111ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_ENTITLEMENTS(NULL, 111ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_ENTITLEMENTS_QUERY(NULL, 111ULL, "limit=25") != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_ENTITLEMENTS_PAGE(
            NULL,
            111ULL,
            333ULL,
            entitlement_sku_ids,
            DCC_ARRAY_LEN(entitlement_sku_ids),
            0U,
            0U,
            25U,
            222ULL,
            1U
        ) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_TEST_ENTITLEMENT(NULL, 111ULL, "{\"sku_id\":\"222\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_TEST_ENTITLEMENT_PARAMS(NULL, &test_entitlement_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_TEST_ENTITLEMENT(NULL, 111ULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CONSUME_ENTITLEMENT(NULL, 111ULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_CURRENT_USER(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_CURRENT_USER(NULL, "{\"username\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_CURRENT_USER_PARAMS(NULL, &user_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_USER(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_CURRENT_USER_CONNECTIONS(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_CURRENT_USER_GUILDS(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_CURRENT_USER_GUILDS_QUERY(NULL, "limit=25") != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_CURRENT_USER_DMS(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_DM_CHANNEL(NULL, "{\"recipient_id\":\"333\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_DM_CHANNEL_PARAMS(NULL, &dm_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_ADD_GROUP_DM_RECIPIENT(NULL, 333ULL, 444ULL, "{\"access_token\":\"token\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_ADD_GROUP_DM_RECIPIENT_PARAMS(NULL, &group_dm_recipient_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_REMOVE_GROUP_DM_RECIPIENT(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_DIRECT_MESSAGE(NULL, 333ULL, "{\"content\":\"hello\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_DIRECT_MESSAGE_BUILDER(NULL, 333ULL, DCC_MESSAGE_TEXT("hello")) !=
            DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_DIRECT_MESSAGE_TEXT(NULL, 333ULL, "hello") != DCC_ERR_INVALID_ARG ||
        DCC_APP_LEAVE_GUILD(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_INVITES(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_INVITE(NULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_INVITE_QUERY(NULL, "abc", "with_counts=true") != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_INVITE_FULL(NULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_INVITE(NULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_STAGE(NULL, "{\"channel_id\":\"333\",\"topic\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_STAGE_PARAMS(NULL, &stage_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_STAGE(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_STAGE(NULL, 333ULL, "{\"topic\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_STAGE_PARAMS(NULL, &stage_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_STAGE(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_TEMPLATE(NULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_FROM_TEMPLATE(NULL, "abc", "{\"name\":\"guild\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_FROM_TEMPLATE_PARAMS(NULL, &template_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_GET_GUILD_TEMPLATES(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_TEMPLATE(NULL, 222ULL, "{\"name\":\"template\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_CREATE_GUILD_TEMPLATE_PARAMS(NULL, &template_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_TEMPLATE(NULL, 222ULL, "abc", "{\"name\":\"template\"}") != DCC_ERR_INVALID_ARG ||
        DCC_APP_EDIT_GUILD_TEMPLATE_PARAMS(NULL, &template_params) != DCC_ERR_INVALID_ARG ||
        DCC_APP_SYNC_GUILD_TEMPLATE(NULL, 222ULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_APP_DELETE_GUILD_TEMPLATE(NULL, 222ULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_APPLICATION_FETCH(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_GATEWAY_BOT_FETCH(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_SKUS_FETCH(NULL, 111ULL) != DCC_ERR_INVALID_ARG ||
        DCC_ENTITLEMENTS_FETCH(NULL, 111ULL) != DCC_ERR_INVALID_ARG ||
        DCC_ENTITLEMENTS_FETCH_QUERY(NULL, 111ULL, "limit=25") != DCC_ERR_INVALID_ARG ||
        DCC_ENTITLEMENTS_FETCH_PAGE(
            NULL,
            111ULL,
            333ULL,
            entitlement_sku_ids,
            DCC_ARRAY_LEN(entitlement_sku_ids),
            0U,
            0U,
            25U,
            222ULL,
            1U
        ) != DCC_ERR_INVALID_ARG ||
        DCC_TEST_ENTITLEMENT_CREATE(NULL, 111ULL, "{\"sku_id\":\"222\"}") != DCC_ERR_INVALID_ARG ||
        DCC_TEST_ENTITLEMENT_CREATE_PARAMS(NULL, &test_entitlement_params) != DCC_ERR_INVALID_ARG ||
        DCC_TEST_ENTITLEMENT_DELETE(NULL, 111ULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_ENTITLEMENT_CONSUME(NULL, 111ULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_ME_FETCH(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_ME_EDIT_USER(NULL, "{\"username\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_ME_EDIT_USER_PARAMS(NULL, &user_params) != DCC_ERR_INVALID_ARG ||
        DCC_ME_CONNECTIONS_FETCH(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_ME_GUILDS_FETCH(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_ME_GUILDS_FETCH_QUERY(NULL, "limit=25") != DCC_ERR_INVALID_ARG ||
        DCC_ME_DMS_FETCH(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_USER_FETCH(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_USER_DM_CHANNEL_CREATE(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GROUP_DM_RECIPIENT_ADD(NULL, 333ULL, 444ULL, "{\"access_token\":\"token\"}") !=
            DCC_ERR_INVALID_ARG ||
        DCC_GROUP_DM_RECIPIENT_ADD_PARAMS(NULL, &group_dm_recipient_params) != DCC_ERR_INVALID_ARG ||
        DCC_GROUP_DM_RECIPIENT_REMOVE(NULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_USER_DM_SEND_JSON(NULL, 333ULL, "{\"content\":\"hello\"}") != DCC_ERR_INVALID_ARG ||
        DCC_USER_DM_SEND(NULL, 333ULL, DCC_MESSAGE_TEXT("hello")) != DCC_ERR_INVALID_ARG ||
        DCC_USER_DM_SEND_TEXT(NULL, 333ULL, "hello") != DCC_ERR_INVALID_ARG ||
        DCC_USER_DM_SEND_UI(NULL, 333ULL, DCC_UI_TEXT("hello")) != DCC_ERR_INVALID_ARG ||
        DCC_DM_SEND(NULL, 333ULL, DCC_MESSAGE_TEXT("hello")) != DCC_ERR_INVALID_ARG ||
        DCC_DM_SEND_TEXT(NULL, 333ULL, "hello") != DCC_ERR_INVALID_ARG ||
        DCC_DM_SEND_UI(NULL, 333ULL, DCC_UI_TEXT("hello")) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_LEAVE(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_INVITES_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_INVITE_FETCH(NULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_INVITE_FETCH_QUERY(NULL, "abc", "with_counts=true") != DCC_ERR_INVALID_ARG ||
        DCC_INVITE_FETCH_FULL(NULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_INVITE_DELETE(NULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_STAGE_CREATE(NULL, "{\"channel_id\":\"333\",\"topic\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_STAGE_CREATE_PARAMS(NULL, &stage_params) != DCC_ERR_INVALID_ARG ||
        DCC_STAGE_FETCH(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_STAGE_EDIT(NULL, 333ULL, "{\"topic\":\"sugar\"}") != DCC_ERR_INVALID_ARG ||
        DCC_STAGE_EDIT_PARAMS(NULL, &stage_params) != DCC_ERR_INVALID_ARG ||
        DCC_STAGE_DELETE(NULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_TEMPLATE_FETCH(NULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_TEMPLATE_CREATE_GUILD(NULL, "abc", "{\"name\":\"guild\"}") != DCC_ERR_INVALID_ARG ||
        DCC_TEMPLATE_CREATE_GUILD_PARAMS(NULL, &template_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_TEMPLATES_FETCH(NULL, 222ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_TEMPLATE_CREATE(NULL, 222ULL, "{\"name\":\"template\"}") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_TEMPLATE_CREATE_PARAMS(NULL, &template_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_TEMPLATE_EDIT(NULL, 222ULL, "abc", "{\"name\":\"template\"}") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_TEMPLATE_EDIT_PARAMS(NULL, &template_params) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_TEMPLATE_SYNC(NULL, 222ULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_TEMPLATE_DELETE(NULL, 222ULL, "abc") != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_MEMBER_CLEAR_TIMEOUT(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_MEMBER_MOVE(NULL, 222ULL, 333ULL, 444ULL) != DCC_ERR_INVALID_ARG ||
        DCC_GUILD_MEMBER_KICK(NULL, 222ULL, 333ULL) != DCC_ERR_INVALID_ARG) {
        fprintf(stderr, "sugar option macro field mismatch\n");
        return 1;
    }

    return 0;
}

static int run_sugar_context_smoke(void) {
    const char *component_values[] = { "alpha", "beta" };
    const char *form_values[] = { "red", "blue" };
    dcc_thread_params_t thread_params = {
        .size = sizeof(thread_params),
        .name = "thread",
    };
    dcc_interaction_option_t options[] = {
        {
            .name = "name",
            .value_type = DCC_INTERACTION_OPTION_VALUE_STRING,
            .string_value = "feral",
        },
        {
            .name = "limit",
            .value_type = DCC_INTERACTION_OPTION_VALUE_INTEGER,
            .integer_value = 25,
        },
        {
            .name = "ratio",
            .value_type = DCC_INTERACTION_OPTION_VALUE_NUMBER,
            .number_value = 0.5,
        },
        {
            .name = "visible",
            .value_type = DCC_INTERACTION_OPTION_VALUE_BOOLEAN,
            .boolean_value = 1U,
        },
        {
            .name = "user",
            .value_type = DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE,
            .snowflake_value = 123ULL,
        },
    };
    dcc_interaction_form_field_t form_fields[] = {
        {
            .custom_id = "name",
            .value_type = DCC_INTERACTION_FORM_VALUE_STRING,
            .value = "Feral",
        },
        {
            .custom_id = "visible",
            .value_type = DCC_INTERACTION_FORM_VALUE_BOOLEAN,
            .boolean_value = 1U,
        },
        {
            .custom_id = "choices",
            .value_type = DCC_INTERACTION_FORM_VALUE_VALUES,
            .values = form_values,
            .values_count = DCC_ARRAY_LEN(form_values),
        },
        {
            .custom_id = "empty",
            .value_type = DCC_INTERACTION_FORM_VALUE_VALUES,
        },
    };
    dcc_interaction_t interaction = {
        .options = options,
        .options_count = DCC_ARRAY_LEN(options),
        .values = component_values,
        .values_count = DCC_ARRAY_LEN(component_values),
        .form_fields = form_fields,
        .form_fields_count = DCC_ARRAY_LEN(form_fields),
        .custom_id = "birthday.register",
    };
    sugar_error_state_t error_state = {0};
    dcc_app_t app = {
        .error_handler = sugar_error_handler,
        .error_user_data = &error_state,
    };
    dcc_ctx_t ctx = {
        .app = &app,
        .interaction = &interaction,
    };
    size_t value_count = 777U;
    const char **values = DCC_CTX_SELECTED_VALUES(&ctx, &value_count);
    size_t field_value_count = 777U;
    const char **field_values = DCC_FIELD_VALUES_RAW(&ctx, "choices", &field_value_count);

    if (values != component_values ||
        value_count != 2U ||
        DCC_CTX_ARG(&ctx, "name") != &options[0] ||
        DCC_CTX_ARG_PRESENT(&ctx, "name") != 1U ||
        strcmp(DCC_CTX_ARG_STRING(&ctx, "name"), "feral") != 0 ||
        strcmp(DCC_CTX_ARG_STRING_OR(&ctx, "missing", "fallback"), "fallback") != 0 ||
        strcmp(DCC_CTX_ARG_TEXT(&ctx, "name"), "feral") != 0 ||
        DCC_CTX_ARG_INT(&ctx, "limit") != 25 ||
        DCC_CTX_ARG_INTEGER(&ctx, "limit") != 25 ||
        DCC_CTX_ARG_NUMBER(&ctx, "ratio") < 0.49 ||
        DCC_CTX_ARG_NUMBER(&ctx, "ratio") > 0.51 ||
        DCC_CTX_ARG_BOOL(&ctx, "visible") != 1U ||
        DCC_CTX_ARG_BOOLEAN(&ctx, "visible") != 1U ||
        DCC_CTX_ARG_SNOWFLAKE(&ctx, "user") != 123ULL ||
        strcmp(DCC_CTX_ARG_USER_MENTION(&ctx, "user"), "<@123>") != 0 ||
        strcmp(DCC_CTX_ARG_MEMBER_MENTION(&ctx, "user"), "<@!123>") != 0 ||
        DCC_CTX_OPTION(&ctx, "name") != &options[0] ||
        DCC_CTX_OPTION_PRESENT(&ctx, "name") != 1U ||
        strcmp(DCC_CTX_OPTION_TEXT(&ctx, "name"), "feral") != 0 ||
        DCC_CTX_OPTION_INTEGER(&ctx, "limit") != 25 ||
        DCC_CTX_OPTION_BOOLEAN(&ctx, "visible") != 1U ||
        strcmp(DCC_CTX_OPTION_USER_MENTION(&ctx, "user"), "<@123>") != 0 ||
        strcmp(DCC_CTX_CUSTOM_ID(&ctx), "birthday.register") != 0 ||
        strcmp(DCC_CTX_COMPONENT_ACTION(&ctx, "birthday"), "register") != 0 ||
        strcmp(DCC_CTX_COMPONENT_ACTION_OR(&ctx, "missing", "fallback"), "fallback") != 0 ||
        DCC_CTX_COMPONENT_IN_NS(&ctx, "birthday") != 1U ||
        DCC_CTX_COMPONENT_IN_NAMESPACE(&ctx, "birthday") != 1U ||
        DCC_CTX_COMPONENT_IN_NS(&ctx, "birth") != 0U ||
        DCC_CTX_COMPONENT_HAS_ACTION(&ctx, "birthday", "register") != 1U ||
        DCC_CTX_COMPONENT_IS(&ctx, "birthday", "register") != 1U ||
        DCC_CTX_COMPONENT_HAS_ACTION(&ctx, "birthday", "other") != 0U ||
        strcmp(DCC_CTX_ACTION_IN_NS(&ctx, "birthday"), "register") != 0 ||
        strcmp(DCC_CTX_ACTION_IN_NAMESPACE(&ctx, "birthday"), "register") != 0 ||
        DCC_CTX_SELECTED_COUNT(&ctx) != 2U ||
        strcmp(DCC_CTX_SELECTED_VALUE(&ctx, 0U), "alpha") != 0 ||
        strcmp(DCC_CTX_SELECTED_VALUE_AT(&ctx, 1U), "beta") != 0 ||
        strcmp(DCC_CTX_SELECTED_FIRST_OR(&ctx, "fallback"), "alpha") != 0 ||
        DCC_CTX_SELECTED_VALUE(&ctx, 2U) != NULL ||
        DCC_CTX_SELECT_COUNT(&ctx) != 2U ||
        strcmp(DCC_CTX_SELECT_VALUE_AT(&ctx, 0U), "alpha") != 0 ||
        strcmp(DCC_CTX_SELECT_FIRST(&ctx), "alpha") != 0 ||
        DCC_CTX_SELECT_VALUES(&ctx, NULL) != component_values ||
        DCC_SELECTED_COUNT(&ctx) != 2U ||
        strcmp(DCC_SELECTED_VALUE(&ctx, 1U), "beta") != 0 ||
        strcmp(DCC_SELECTED_FIRST(&ctx), "alpha") != 0 ||
        DCC_SELECTED_VALUES(&ctx, NULL) != component_values ||
        DCC_SELECT_COUNT(&ctx) != 2U ||
        strcmp(DCC_SELECT_VALUE_AT(&ctx, 0U), "alpha") != 0 ||
        strcmp(DCC_SELECT_FIRST_OR(&ctx, "fallback"), "alpha") != 0 ||
        DCC_SELECT_VALUES_RAW(&ctx, NULL) != component_values ||
        field_values != form_values ||
        field_value_count != 2U ||
        DCC_CTX_FORM_VALUE_COUNT(&ctx, "choices") != 2U ||
        strcmp(DCC_CTX_FORM_VALUE(&ctx, "choices", 1U), "blue") != 0 ||
        strcmp(DCC_CTX_FORM_FIRST_VALUE(&ctx, "choices", "fallback"), "red") != 0 ||
        DCC_CTX_FORM_PRESENT(&ctx, "name") != 1U ||
        strcmp(DCC_CTX_FORM_STRING_OR(&ctx, "name", ""), "Feral") != 0 ||
        strcmp(DCC_CTX_FORM_TEXT_VALUE(&ctx, "name"), "Feral") != 0 ||
        DCC_CTX_FORM_BOOL_OR(&ctx, "visible", 0U) != 1U ||
        DCC_CTX_FORM_IS_CHECKED(&ctx, "visible") != 1U ||
        DCC_CTX_FIELD_VALUES(&ctx, "choices", NULL) != form_values ||
        DCC_CTX_FIELD_PRESENT(&ctx, "name") != 1U ||
        strcmp(DCC_CTX_FIELD_TEXT(&ctx, "name"), "Feral") != 0 ||
        strcmp(DCC_CTX_FIELD_STRING(&ctx, "name"), "Feral") != 0 ||
        DCC_CTX_FIELD_BOOL(&ctx, "visible") != 1U ||
        DCC_CTX_FIELD_CHECKED(&ctx, "visible") != 1U ||
        DCC_CTX_FIELD_VALUE_COUNT(&ctx, "choices") != 2U ||
        strcmp(DCC_CTX_FIELD_VALUE_AT(&ctx, "choices", 1U), "blue") != 0 ||
        strcmp(DCC_CTX_FIELD_FIRST_VALUE(&ctx, "choices"), "red") != 0 ||
        DCC_CTX_FIELD_VALUES(&ctx, "choices", NULL) != form_values ||
        DCC_CTX_FORM_VALUE(&ctx, "choices", 2U) != NULL ||
        DCC_FIELD_VALUE_COUNT(&ctx, "choices") != 2U ||
        strcmp(DCC_FIELD_VALUE_AT(&ctx, "choices", 0U), "red") != 0 ||
        strcmp(DCC_FIELD_FIRST_VALUE(&ctx, "choices"), "red") != 0 ||
        strcmp(DCC_FIELD_FIRST_VALUE_OR(&ctx, "missing", "fallback"), "fallback") != 0 ||
        DCC_FIELD_VALUES_RAW(&ctx, "empty", NULL) != NULL) {
        fprintf(stderr, "sugar context value accessor mismatch\n");
        return 1;
    }

    if (DCC_CTX_HANDLE_ERROR(NULL, DCC_OK, NULL) != DCC_OK ||
        DCC_CTX_HANDLE_ERROR(NULL, DCC_ERR_STATE, "bad") != DCC_ERR_INVALID_ARG ||
        DCC_CTX_HANDLE_ERROR(&ctx, DCC_ERR_STATE, "handled") != DCC_ERR_STATE ||
        error_state.count != 1U ||
        error_state.status != DCC_ERR_STATE ||
        strcmp(error_state.message, "handled") != 0) {
        fprintf(stderr, "sugar context error handler mismatch\n");
        return 1;
    }
    sugar_try_ok(&ctx, &error_state);
    if (error_state.after_try != 1U || error_state.count != 1U) {
        fprintf(stderr, "sugar context try ok mismatch\n");
        return 1;
    }
    error_state.after_try = 0U;
    sugar_try_failed(&ctx, &error_state);
    if (error_state.after_try != 0U ||
        error_state.count != 2U ||
        error_state.status != DCC_ERR_STATE ||
        strcmp(error_state.message, "custom failure") != 0) {
        fprintf(stderr, "sugar context try failure mismatch\n");
        return 1;
    }

    value_count = 777U;
    if (DCC_CTX_SELECTED_VALUES(NULL, &value_count) != NULL || value_count != 0U) {
        fprintf(stderr, "sugar context component NULL count mismatch\n");
        return 1;
    }
    field_value_count = 777U;
    if (DCC_FIELD_VALUES_RAW(NULL, "choices", &field_value_count) != NULL ||
        field_value_count != 0U) {
        fprintf(stderr, "sugar context field NULL count mismatch\n");
        return 1;
    }

    if (DCC_CTX_APP(NULL) != NULL ||
        DCC_CTX_CLIENT(NULL) != NULL ||
        DCC_CTX_EVENT(NULL) != NULL ||
        DCC_CTX_INTERACTION(NULL) != NULL ||
        DCC_CTX_FLOW(NULL) != NULL ||
        DCC_CTX_EVENT_TYPE(NULL) != DCC_EVENT_MAX ||
        DCC_CTX_INTERACTION_ID(NULL) != 0U ||
        DCC_CTX_APPLICATION_ID(NULL) != 0U ||
        DCC_CTX_GUILD_ID(NULL) != 0U ||
        DCC_CTX_CHANNEL_ID(NULL) != 0U ||
        DCC_CTX_COMMAND_ID(NULL) != 0U ||
        DCC_CTX_MESSAGE_ID(NULL) != 0U ||
        DCC_CTX_TARGET_ID(NULL) != 0U ||
        DCC_CTX_USER_ID(NULL) != 0U ||
        DCC_CTX_AUTHOR_ID(NULL) != 0U ||
        DCC_CTX_USER_DATA(NULL) != NULL ||
        DCC_CTX_TOKEN(NULL) != NULL ||
        DCC_CTX_NAME(NULL) != NULL ||
        DCC_CTX_COMMAND_NAME(NULL) != NULL ||
        DCC_CTX_CUSTOM_ID(NULL) != NULL ||
        DCC_CTX_SUBCOMMAND(NULL) != NULL ||
        DCC_CTX_SUBCOMMAND_GROUP(NULL) != NULL ||
        DCC_CTX_RESPONSE_STATE(NULL) != DCC_INTERACTION_FLOW_FAILED ||
        strcmp(DCC_CTX_RESPONSE_STATE_NAME(NULL), "failed") != 0 ||
        DCC_CTX_RESPONDED(NULL) != 0U ||
        DCC_CTX_RESPONSE_SENT(NULL) != 0U ||
        DCC_CTX_DEFERRED(NULL) != 0U ||
        DCC_CTX_REPLIED(NULL) != 0U ||
        DCC_CTX_FOLLOWED_UP(NULL) != 0U ||
        DCC_CTX_RESPONSE_FAILED(NULL) != 1U ||
        DCC_CTX_CAN_FOLLOWUP(NULL) != 0U ||
        DCC_CTX_CAN_EDIT_ORIGINAL(NULL) != 0U ||
        DCC_CTX_HAS_MEMBER_PERMISSIONS(NULL) != 0U ||
        DCC_CTX_MEMBER_PERMISSIONS(NULL) != 0U ||
        DCC_CTX_MEMBER_HAS_PERMISSIONS(NULL, DCC_PERMISSION_MANAGE_MESSAGES) != 0U ||
        DCC_CTX_HAS_PERMISSIONS(NULL, DCC_PERMISSION_MANAGE_MESSAGES) != 0U ||
        DCC_CTX_IS_GUILD(NULL) != 0U ||
        DCC_CTX_IS_DM(NULL) != 0U ||
        DCC_CTX_IS_AUTHOR(NULL, 123ULL) != 0U ||
        strcmp(DCC_CTX_NAME_OR(NULL, "fallback"), "fallback") != 0 ||
        strcmp(DCC_CTX_TOKEN_OR(NULL, "token"), "token") != 0 ||
        strcmp(DCC_CTX_CUSTOM_ID_OR(NULL, "custom"), "custom") != 0 ||
        DCC_CTX_HAS_CUSTOM_ID(NULL, "custom") != 0U ||
        DCC_CTX_CUSTOM_ID_STARTS_WITH(NULL, "prefix:") != 0U ||
        DCC_CTX_CUSTOM_ID_SUFFIX(NULL, "prefix:") != NULL ||
        DCC_CTX_ACTION(NULL, "prefix:") != NULL ||
        DCC_CTX_COMPONENT_ACTION(NULL, "birthday") != NULL ||
        strcmp(DCC_CTX_COMPONENT_ACTION_OR(NULL, "birthday", "fallback"), "fallback") != 0 ||
        DCC_CTX_COMPONENT_IN_NS(NULL, "birthday") != 0U ||
        DCC_CTX_COMPONENT_HAS_ACTION(NULL, "birthday", "register") != 0U ||
        strcmp(DCC_CTX_USER_MENTION(NULL), "") != 0 ||
        strcmp(DCC_CTX_AUTHOR_MENTION(NULL), "") != 0 ||
        strcmp(DCC_CTX_MEMBER_MENTION(NULL), "") != 0 ||
        strcmp(DCC_CTX_CHANNEL_MENTION(NULL), "") != 0 ||
        strcmp(DCC_CTX_TARGET_MENTION(NULL), "") != 0 ||
        strcmp(DCC_CTX_TARGET_USER_MENTION(NULL), "") != 0 ||
        strcmp(DCC_CTX_TARGET_MEMBER_MENTION(NULL), "") != 0 ||
        strcmp(DCC_CTX_TARGET_ROLE_MENTION(NULL), "") != 0 ||
        strcmp(DCC_CTX_TARGET_CHANNEL_MENTION(NULL), "") != 0 ||
        strcmp(DCC_CTX_COMMAND_MENTION(NULL), "") != 0 ||
        strcmp(DCC_CTX_MESSAGE_LINK(NULL), "") != 0 ||
        DCC_CTX_FOCUSED_OPTION(NULL) != NULL ||
        DCC_CTX_FOCUSED_NAME(NULL) != NULL ||
        strcmp(DCC_CTX_FOCUSED_STRING(NULL, "fallback"), "fallback") != 0 ||
        DCC_CTX_OPT_PRESENT(NULL, "name") != 0U ||
        strcmp(DCC_CTX_OPT_STRING(NULL, "name", "fallback"), "fallback") != 0 ||
        DCC_CTX_OPT_INT(NULL, "count", 7) != 7 ||
        DCC_CTX_OPT_NUMBER(NULL, "ratio", 0.25) < 0.24 ||
        DCC_CTX_OPT_NUMBER(NULL, "ratio", 0.25) > 0.26 ||
        DCC_CTX_OPT_BOOL(NULL, "visible", 1U) != 1U ||
        DCC_CTX_OPT_SNOWFLAKE(NULL, "user", 123ULL) != 123ULL ||
        DCC_CTX_ARG(NULL, "name") != NULL ||
        DCC_CTX_ARG_PRESENT(NULL, "name") != 0U ||
        DCC_CTX_ARG_STRING(NULL, "name") != NULL ||
        strcmp(DCC_CTX_ARG_STRING_OR(NULL, "name", "fallback"), "fallback") != 0 ||
        DCC_CTX_ARG_INT(NULL, "count") != 0 ||
        DCC_CTX_ARG_INT_OR(NULL, "count", 7) != 7 ||
        DCC_CTX_ARG_NUMBER(NULL, "ratio") != 0.0 ||
        DCC_CTX_ARG_NUMBER_OR(NULL, "ratio", 0.25) < 0.24 ||
        DCC_CTX_ARG_NUMBER_OR(NULL, "ratio", 0.25) > 0.26 ||
        DCC_CTX_ARG_BOOL(NULL, "visible") != 0U ||
        DCC_CTX_ARG_BOOL_OR(NULL, "visible", 1U) != 1U ||
        DCC_CTX_ARG_SNOWFLAKE(NULL, "user") != 0U ||
        DCC_CTX_ARG_SNOWFLAKE_OR(NULL, "user", 123ULL) != 123ULL ||
        DCC_CTX_ARG_USER(NULL, "user") != NULL ||
        strcmp(DCC_CTX_ARG_USER_MENTION(NULL, "user"), "") != 0 ||
        DCC_CTX_OPTION(NULL, "name") != NULL ||
        DCC_CTX_OPTION_PRESENT(NULL, "name") != 0U ||
        DCC_CTX_OPTION_TEXT(NULL, "name") != NULL ||
        strcmp(DCC_CTX_OPTION_TEXT_OR(NULL, "name", "fallback"), "fallback") != 0 ||
        DCC_CTX_OPTION_INTEGER(NULL, "count") != 0 ||
        DCC_CTX_OPTION_INTEGER_OR(NULL, "count", 7) != 7 ||
        DCC_CTX_OPTION_BOOLEAN(NULL, "visible") != 0U ||
        DCC_CTX_OPTION_BOOLEAN_OR(NULL, "visible", 1U) != 1U ||
        strcmp(DCC_CTX_OPTION_USER_MENTION(NULL, "user"), "") != 0 ||
        DCC_PARAM_PRESENT(NULL, "name") != 0U ||
        DCC_PARAM_STRING(NULL, "name") != NULL ||
        strcmp(DCC_PARAM_STRING_OR(NULL, "name", "fallback"), "fallback") != 0 ||
        DCC_PARAM_INT(NULL, "count") != 0 ||
        DCC_PARAM_INT_OR(NULL, "count", 7) != 7 ||
        DCC_PARAM_INTEGER(NULL, "count") != 0 ||
        DCC_PARAM_INTEGER_OR(NULL, "count", 8) != 8 ||
        DCC_PARAM_NUMBER(NULL, "ratio") != 0.0 ||
        DCC_PARAM_NUMBER_OR(NULL, "ratio", 0.25) < 0.24 ||
        DCC_PARAM_NUMBER_OR(NULL, "ratio", 0.25) > 0.26 ||
        DCC_PARAM_BOOL(NULL, "visible") != 0U ||
        DCC_PARAM_BOOL_OR(NULL, "visible", 1U) != 1U ||
        DCC_PARAM_BOOLEAN(NULL, "visible") != 0U ||
        DCC_PARAM_BOOLEAN_OR(NULL, "visible", 1U) != 1U ||
        DCC_PARAM_SNOWFLAKE(NULL, "user") != 0U ||
        DCC_PARAM_SNOWFLAKE_OR(NULL, "user", 123ULL) != 123ULL ||
        DCC_PARAM_USER(NULL, "user") != NULL ||
        DCC_PARAM_MEMBER(NULL, "user") != NULL ||
        DCC_PARAM_ROLE(NULL, "role") != NULL ||
        DCC_PARAM_CHANNEL(NULL, "channel") != NULL ||
        DCC_PARAM_MESSAGE(NULL, "message") != NULL ||
        DCC_PARAM_ATTACHMENT(NULL, "attachment") != NULL ||
        strcmp(DCC_OPTION_MENTION(NULL, "user"), "") != 0 ||
        strcmp(DCC_OPTION_USER_MENTION(NULL, "user"), "") != 0 ||
        strcmp(DCC_OPTION_MEMBER_MENTION(NULL, "user"), "") != 0 ||
        strcmp(DCC_OPTION_ROLE_MENTION(NULL, "role"), "") != 0 ||
        strcmp(DCC_OPTION_CHANNEL_MENTION(NULL, "channel"), "") != 0 ||
        strcmp(DCC_PARAM_MENTION(NULL, "user"), "") != 0 ||
        strcmp(DCC_PARAM_USER_MENTION(NULL, "user"), "") != 0 ||
        strcmp(DCC_PARAM_MEMBER_MENTION(NULL, "user"), "") != 0 ||
        strcmp(DCC_PARAM_ROLE_MENTION(NULL, "role"), "") != 0 ||
        strcmp(DCC_PARAM_CHANNEL_MENTION(NULL, "channel"), "") != 0 ||
        DCC_PARAM_FOCUSED(NULL) != NULL ||
        DCC_PARAM_FOCUSED_NAME(NULL) != NULL ||
        DCC_PARAM_FOCUSED_STRING(NULL) != NULL ||
        strcmp(DCC_PARAM_FOCUSED_STRING_OR(NULL, "fallback"), "fallback") != 0 ||
        DCC_CTX_FORM_PRESENT(NULL, "birthday") != 0U ||
        strcmp(DCC_CTX_FORM_STRING_OR(NULL, "birthday", "20070101"), "20070101") != 0 ||
        strcmp(DCC_CTX_FORM_TEXT(NULL, "birthday", "20070101"), "20070101") != 0 ||
        strcmp(DCC_CTX_FORM_TEXT_OR(NULL, "birthday", "20070101"), "20070101") != 0 ||
        DCC_CTX_FORM_TEXT_VALUE(NULL, "birthday") != NULL ||
        DCC_CTX_FORM_BOOL_OR(NULL, "public", 1U) != 1U ||
        DCC_CTX_FORM_CHECKED(NULL, "public", 1U) != 1U ||
        DCC_CTX_FORM_CHECKED_OR(NULL, "public", 1U) != 1U ||
        DCC_CTX_FORM_IS_CHECKED(NULL, "public") != 0U ||
        DCC_CTX_FIELD_PRESENT(NULL, "birthday") != 0U ||
        DCC_CTX_FIELD_TEXT(NULL, "birthday") != NULL ||
        strcmp(DCC_CTX_FIELD_TEXT_OR(NULL, "birthday", "20070101"), "20070101") != 0 ||
        DCC_CTX_FIELD_BOOL(NULL, "public") != 0U ||
        DCC_CTX_FIELD_BOOL_OR(NULL, "public", 1U) != 1U ||
        DCC_CTX_FIELD_CHECKED(NULL, "public") != 0U ||
        DCC_CTX_FIELD_CHECKED_OR(NULL, "public", 1U) != 1U ||
        DCC_FIELD_TEXT(NULL, "birthday") != NULL ||
        strcmp(DCC_FIELD_TEXT_OR(NULL, "birthday", "20070101"), "20070101") != 0 ||
        DCC_FIELD_CHECKED(NULL, "public") != 0U ||
        DCC_FIELD_CHECKED_OR(NULL, "public", 1U) != 1U ||
        DCC_CTX_FORM_VALUE_COUNT(NULL, "choices") != 0U ||
        DCC_CTX_FORM_VALUE(NULL, "choices", 0U) != NULL ||
        strcmp(DCC_CTX_FORM_FIRST_VALUE(NULL, "choices", "fallback"), "fallback") != 0 ||
        DCC_FIELD_VALUE_COUNT(NULL, "choices") != 0U ||
        DCC_FIELD_VALUE_AT(NULL, "choices", 0U) != NULL ||
        DCC_FIELD_FIRST_VALUE(NULL, "choices") != NULL ||
        strcmp(DCC_FIELD_FIRST_VALUE_OR(NULL, "choices", "fallback"), "fallback") != 0 ||
        DCC_CTX_FIELD_VALUE_COUNT(NULL, "choices") != 0U ||
        DCC_CTX_FIELD_VALUE_AT(NULL, "choices", 0U) != NULL ||
        DCC_CTX_FIELD_FIRST_VALUE(NULL, "choices") != NULL ||
        strcmp(DCC_CTX_FIELD_FIRST_VALUE_OR(NULL, "choices", "fallback"), "fallback") != 0 ||
        DCC_CTX_SELECTED_COUNT(NULL) != 0U ||
        DCC_CTX_SELECTED_VALUE(NULL, 0U) != NULL ||
        DCC_CTX_SELECTED_VALUE_AT(NULL, 0U) != NULL ||
        DCC_CTX_SELECTED_FIRST(NULL) != NULL ||
        strcmp(DCC_CTX_SELECTED_FIRST_OR(NULL, "fallback"), "fallback") != 0 ||
        DCC_CTX_SELECT_COUNT(NULL) != 0U ||
        DCC_CTX_SELECT_VALUE(NULL, 0U) != NULL ||
        DCC_CTX_SELECT_VALUE_AT(NULL, 0U) != NULL ||
        DCC_CTX_SELECT_FIRST(NULL) != NULL ||
        strcmp(DCC_CTX_SELECT_FIRST_OR(NULL, "fallback"), "fallback") != 0 ||
        DCC_SELECTED_COUNT(NULL) != 0U ||
        DCC_SELECTED_VALUE(NULL, 0U) != NULL ||
        DCC_SELECTED_FIRST(NULL) != NULL ||
        strcmp(DCC_SELECTED_FIRST_OR(NULL, "fallback"), "fallback") != 0 ||
        DCC_SELECT_COUNT(NULL) != 0U ||
        DCC_SELECT_VALUE_AT(NULL, 0U) != NULL ||
        DCC_SELECT_FIRST(NULL) != NULL ||
        strcmp(DCC_SELECT_FIRST_OR(NULL, "fallback"), "fallback") != 0 ||
        DCC_CTX_ADD_MEMBER_ROLE(NULL, 1ULL, 2ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_REMOVE_MEMBER_ROLE(NULL, 1ULL, 2ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_ADD_AUTHOR_ROLE(NULL, 2ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_REMOVE_AUTHOR_ROLE(NULL, 2ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_MEMBER_ADD_ROLE(NULL, 1ULL, 2ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_MEMBER_REMOVE_ROLE(NULL, 1ULL, 2ULL) != DCC_ERR_INVALID_ARG ||
        DCC_AUTHOR_ADD_ROLE(NULL, 2ULL) != DCC_ERR_INVALID_ARG ||
        DCC_AUTHOR_REMOVE_ROLE(NULL, 2ULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_THREAD_FROM_MESSAGE(NULL, "thread") != DCC_ERR_INVALID_ARG ||
        DCC_CTX_THREAD_FROM_MESSAGE_PARAMS(NULL, &thread_params) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_CREATE_THREAD(NULL, "thread") != DCC_ERR_INVALID_ARG ||
        DCC_CTX_CREATE_THREAD_PARAMS(NULL, &thread_params) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_ARCHIVE_THREAD(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_LOCK_THREAD(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_UNLOCK_THREAD(NULL) != DCC_ERR_INVALID_ARG ||
        DCC_CTX_CLOSE_THREAD(NULL) != DCC_ERR_INVALID_ARG ||
        !dcc_sugar_ctx_text_eq("same", "same") ||
        dcc_sugar_ctx_text_eq("left", "right") ||
        !dcc_sugar_ctx_text_starts_with("prefix:value", "prefix:") ||
        strcmp(dcc_sugar_ctx_text_suffix_after("prefix:value", "prefix:"), "value") != 0) {
        fprintf(stderr, "sugar context macro mismatch\n");
        return 1;
    }
    return 0;
}

static int run_sugar_store_smoke(void) {
    const char *path = "dcc_sugar_smoke_store.kv";
    dcc_app_t app;
    dcc_ctx_t ctx;
    char value[32];
    size_t value_len = 0U;

    memset(&app, 0, sizeof(app));
    memset(&ctx, 0, sizeof(ctx));
    ctx.app = &app;
    (void)remove(path);

    if (DCC_CTX_STORE(NULL) != NULL ||
        DCC_CTX_HAS_STORE(NULL) != 0U ||
        DCC_APP_HAS_STORE(NULL) != 0U ||
        DCC_CTX_STORE(&ctx) != NULL ||
        DCC_CTX_HAS_STORE(&ctx) != 0U ||
        DCC_CTX_STORE_U64_OR(NULL, "missing", 7U) != 7U ||
        DCC_CTX_STORE_U64_OR(&ctx, "missing", 8U) != 8U ||
        DCC_CTX_STORE_SET_U64(NULL, "missing", 1U) != DCC_ERR_STATE ||
        DCC_APP_STORE_GET_U64(NULL, "missing", NULL) != DCC_ERR_STATE) {
        fprintf(stderr, "sugar store null fallback mismatch\n");
        return 1;
    }

    if (DCC_APP_STORE_FILE(&app, path) != DCC_OK ||
        DCC_CTX_STORE(&ctx) != DCC_APP_STORE(&app) ||
        DCC_CTX_HAS_STORE(&ctx) != 1U ||
        DCC_APP_HAS_STORE(&app) != 1U ||
        DCC_APP_STORE_U64_OR(&app, "count", 5U) != 5U ||
        DCC_CTX_STORE_U64_OR(&ctx, "count", 6U) != 6U ||
        DCC_CTX_STORE_SET_U64(&ctx, "count", 42U) != DCC_OK ||
        DCC_APP_STORE_U64_OR(&app, "count", 0U) != 42U ||
        DCC_APP_STORE_SET(&app, "name", "dcc") != DCC_OK ||
        DCC_CTX_STORE_GET(&ctx, "name", value, sizeof(value), &value_len) != DCC_OK ||
        value_len != 3U ||
        strcmp(value, "dcc") != 0 ||
        DCC_CTX_STORE_DELETE(&ctx, "name") != DCC_OK ||
        DCC_APP_STORE_GET(&app, "name", value, sizeof(value), NULL) != DCC_ERR_NOT_FOUND) {
        fprintf(stderr, "sugar store macro mismatch\n");
        DCC_APP_STORE_CLOSE(&app);
        (void)remove(path);
        return 1;
    }

    DCC_APP_STORE_CLOSE(&app);
    (void)remove(path);
    return 0;
}

static void sugar_store_require_compile_smoke(dcc_ctx_t *ctx) {
    DCC_CTX_REQUIRE_STORE(ctx, "Store is not configured.");
    DCC_REQUIRE_STORE(ctx, "Store is not configured.");
}

static int run_sugar_typed_handler_impl_smoke(void) {
    sugar_typed_impl_args_t args = {
        .value = "ok",
        .count = 7U
    };
    uint32_t total = 0U;

    sugar_typed_impl_handler(NULL, &args, &total);
    sugar_typed_impl_handler(NULL, NULL, &total);
    sugar_typed_mut_impl_handler(NULL, &args, NULL);
    sugar_typed_mut_impl_handler(NULL, NULL, NULL);
    sugar_no_args_command_handler(NULL, NULL, &total);
    sugar_no_args_command_handler(NULL, &args, &total);

    if (total != 9U || args.count != 8U) {
        fprintf(stderr, "typed handler impl sugar mismatch\n");
        return 1;
    }
    return 0;
}

int main(void) {
    sugar_guard_compile_smoke(NULL);
    sugar_store_require_compile_smoke(NULL);

    if (run_sugar_context_smoke() != 0) {
        return 1;
    }
    if (run_sugar_store_smoke() != 0) {
        return 1;
    }
    if (run_sugar_context_display_smoke() != 0) {
        return 1;
    }
    if (run_sugar_reply_smoke() != 0) {
        return 1;
    }
    if (run_sugar_message_smoke() != 0) {
        return 1;
    }
    if (run_sugar_legacy_component_smoke() != 0) {
        return 1;
    }
    if (run_sugar_component_v2_smoke() != 0) {
        return 1;
    }
    if (run_sugar_modal_smoke() != 0) {
        return 1;
    }
    if (run_sugar_command_smoke() != 0) {
        return 1;
    }
    if (run_sugar_array_compat_smoke() != 0) {
        return 1;
    }
    if (run_sugar_options_smoke() != 0) {
        return 1;
    }
    if (run_sugar_typed_handler_impl_smoke() != 0) {
        return 1;
    }
    return 0;
}
