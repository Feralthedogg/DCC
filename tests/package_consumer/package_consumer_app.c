#include "package_consumer_checks.h"

#include <dcc/sugar.h>

#include <string.h>

static dcc_status_t dcc_package_consumer_app_middleware(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
    return DCC_OK;
}

static dcc_status_t dcc_package_consumer_app_check(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
    return DCC_OK;
}

static void dcc_package_consumer_app_handler(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
}

static void dcc_package_consumer_app_typed_handler(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)ctx;
    (void)args;
    (void)user_data;
}

static void dcc_package_consumer_app_error_handler(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const char *message,
    void *user_data
) {
    (void)ctx;
    (void)status;
    (void)message;
    (void)user_data;
}

static void dcc_package_consumer_app_event_handler(
    dcc_app_t *app,
    const dcc_event_t *event,
    void *user_data
) {
    (void)app;
    (void)event;
    (void)user_data;
}

static void dcc_package_consumer_app_ready_handler(
    dcc_app_t *app,
    const dcc_ready_event_t *ready,
    void *user_data
) {
    (void)app;
    (void)ready;
    (void)user_data;
}

static void dcc_package_consumer_app_message_handler(
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

static void dcc_package_consumer_app_message_command_handler(
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

static void dcc_package_consumer_app_message_id_cb(
    dcc_app_t *app,
    const dcc_rest_response_t *response,
    dcc_snowflake_t message_id,
    dcc_status_t status,
    void *user_data
) {
    (void)app;
    (void)response;
    (void)message_id;
    (void)status;
    (void)user_data;
}

DCC_PREFIX_COMMAND_FN(dcc_package_consumer_prefix_command_alias) {
    (void)app;
    (void)message;
    (void)args;
    (void)event;
    (void)user_data;
}

static void dcc_package_consumer_app_task_handler(dcc_app_t *app, void *user_data) {
    (void)app;
    (void)user_data;
}

static void dcc_package_consumer_validation_reply_helper(void) {
    typedef struct helper_args {
        const char *birthday;
    } helper_args_t;
    helper_args_t args = {
        .birthday = "2007",
    };
    DCC_CTX_VALIDATE_OR_REPLY(
        NULL,
        &args,
        DCC_VALIDATE_BIRTHDAY(helper_args_t, birthday, "birthday")
    );
}

static void dcc_package_consumer_ctx_require_helper(dcc_ctx_t *ctx) {
    dcc_snowflake_t roles[] = { 1ULL, 2ULL };
    if (ctx != NULL) {
        DCC_CTX_REQUIRE(ctx, 1, "blocked");
        DCC_CTX_REQUIRE_GUILD(ctx, "Guild only.");
        DCC_CTX_REQUIRE_DM(ctx, "DM only.");
        DCC_CTX_REQUIRE_AUTHOR(ctx, 1ULL, "Author only.");
        DCC_CTX_REQUIRE_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_MESSAGES, "Missing permissions.");
        DCC_CTX_REQUIRE_CAN(ctx, DCC_PERMISSION_MANAGE_MESSAGES, "Missing permissions.");
        DCC_CTX_REQUIRE_CAN_ANY(ctx, DCC_PERMISSION_MODERATION_BASIC, "Missing permissions.");
        DCC_CTX_REQUIRE_ROLE(ctx, 1ULL, "Missing role.");
        DCC_CTX_REQUIRE_ANY_ROLE(ctx, roles, DCC_ARRAY_LEN(roles), "Missing role.");
        DCC_CTX_REQUIRE_ALL_ROLES(ctx, roles, DCC_ARRAY_LEN(roles), "Missing role.");
        DCC_CTX_REQUIRE_ANY_ROLE_IDS(ctx, "Missing role.", 1ULL, 2ULL);
        DCC_CTX_REQUIRE_ALL_ROLE_IDS(ctx, "Missing role.", 1ULL, 2ULL);
        DCC_CTX_REQUIRE_TEXT(ctx, "value", "Missing text.");
        DCC_CTX_REQUIRE_OPTION(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_FIELD(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_SELECTED(ctx, "Choose at least one option.");
        DCC_CTX_REQUIRE_BAD_INPUT(ctx, 1, "Invalid input.");
        DCC_CTX_REQUIRE_BAD_INPUT_F(ctx, 1, "Invalid input %d.", 1);
        DCC_CTX_REQUIRE_DENY(ctx, 1, "Permission denied.");
        DCC_CTX_REQUIRE_DENY_F(ctx, 1, "Permission denied %d.", 1);
        DCC_CTX_REQUIRE_NOT_FOUND(ctx, 1, "Not found.");
        DCC_CTX_REQUIRE_NOT_FOUND_F(ctx, 1, "Not found %d.", 1);
        DCC_CTX_REQUIRE_BUSY(ctx, 1, "Please wait.");
        DCC_CTX_REQUIRE_BUSY_F(ctx, 1, "Please wait %d.", 1);
        DCC_CTX_REQUIRE_FAIL(ctx, 1, "Failed.");
        DCC_CTX_REQUIRE_FAIL_F(ctx, 1, "Failed %d.", 1);
        DCC_CTX_REQUIRE_INTERNAL(ctx, 1);
        DCC_CTX_REQUIRE_ARG_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_ARG_STRING_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_CTX_REQUIRE_FORM_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_FORM_STRING_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_CTX_REQUIRE_SELECT_BAD_INPUT(ctx, "Choose at least one option.");
        DCC_CTX_REQUIRE_CUSTOM_ID(ctx, "custom.id", "Wrong component.");
        DCC_CTX_REQUIRE_CUSTOM_ID_PREFIX(ctx, "custom.", "Wrong component.");
        DCC_REQUIRE_BAD_INPUT(ctx, 1, "Invalid input.");
        DCC_REQUIRE_BAD_INPUT_F(ctx, 1, "Invalid input %d.", 1);
        DCC_REQUIRE_DENY(ctx, 1, "Permission denied.");
        DCC_REQUIRE_DENY_F(ctx, 1, "Permission denied %d.", 1);
        DCC_REQUIRE_NOT_FOUND(ctx, 1, "Not found.");
        DCC_REQUIRE_NOT_FOUND_F(ctx, 1, "Not found %d.", 1);
        DCC_REQUIRE_BUSY(ctx, 1, "Please wait.");
        DCC_REQUIRE_BUSY_F(ctx, 1, "Please wait %d.", 1);
        DCC_REQUIRE_FAIL(ctx, 1, "Failed.");
        DCC_REQUIRE_FAIL_F(ctx, 1, "Failed %d.", 1);
        DCC_REQUIRE_INTERNAL(ctx, 1);
        DCC_REQUIRE_CAN(ctx, DCC_PERMISSION_MANAGE_MESSAGES, "Missing permissions.");
        DCC_REQUIRE_CAN_ANY(ctx, DCC_PERMISSION_MODERATION_BASIC, "Missing permissions.");
        DCC_REQUIRE_TEXT(ctx, "value", "Missing text.");
        DCC_REQUIRE_OPTION(ctx, "name", "Missing option.");
        DCC_REQUIRE_FIELD(ctx, "field", "Missing field.");
        DCC_REQUIRE_SELECTED(ctx, "Choose at least one option.");
        DCC_REQUIRE_ARG_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_REQUIRE_ARG_TEXT_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_REQUIRE_ARG_STRING_BAD_INPUT(ctx, "name", "Missing option.");
        DCC_REQUIRE_FORM_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_REQUIRE_FORM_TEXT_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_REQUIRE_FORM_STRING_BAD_INPUT(ctx, "field", "Missing field.");
        DCC_REQUIRE_SELECT_BAD_INPUT(ctx, "Choose at least one option.");
        DCC_REQUIRE_CUSTOM_ID(ctx, "custom.id", "Wrong component.");
        DCC_REQUIRE_CUSTOM_ID_PREFIX(ctx, "custom.", "Wrong component.");
    }
}

static void dcc_package_consumer_try_ok_helper(uint8_t *after_try) {
    DCC_CTX_TRY(NULL, DCC_OK);
    if (after_try != NULL) {
        *after_try = 1U;
    }
}

typedef struct dcc_package_consumer_app_state {
    int setup_count;
    int cleanup_count;
} dcc_package_consumer_app_state_t;

typedef struct dcc_package_consumer_app_args {
    const char *name;
    int64_t limit;
    const dcc_user_t *user;
    const dcc_role_t *role;
    uint8_t visible;
    const char **tags;
    size_t tag_count;
} dcc_package_consumer_app_args_t;

#define DCC_PACKAGE_CONSUMER_APP_PARAMS(PARAM_, type_) \
    PARAM_(type_, REQUIRED_STRING, name, "name", "Name") \
    PARAM_(type_, INT_RANGE, limit, "limit", "Limit", 0U, 25, 1, 100) \
    PARAM_(type_, USER, user, "user", "User", 0U) \
    PARAM_(type_, REQUIRED_ROLE, role, "role", "Role") \
    PARAM_(type_, BOOL, visible, "visible", "Visible", 0U, 1U)

#define DCC_PACKAGE_CONSUMER_MODAL_PARAMS(PARAM_, type_) \
    PARAM_(type_, BOOL, visible, "visible", 1U) \
    PARAM_(type_, REQUIRED_VALUES, tags, tag_count, "tags")

#define DCC_PACKAGE_CONSUMER_BUTTON_PARAMS(PARAM_, type_) \
    PARAM_(type_, REQUIRED_CUSTOM_ID_SUFFIX, name, "package.params.button:")

#define DCC_PACKAGE_CONSUMER_NS_BUTTON_PARAMS(PARAM_, type_) \
    PARAM_(type_, REQUIRED_CUSTOM_ID_SUFFIX, name, "package.ns.button:")

#define DCC_PACKAGE_CONSUMER_SELECT_PARAMS(PARAM_, type_) \
    PARAM_(type_, REQUIRED_FIRST_VALUE, name) \
    PARAM_(type_, REQUIRED_VALUES, tags, tag_count)

static dcc_status_t dcc_package_consumer_app_module_setup(dcc_app_t *app, void *user_data) {
    dcc_package_consumer_app_state_t *state = (dcc_package_consumer_app_state_t *)user_data;
    state->setup_count++;
    return dcc_app_state(app) == state ? DCC_OK : DCC_ERR_STATE;
}

static void dcc_package_consumer_app_module_cleanup(void *user_data) {
    dcc_package_consumer_app_state_t *state = (dcc_package_consumer_app_state_t *)user_data;
    state->cleanup_count++;
}

DCC_DEFINE_COG(
    package_macro,
    "package.macro",
    DCC_COG_SLASHES(
        DCC_ON_SLASH_DATA(
            "package_macro_cog",
            "Package macro cog command",
            dcc_package_consumer_app_handler,
            user_data
        )
    ),
    DCC_COG_COMPONENTS(
        DCC_ON_BUTTON_DATA(
            "package.macro.button",
            dcc_package_consumer_app_handler,
            user_data
        )
    ),
    DCC_COG_CONTEXT_MENUS(
        DCC_ON_USER_CONTEXT_MENU_DATA(
            "Package macro user",
            dcc_package_consumer_app_handler,
            user_data
        ),
        DCC_ON_MESSAGE_CONTEXT_MENU_DATA(
            "Package macro message",
            dcc_package_consumer_app_handler,
            user_data
        )
    ),
    DCC_COG_EVENTS(DCC_ON_READY_DATA(dcc_package_consumer_app_ready_handler, user_data))
)

DCC_DEFINE_PUBLIC_COG(
    package_public_macro,
    "package.public_macro",
    DCC_COG_SLASHES(
        DCC_ON_SLASH_DATA(
            "package_public_macro_cog",
            "Package public macro cog command",
            dcc_package_consumer_app_handler,
            user_data
        )
    ),
    DCC_COG_COMPONENTS(
        DCC_ON_BUTTON_DATA(
            "package.public_macro.button",
            dcc_package_consumer_app_handler,
            user_data
        )
    )
)

int dcc_package_consumer_check_app_api(void) {
    dcc_app_options_t options;
    dcc_app_options_init(&options);
    options.client.token = "";
    options.client.intents = DCC_INTENT_GUILDS;
    dcc_app_options_t store_options =
        DCC_APP_OPTIONS_WITH_STORE("", DCC_INTENT_GUILDS, "package-state.kv");
    dcc_app_options_t preset_dev_options =
        DCC_APP_OPTIONS_DEV_GUILD("", 42ULL);
    dcc_app_options_t default_auto_defer_options =
        DCC_APP_OPTIONS_AUTO_DEFER_DEFAULT("", DCC_INTENT_GUILDS);
    dcc_app_options_t private_auto_defer_options =
        DCC_APP_OPTIONS_AUTO_DEFER_PRIVATE("", DCC_INTENT_GUILDS, 1750U);
    dcc_application_command_builder_t command_group_alias =
        DCC_SLASH_GROUP(
            "package",
            "Package command",
            DCC_CMD_GROUP(
                "tools",
                "Tools",
                DCC_CMD_SUB("ping", "Ping", DCC_OPTIONAL_STRING("note", "Note"))
            )
        );
    dcc_application_command_builder_t command_tree_alias =
        DCC_CMD_TREE(
            "package_tree",
            "Package command tree",
            DCC_CMD_SUBGROUP(
                "tools",
                "Tools",
                DCC_SUBCOMMAND_SCHEMA("sync", "Sync", DCC_OPTIONAL_BOOL("dry-run", "Dry run"))
            )
        );
    dcc_app_env_requirement_t package_env =
        DCC_ENV_DESCRIBED("DCC_PACKAGE_CONSUMER_ENV", "Package consumer env");
    dcc_app_definition_t env_definition =
        DCC_APP("package.env", DCC_REQUIRE_ENV(package_env));
    const char *env_string_out = NULL;
    uint8_t env_bool_out = 0U;
    uint64_t env_u64_out = 0U;
    int64_t env_i64_out = 0;
    dcc_snowflake_t env_channel_out = 0U;
    dcc_intents_t env_intents_out = 0U;
    dcc_app_env_binding_t env_bindings[] = {
        DCC_CONFIG_STRING("DCC_PACKAGE_STRING", &env_string_out),
        DCC_CONFIG_BOOL_DEFAULT("DCC_PACKAGE_BOOL", 1U, &env_bool_out),
        DCC_CONFIG_U64_DEFAULT("DCC_PACKAGE_U64", 64U, &env_u64_out),
        DCC_CONFIG_I64_DEFAULT("DCC_PACKAGE_I64", -64, &env_i64_out),
        DCC_CONFIG_CHANNEL("DCC_PACKAGE_CHANNEL", &env_channel_out),
        DCC_CONFIG_INTENTS_DEFAULT("DCC_PACKAGE_INTENTS", DCC_INTENTS_DEFAULT, &env_intents_out)
    };
    dcc_status_t (*env_get_token_fn)(const char *, const char **) =
        dcc_app_env_get_token;
    dcc_status_t (*env_get_string_fn)(const char *, const char **) =
        dcc_app_env_get_string;
    dcc_status_t (*env_get_bool_fn)(const char *, uint8_t *) =
        dcc_app_env_get_bool;
    dcc_status_t (*env_get_u64_fn)(const char *, uint64_t *) =
        dcc_app_env_get_u64;
    dcc_status_t (*env_get_u32_fn)(const char *, uint32_t *) =
        dcc_app_env_get_u32;
    dcc_status_t (*env_get_u32_range_or_fn)(const char *, uint32_t, uint32_t, uint32_t, uint32_t *) =
        dcc_app_env_get_u32_range_or;
    dcc_status_t (*env_get_i64_fn)(const char *, int64_t *) =
        dcc_app_env_get_i64;
    dcc_status_t (*env_get_snowflake_fn)(const char *, dcc_snowflake_t *) =
        dcc_app_env_get_snowflake;
    dcc_status_t (*env_get_intents_fn)(const char *, dcc_intents_t *) =
        dcc_app_env_get_intents;
    dcc_status_t (*env_bind_fn)(const dcc_app_env_binding_t *, size_t) =
        dcc_app_env_bind;

    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK || app == NULL) {
        return 0;
    }
    dcc_app_t *created_defined_app = NULL;

    const char secret[] = "package-session-secret";
    dcc_component_session_options_t session_options;
    dcc_component_session_options_init(&session_options);
    dcc_component_session_t session;
    memset(&session, 0, sizeof(session));
    dcc_component_session_store_t session_store;
    dcc_component_session_store_init(&session_store);
    dcc_component_session_store_t restored_session_store;
    dcc_component_session_store_init(&restored_session_store);
    char *session_store_json = NULL;
    size_t session_store_json_len = 0U;
    dcc_component_session_t definition_session;
    memset(&definition_session, 0, sizeof(definition_session));
    dcc_component_session_store_t definition_session_store;
    dcc_component_session_store_init(&definition_session_store);
    dcc_component_session_listener_options_t definition_session_listener =
        DCC_COMPONENT_SESSION_LISTENER_OPTIONS();
    dcc_component_session_options_t definition_session_options =
        DCC_COMPONENT_SESSION_OPTIONS_TTL(secret, sizeof(secret) - 1U, 1000U, 10000U);
    if (dcc_component_session_options_set_ids(&definition_session_options, "pkgdef", "nonce") != DCC_OK ||
        dcc_component_session_create(&definition_session_options, &definition_session) != DCC_OK ||
        dcc_component_session_store_add(&definition_session_store, &definition_session) != DCC_OK) {
        dcc_component_session_store_deinit(&definition_session_store);
        dcc_component_session_store_deinit(&restored_session_store);
        dcc_component_session_store_deinit(&session_store);
        dcc_app_destroy(app);
        return 0;
    }
    dcc_app_route_id_t route_id = DCC_APP_ROUTE_INVALID;
    dcc_app_route_id_t subcommand_route_id = DCC_APP_ROUTE_INVALID;
    dcc_app_route_id_t autocomplete_route_id = DCC_APP_ROUTE_INVALID;
    dcc_app_route_id_t group_route_id = DCC_APP_ROUTE_INVALID;
    dcc_package_consumer_app_state_t app_state = {0};
    uint8_t try_after = 0U;
    dcc_app_view_t view =
        DCC_APP_VIEW(DCC_VIEW_BUTTON("package.view", dcc_package_consumer_app_handler, NULL));
    dcc_app_view_t group_view =
        DCC_APP_VIEW(DCC_VIEW_BUTTON("view", dcc_package_consumer_app_handler, NULL));
    dcc_app_view_t definition_view =
        DCC_PERSISTENT_VIEW(
            DCC_VIEW_BUTTON("package.definition.view", dcc_package_consumer_app_handler, NULL)
        );
    dcc_app_view_t namespace_view =
        DCC_PERSISTENT_VIEW_DATA(
            &app_state,
            DCC_VIEW_BUTTON_NS("package.view.ns", "open", dcc_package_consumer_app_handler),
            DCC_VIEW_BUTTON_NS_DATA("package.view.ns", "edit", dcc_package_consumer_app_handler, &app_state),
            DCC_VIEW_SELECT_NS_PREFIX("package.view.ns", "select:", dcc_package_consumer_app_handler),
            DCC_VIEW_MODAL_NS("package.view.ns", "modal", dcc_package_consumer_app_handler)
        );
    dcc_app_view_t namespace_action_view =
        DCC_VIEW_ACTION_ROUTES_DATA(
            &app_state,
            DCC_VIEW_ACTION_PRIMARY_NS("Open", "package.view.action", "open", dcc_package_consumer_app_handler),
            DCC_VIEW_ACTION_SECONDARY_NS_DATA(
                "Edit",
                "package.view.action",
                "edit",
                dcc_package_consumer_app_handler,
                &app_state
            )
        );
    dcc_app_route_group_t group = DCC_GROUP_DATA(app, "package:", NULL);
    dcc_app_route_group_t policy_group = DCC_GROUP_DATA(app, "package-policy:", &app_state);
    dcc_app_wait_result_t wait_result = { .size = sizeof(wait_result) };
    dcc_app_wait_result_t zero_wait_result = {0};
    dcc_app_wait_filter_t wait_filter = DCC_WAIT_FILTER_COMPONENT("package.button");
    dcc_app_wait_filter_t initialized_wait_filter;
    dcc_app_wait_filter_init(&initialized_wait_filter);
    const char *package_wait_values[] = { "a", "b" };
    const char *package_wait_field_values[] = { "x", "y" };
    dcc_interaction_form_field_t package_wait_fields[] = {
        {
            .custom_id = "name",
            .value_type = DCC_INTERACTION_FORM_VALUE_STRING,
            .value = "package"
        },
        {
            .custom_id = "enabled",
            .value_type = DCC_INTERACTION_FORM_VALUE_BOOLEAN,
            .boolean_value = 1U
        },
        {
            .custom_id = "choices",
            .value_type = DCC_INTERACTION_FORM_VALUE_VALUES,
            .values = package_wait_field_values,
            .values_count = 2U
        }
    };
    dcc_interaction_t package_wait_interaction = {
        .id = 111ULL,
        .guild_id = 222ULL,
        .channel_id = 333ULL,
        .user_id = 444ULL,
        .custom_id = "package.button",
        .values = package_wait_values,
        .values_count = 2U,
        .form_fields = package_wait_fields,
        .form_fields_count = 3U
    };
    dcc_message_t package_wait_message = {
        .id = 555ULL,
        .guild_id = 222ULL,
        .channel_id = 333ULL,
        .author = { .id = 444ULL },
        .content = "package message"
    };
    dcc_app_wait_result_t package_wait_interaction_result = {
        .size = sizeof(package_wait_interaction_result),
        .snapshot = { .type = DCC_EVENT_BUTTON_CLICK },
        .interaction = &package_wait_interaction
    };
    dcc_app_wait_result_t package_wait_message_result = {
        .size = sizeof(package_wait_message_result),
        .snapshot = { .type = DCC_EVENT_MESSAGE_CREATE },
        .message = &package_wait_message
    };
    dcc_autocomplete_choice_t package_autocomplete_choices[] = {
        DCC_AUTOCOMPLETE_STRING_CHOICE("Package", "package"),
    };
    dcc_autocomplete_choice_t package_autocomplete_filtered[DCC_AUTOCOMPLETE_MAX_CHOICES];
    size_t package_autocomplete_filtered_count = 99U;
    dcc_status_t package_autocomplete_filter_status =
        dcc_ctx_autocomplete_filter_choices(
            NULL,
            package_autocomplete_choices,
            sizeof(package_autocomplete_choices) / sizeof(package_autocomplete_choices[0]),
            package_autocomplete_filtered,
            DCC_AUTOCOMPLETE_MAX_CHOICES,
            &package_autocomplete_filtered_count
        );
    dcc_app_command_sync_options_t sync_options =
        DCC_APP_COMMAND_SYNC_PLAN(123456789ULL);
    dcc_app_command_sync_options_t auto_sync_options =
        DCC_APP_COMMAND_SYNC_AUTO();
    dcc_package_consumer_app_args_t args = {0};
    dcc_ctx_option_field_binding_t option_fields[] = {
        DCC_ARG_REQUIRED_STRING(dcc_package_consumer_app_args_t, name, "name"),
        DCC_ARG_INT(dcc_package_consumer_app_args_t, limit, "limit", 25),
        DCC_ARG_USER(dcc_package_consumer_app_args_t, user, "user"),
        DCC_ARG_REQUIRED_ROLE(dcc_package_consumer_app_args_t, role, "role"),
    };
    dcc_ctx_form_field_binding_t form_fields[] = {
        DCC_CTX_FORM_FIELD_BOOL(dcc_package_consumer_app_args_t, visible, "visible", 1U),
        DCC_CTX_FORM_FIELD_REQUIRED_VALUES(dcc_package_consumer_app_args_t, tags, tag_count, "tags"),
    };
    dcc_ctx_component_field_binding_t component_fields[] = {
        DCC_CTX_COMPONENT_FIELD_REQUIRED_CUSTOM_ID_SUFFIX(
            dcc_package_consumer_app_args_t,
            name,
            "package.button:"
        ),
        DCC_CTX_COMPONENT_FIELD_REQUIRED_VALUES(
            dcc_package_consumer_app_args_t,
            tags,
            tag_count
        ),
    };
    args.name = "package";
    args.limit = 25;
    dcc_ctx_field_validator_t validators[] = {
        DCC_VALIDATE_STRING_REQUIRED(dcc_package_consumer_app_args_t, name, "name"),
        DCC_VALIDATE_INT_RANGE(dcc_package_consumer_app_args_t, limit, "limit", 1, 100),
    };
    dcc_ctx_validation_error_t validation_error;
    dcc_package_consumer_app_args_t invalid_args = args;
    invalid_args.name = "";
    dcc_ctx_validation_error_t invalid_error;
    dcc_ctx_parsed_date_t parsed_date;
    dcc_application_command_builder_t typed_command =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "package_typed",
            "Package typed command",
            DCC_OPT_STRING("name", "Name", 1U),
            DCC_OPT_INT_RANGE("limit", "Limit", 0U, 1, 100)
        );
    dcc_application_command_builder_t policy_typed_command =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "package_typed_policy",
            "Package typed policy command",
            DCC_OPT_STRING("name", "Name", 1U)
        );
    dcc_app_route_policy_t route_policy =
        DCC_GUARDS(
            DCC_GUARD_MIDDLEWARE(DCC_MIDDLEWARE(dcc_package_consumer_app_middleware, NULL)),
            DCC_GUARD_GUILD_ONLY(),
            DCC_GUARD_CHANNEL_TYPES(DCC_CHANNEL_TEXT),
            DCC_GUARD_OWNER(123ULL),
            DCC_GUARD_HAS_PERMISSIONS(DCC_PERMISSION_MANAGE_MESSAGES),
            DCC_GUARD_HAS_ROLE(789ULL),
            DCC_GUARD_HAS_ANY_ROLE(790ULL, 791ULL),
            DCC_GUARD_CHECK_MESSAGE(dcc_package_consumer_app_check, NULL, "Blocked", "Package check failed."),
            DCC_GUARD_COOLDOWN_USER(2U, 1000U)
        );
    dcc_app_route_policy_t dm_route_policy =
        DCC_GUARDS(DCC_GUARD_DM_ONLY());
    dcc_app_route_policy_t nsfw_route_policy =
        DCC_GUARDS(DCC_GUARD_NSFW_ONLY());
    dcc_app_check_t route_check =
        DCC_CHECK_MESSAGE(dcc_package_consumer_app_check, NULL, "Blocked", "Package check failed.");
    dcc_app_cooldown_options_t cooldown = DCC_COOLDOWN_USER(1U, 60000U);
    dcc_command_route_t typed_slash =
        DCC_TYPED_SLASH(
            &typed_command,
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(dcc_package_consumer_app_args_t, name, "name"),
                DCC_CTX_OPTION_FIELD_INT(dcc_package_consumer_app_args_t, limit, "limit", 25)
            ),
            DCC_CTX_FIELD_VALIDATORS(
                DCC_VALIDATE_STRING_REQUIRED(dcc_package_consumer_app_args_t, name, "name"),
                DCC_VALIDATE_INT_RANGE(dcc_package_consumer_app_args_t, limit, "limit", 1, 100)
            )
        );
    dcc_command_route_t policy_typed_slash =
        DCC_TYPED_SLASH_POLICY(
            &policy_typed_command,
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(dcc_package_consumer_app_args_t, name, "name")
            ),
            DCC_CTX_NO_FIELD_VALIDATORS(),
            route_policy
        );
    dcc_command_route_t inline_typed_slash =
        DCC_TYPED_SLASH_OPTIONS(
            "package_inline_typed",
            "Package inline typed command",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            DCC_ARGS(
                DCC_ARG_REQUIRED_STRING(dcc_package_consumer_app_args_t, name, "name"),
                DCC_ARG_INT(dcc_package_consumer_app_args_t, limit, "limit", 25)
            ),
            DCC_VALIDATORS(
                DCC_VALIDATE_STRING_REQUIRED(dcc_package_consumer_app_args_t, name, "name"),
                DCC_VALIDATE_INT_RANGE(dcc_package_consumer_app_args_t, limit, "limit", 1, 100)
            ),
            DCC_OPT_STRING("name", "Name", 1U),
            DCC_OPT_INT_RANGE("limit", "Limit", 0U, 1, 100)
        );
    dcc_command_route_t on_typed_slash_alias =
        DCC_ON_TYPED_SLASH_DATA(
            "package_on_typed",
            "Package typed route alias",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_ARGS(
                DCC_ARG_REQUIRED_STRING(dcc_package_consumer_app_args_t, name, "name")
            ),
            DCC_NO_VALIDATORS(),
            DCC_OPT_STRING("name", "Name", 1U)
        );
    dcc_command_route_t params_typed_slash =
        DCC_COMMAND_ROUTE_VALIDATED(
            "package_params",
            "Package single-source params command",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            DCC_PACKAGE_CONSUMER_APP_PARAMS,
            DCC_VALIDATORS(
                DCC_VALIDATE_STRING_REQUIRED(dcc_package_consumer_app_args_t, name, "name"),
                DCC_VALIDATE_INT_RANGE(dcc_package_consumer_app_args_t, limit, "limit", 1, 100)
            )
        );
    dcc_application_command_builder_t params_schema_command =
        DCC_COMMAND_PARAMS(
            "package_params_schema",
            "Package params schema command",
            dcc_package_consumer_app_args_t,
            DCC_PACKAGE_CONSUMER_APP_PARAMS
        );
    dcc_application_command_builder_t typed_subcommand_command =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "package_admin",
            "Package admin command",
            DCC_COMMAND_SUBCOMMAND(
                "reload",
                "Reload",
                DCC_COMMAND_STRING_OPTION_AUTOCOMPLETE("name", "Name", 1U)
            )
        );
    dcc_application_command_builder_t params_subcommand_command =
        DCC_SLASH_COMMAND_WITH_OPTIONS(
            "package_admin_params",
            "Package params admin command",
            DCC_CMD_SUB_PARAMS(
                "inspect",
                "Inspect",
                dcc_package_consumer_app_args_t,
                DCC_PACKAGE_CONSUMER_APP_PARAMS
            )
        );
    dcc_subcommand_route_t typed_subcommand =
        DCC_TYPED_SUBCOMMAND_BUILDER(
            &typed_subcommand_command,
            "reload",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(dcc_package_consumer_app_args_t, name, "name")
            ),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_subcommand_route_t params_typed_subcommand =
        DCC_ON_SUBCOMMAND_BUILDER_PARAMS_DATA(
            &params_subcommand_command,
            "inspect",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_APP_PARAMS
        );
    dcc_subcommand_route_t named_params_typed_subcommand =
        DCC_ON_SUBCOMMAND_PARAMS_VALIDATED(
            "package_admin",
            "reload/named",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            DCC_PACKAGE_CONSUMER_APP_PARAMS,
            DCC_VALIDATORS(
                DCC_VALIDATE_STRING_REQUIRED(dcc_package_consumer_app_args_t, name, "name"),
                DCC_VALIDATE_INT_RANGE(dcc_package_consumer_app_args_t, limit, "limit", 1, 100)
            )
        );
    dcc_subcommand_route_t in_params_typed_subcommand =
        DCC_ON_SUBCOMMAND_BUILDER_PARAMS_IN_DATA(
            &params_subcommand_command,
            "tools",
            "inspect",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_APP_PARAMS
        );
    dcc_subcommand_route_t in_named_params_typed_subcommand =
        DCC_SUBCOMMAND_ROUTE_IN_DATA(
            "package_admin",
            "reload",
            "in_named",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_APP_PARAMS
        );
    dcc_autocomplete_route_t typed_autocomplete =
        DCC_TYPED_AUTOCOMPLETE_BUILDER(
            &typed_command,
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(dcc_package_consumer_app_args_t, name, "name")
            ),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_autocomplete_route_t params_typed_autocomplete =
        DCC_AUTOCOMPLETE_BUILDER_PARAMS_DATA(
            &typed_command,
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_APP_PARAMS
        );
    dcc_autocomplete_route_t policy_params_typed_autocomplete =
        DCC_AUTOCOMPLETE_PARAMS_DATA_POLICY(
            "package_typed_policy",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_APP_PARAMS,
            route_policy
        );
    dcc_autocomplete_route_t typed_subcommand_autocomplete =
        DCC_TYPED_SUBCOMMAND_AUTOCOMPLETE_BUILDER(
            &typed_subcommand_command,
            "reload",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_CTX_OPTION_FIELD_BINDINGS(
                DCC_CTX_OPTION_FIELD_REQUIRED_STRING(dcc_package_consumer_app_args_t, name, "name")
            ),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_autocomplete_route_t params_typed_subcommand_autocomplete =
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_DATA(
            "package_admin",
            "reload",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_APP_PARAMS
        );
    dcc_autocomplete_route_t in_params_typed_subcommand_autocomplete =
        DCC_ON_SUBCOMMAND_AUTOCOMPLETE_PARAMS_IN_DATA(
            "package_admin",
            "reload",
            "complete",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_APP_PARAMS
        );
    dcc_modal_route_t typed_modal =
        DCC_TYPED_MODAL(
            "package.modal",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_CTX_FORM_FIELD_BINDINGS(
                DCC_CTX_FORM_FIELD_BOOL(dcc_package_consumer_app_args_t, visible, "visible", 1U)
            ),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_modal_flow_t package_modal_flow =
        DCC_MODAL_FLOW(
            "package.flow",
            "Package flow",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_CTX_FORM_FIELD_BINDINGS(
                DCC_CTX_FORM_FIELD_BOOL(dcc_package_consumer_app_args_t, visible, "visible", 1U)
            ),
            DCC_CTX_NO_FIELD_VALIDATORS(),
            DCC_ACTION_ROW(DCC_MODAL_TEXT_INPUT("visible", "Visible", 0U))
        );
    dcc_modal_route_t params_typed_modal =
        DCC_MODAL_PARAMS_DATA(
            "package.modal.params",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_MODAL_PARAMS
        );
    dcc_modal_flow_t params_modal_flow =
        DCC_MODAL_V2_FLOW_PARAMS_DATA(
            "package.flow.params",
            "Package params flow",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_MODAL_PARAMS,
            DCC_V2_LABEL("Visible", DCC_MODAL_V2_CHECKBOX("visible", "Visible", 1U))
        );
    dcc_modal_route_t ns_params_typed_modal =
        DCC_MODAL_NS_PARAMS_DATA(
            "package.ns",
            "modal",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_MODAL_PARAMS
        );
    dcc_modal_flow_t ns_params_modal_flow =
        DCC_MODAL_V2_FLOW_NS_PARAMS_DATA(
            "package.ns",
            "flow",
            "Package namespace flow",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_MODAL_PARAMS,
            DCC_V2_LABEL("Visible", DCC_UI_CHECKBOX_NS("package.ns", "visible", "Visible", 1U))
        );
    dcc_modal_flow_t invalid_package_modal_flow = package_modal_flow;
    invalid_package_modal_flow.route.custom_id = "package.flow.other";
    dcc_button_route_t typed_button =
        DCC_TYPED_BUTTON_PREFIX(
            "package.button:",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_CTX_COMPONENT_FIELD_BINDINGS(
                DCC_CTX_COMPONENT_FIELD_REQUIRED_CUSTOM_ID_SUFFIX(
                    dcc_package_consumer_app_args_t,
                    name,
                    "package.button:"
                )
            ),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_button_route_t params_typed_button =
        DCC_BUTTON_PREFIX_PARAMS_DATA(
            "package.params.button:",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_BUTTON_PARAMS
        );
    dcc_button_route_t ns_typed_button =
        DCC_TYPED_BUTTON_NS(
            "package.ns",
            "button",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_NO_COMPONENT_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_button_route_t ns_params_typed_button =
        DCC_BUTTON_NS_PREFIX_PARAMS_DATA(
            "package.ns",
            "button:",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_NS_BUTTON_PARAMS
        );
    dcc_select_route_t typed_select =
        DCC_TYPED_SELECT(
            "package.select",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_CTX_COMPONENT_FIELD_BINDINGS(
                DCC_CTX_COMPONENT_FIELD_REQUIRED_FIRST_VALUE(dcc_package_consumer_app_args_t, name),
                DCC_CTX_COMPONENT_FIELD_REQUIRED_VALUES(
                    dcc_package_consumer_app_args_t,
                    tags,
                    tag_count
                )
            ),
            DCC_CTX_NO_FIELD_VALIDATORS()
        );
    dcc_select_route_t params_typed_select =
        DCC_SELECT_PARAMS_DATA(
            "package.params.select",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_SELECT_PARAMS
        );
    dcc_select_route_t ns_typed_select =
        DCC_TYPED_SELECT_NS(
            "package.ns",
            "select",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_NO_COMPONENT_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_select_route_t ns_params_typed_select =
        DCC_SELECT_NS_PARAMS_DATA(
            "package.ns",
            "select.params",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            &app_state,
            DCC_PACKAGE_CONSUMER_SELECT_PARAMS
        );
    dcc_command_route_t definition_slash =
        DCC_TYPED_SLASH_NO_OPTIONS(
            "package_defined",
            "Package definition command",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            DCC_NO_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_app_definition_t app_definition =
            DCC_APP(
                "package.app",
                DCC_APP_TYPED_SLASHES(definition_slash),
                DCC_APP_SUBCOMMAND_ROUTES(
                    DCC_TYPED_SUBCOMMAND(
                        "package_defined",
                        "reload",
                        dcc_package_consumer_app_args_t,
                        dcc_package_consumer_app_typed_handler,
                        NULL,
                        DCC_NO_ARGS(),
                        DCC_NO_VALIDATORS()
                    )
                ),
                DCC_APP_AUTOCOMPLETE_ROUTES(
                    DCC_TYPED_AUTOCOMPLETE(
                        "package_defined",
                        dcc_package_consumer_app_args_t,
                        dcc_package_consumer_app_typed_handler,
                        NULL,
                        DCC_NO_ARGS(),
                        DCC_NO_VALIDATORS()
                    )
                ),
                DCC_APP_MESSAGE_COMMANDS(
                    DCC_MESSAGE_COMMAND("!", "package_top", dcc_package_consumer_app_message_command_handler),
                    DCC_PREFIX_COMMAND("?", "package_alias", dcc_package_consumer_app_message_command_handler)
                ),
                DCC_APP_EVENTS(
                    DCC_ON_READY(dcc_package_consumer_app_ready_handler),
                    DCC_ON_MESSAGE_UPDATE_DATA(dcc_package_consumer_app_message_handler, &app_state)
                ),
            DCC_APP_TASKS(
                DCC_TASK_LOOP_MINUTES_DATA(10U, dcc_package_consumer_app_task_handler, &app_state)
            ),
            DCC_APP_VIEWS(definition_view),
            DCC_APP_COMPONENT_SESSIONS(
                DCC_COMPONENT_SESSION_ROUTE_OPTIONS(
                    &definition_session,
                    &definition_session_listener,
                    dcc_package_consumer_app_handler,
                    &app_state
                ),
                DCC_COMPONENT_SESSION_STORE_ROUTE(
                    &definition_session_store,
                    dcc_package_consumer_app_handler,
                    &app_state
                )
            ),
            DCC_APP_MIDDLEWARE(DCC_EXTENSION_MIDDLEWARE(dcc_package_consumer_app_middleware, NULL)),
            DCC_APP_GUILD_ONLY(),
            DCC_APP_CHANNEL_TYPES(DCC_CHANNEL_TEXT),
            DCC_APP_OWNER(123ULL),
            DCC_APP_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_MESSAGES),
            DCC_APP_REQUIRE_ROLE(789ULL),
            DCC_APP_REQUIRE_ANY_ROLE(790ULL, 791ULL),
            DCC_APP_CHECK_MESSAGE(dcc_package_consumer_app_check, NULL, "Blocked", "Package app check failed."),
            DCC_APP_COOLDOWN_USER(2U, 1000U),
            DCC_APP_AUTO_SYNC(),
            DCC_APP_AUTO_DEFER(1000U),
            DCC_APP_DEFAULT_ERRORS(),
            DCC_APP_ERROR_HANDLER(dcc_package_consumer_app_error_handler, &app_state)
        );
    dcc_app_definition_t dm_app_definition =
        DCC_APP("package.dm_app", DCC_APP_DM_ONLY());
    dcc_app_definition_t nsfw_app_definition =
        DCC_APP("package.nsfw_app", DCC_APP_NSFW_ONLY());
    dcc_status_t (*run_env_defined_fn)(const char *, const dcc_app_definition_t *) =
        dcc_app_run_from_env_defined;
    dcc_status_t (*run_dotenv_defined_fn)(const char *, const dcc_app_definition_t *) =
        dcc_app_run_dotenv_defined;
    dcc_status_t invalid_run_defined =
        dcc_app_run_defined(&options, &((dcc_app_definition_t){0}));
    dcc_status_t invalid_run_macro =
        DCC_RUN_APP_OPTIONS(options, ((dcc_app_definition_t){0}));
    dcc_command_route_t extension_typed_slash =
        DCC_TYPED_SLASH_NO_OPTIONS(
            "package_ext_typed",
            "Package extension typed command",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            DCC_NO_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_subcommand_route_t extension_typed_subcommand =
        DCC_TYPED_SUBCOMMAND(
            "package_ext_typed",
            "reload",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_NO_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_autocomplete_route_t extension_typed_autocomplete =
        DCC_TYPED_AUTOCOMPLETE(
            "package_ext_typed",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_NO_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_modal_route_t extension_typed_modal =
        DCC_TYPED_MODAL(
            "package.extension.modal.typed",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_NO_FORM_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_button_route_t extension_typed_button =
        DCC_TYPED_BUTTON(
            "package.extension.button.typed",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_NO_COMPONENT_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_select_route_t extension_typed_select =
        DCC_TYPED_SELECT(
            "package.extension.select.typed",
            dcc_package_consumer_app_args_t,
            dcc_package_consumer_app_typed_handler,
            NULL,
            DCC_NO_COMPONENT_ARGS(),
            DCC_NO_VALIDATORS()
        );
    dcc_app_extension_t extension =
        DCC_COG(
            "package.extension",
            DCC_COG_SLASHES(
                DCC_ON_COMMAND("package_ext", "Package extension command", dcc_package_consumer_app_handler),
                DCC_ON_COMMAND_POLICY(
                    "package_ext_policy",
                    "Package extension policy command",
                    dcc_package_consumer_app_handler,
                    route_policy
                )
            ),
            DCC_COG_COMPONENTS(
                DCC_ON_BUTTON("package.extension.button", dcc_package_consumer_app_handler),
                DCC_ON_BUTTON_POLICY(
                    "package.extension.button.policy",
                    dcc_package_consumer_app_handler,
                    route_policy
                ),
                DCC_ON_SELECT("package.extension.select", dcc_package_consumer_app_handler),
                DCC_ON_MODAL("package.extension.modal", dcc_package_consumer_app_handler)
            ),
            DCC_COG_CONTEXT_MENUS(
                DCC_ON_USER_CONTEXT_MENU("Package extension user", dcc_package_consumer_app_handler),
                DCC_ON_MESSAGE_CONTEXT_MENU_POLICY(
                    "Package extension message",
                    dcc_package_consumer_app_handler,
                    route_policy
                )
            ),
            DCC_COG_TYPED_SLASHES(extension_typed_slash),
            DCC_COG_SUBCOMMAND_ROUTES(extension_typed_subcommand),
            DCC_COG_AUTOCOMPLETE_ROUTES(extension_typed_autocomplete),
            DCC_COG_MODAL_ROUTES(extension_typed_modal),
            DCC_COG_BUTTON_ROUTES(extension_typed_button),
            DCC_COG_SELECT_ROUTES(extension_typed_select),
            DCC_COG_EVENTS(
                DCC_ON_READY(dcc_package_consumer_app_ready_handler),
                DCC_ON_MESSAGE_UPDATE(dcc_package_consumer_app_message_handler)
            ),
            DCC_COG_MESSAGE_COMMANDS(
                DCC_MESSAGE_COMMAND("!", "package", dcc_package_consumer_app_message_command_handler)
            ),
            DCC_COG_TASKS(DCC_TASK_LOOP_SECONDS(60U, dcc_package_consumer_app_task_handler))
        );
    dcc_app_module_t module =
        DCC_APP_MODULE_WITH_CLEANUP(
            "package.module",
            dcc_package_consumer_app_module_setup,
            &app_state,
            dcc_package_consumer_app_module_cleanup
        );
    dcc_app_module_t macro_cog_module = DCC_COG_MODULE(package_macro, &app_state);
    dcc_app_module_t public_macro_cog_module =
        DCC_COG_MODULE_NAMED("package.public_macro", package_public_macro, &app_state);
    dcc_message_builder_t managed_message = DCC_MESSAGE_TEXT("managed");
    dcc_component_v2_builder_t ui_card =
        DCC_UI_CARD_ACCENT(
            0x5865F2U,
            DCC_UI_TEXT("## Package"),
            DCC_UI_ROW(DCC_UI_PRIMARY("Open", "package.ui.open"))
        );
    dcc_component_builder_t legacy_button_ns =
        DCC_BUTTON_PRIMARY_BUILDER_NS("Open", "package", "legacy.open");
    dcc_component_v2_builder_t ui_button_ns =
        DCC_UI_PRIMARY_NS("Open", "package", "ui.open");
    dcc_component_v2_builder_t ui_select_ns =
        DCC_UI_STRING_SELECT_NS("package", "ui.select", DCC_SELECT_OPTION("One", "one"));
    dcc_component_v2_builder_t ui_input_ns =
        DCC_UI_INPUT_PLACEHOLDER_NS("package", "ui.input", "Input", "value", 1U);
    dcc_message_builder_t ui_message = DCC_UI_MESSAGE(ui_card);
    dcc_message_builder_t ui_ephemeral = DCC_UI_EPHEMERAL(DCC_UI_TEXT("Private"));
    dcc_message_builder_t confirm_message =
        DCC_CONFIRM_MESSAGE("Confirm package?", "package.confirm", "package.cancel");
    dcc_message_builder_t paginator_message =
        DCC_PAGINATOR_V2("Package page", "package.prev", "package.next", 1U, 0U);
    dcc_message_builder_t settings_panel =
        DCC_SETTINGS_PANEL_MESSAGE("Package settings", "package.edit", "package.enable", "package.disable", "package.reset", 1U);
    dcc_message_builder_t settings_panel_v2 =
        DCC_SETTINGS_PANEL_V2("## Package settings", "package.edit", "package.enable", "package.disable", "package.reset", 0U);
    dcc_interaction_flow_t sugar_flow =
        DCC_FLOW_AUTO_DEFER_STARTED(NULL, NULL, 1000U, 1500U);
    dcc_managed_message_options_t managed =
        DCC_MANAGED_MESSAGE_OPTIONS(123U, &managed_message, NULL, NULL, NULL);
    dcc_store_t package_store = {0};
    dcc_store_managed_message_binding_t store_binding =
        DCC_STORE_MANAGED_MESSAGE("package.latest", &package_store);
    dcc_store_managed_message_binding_t app_store_binding =
        DCC_APP_STORE_MANAGED_MESSAGE(app, "package.app.latest");
    dcc_managed_message_options_t store_managed =
        DCC_MANAGED_MESSAGE_STORE_OPTIONS(123U, &managed_message, &store_binding);
    dcc_thread_params_t thread_params = {
        .size = sizeof(thread_params),
        .name = "package-thread",
    };
    dcc_channel_params_t channel_params = {
        .size = sizeof(channel_params),
        .guild_id = 1U,
        .channel_id = 2U,
        .type = DCC_CHANNEL_TEXT,
        .name = "package-channel",
    };
    dcc_channel_position_t channel_positions[] = {
        { .channel_id = 2U, .position = 1U },
    };
    dcc_channel_positions_params_t channel_positions_params = {
        .size = sizeof(channel_positions_params),
        .guild_id = 1U,
        .positions = channel_positions,
        .position_count = DCC_ARRAY_LEN(channel_positions),
    };
    dcc_invite_params_t invite_params = {
        .size = sizeof(invite_params),
        .channel_id = 2U,
        .max_age = 3600U,
        .max_uses = 5U,
        .unique = 1U,
    };
    dcc_channel_permission_overwrite_t permission_overwrite = {
        .id = 3U,
        .allow = DCC_PERMISSION_VIEW_CHANNEL,
        .deny = 0U,
        .type = DCC_CHANNEL_OVERWRITE_ROLE,
    };
    dcc_role_params_t role_params = {
        .size = sizeof(role_params),
        .guild_id = 1U,
        .role_id = 2U,
        .name = "package-role",
    };
    dcc_role_position_t role_positions[] = {
        { .role_id = 2U, .position = 1U },
    };
    dcc_snowflake_t prune_roles[] = { 2U };
    dcc_guild_ban_params_t ban_params = {
        .size = sizeof(ban_params),
        .guild_id = 1U,
        .user_id = 2U,
        .delete_message_seconds = 60U,
    };
    dcc_guild_prune_params_t prune_params = {
        .size = sizeof(prune_params),
        .guild_id = 1U,
        .days = 7U,
        .include_roles = prune_roles,
        .include_role_count = DCC_ARRAY_LEN(prune_roles),
        .compute_prune_count = 1U,
    };
    dcc_current_guild_member_params_t current_member_params = {
        .size = sizeof(current_member_params),
        .guild_id = 1U,
        .nick = "package-nick",
    };
    dcc_guild_member_nickname_params_t nickname_params = {
        .size = sizeof(nickname_params),
        .guild_id = 1U,
        .nick = "package-nick",
    };
    dcc_scheduled_event_params_t scheduled_event_params = {
        .size = sizeof(scheduled_event_params),
        .guild_id = 1U,
        .event_id = 2U,
        .channel_id = 3U,
        .name = "package-event",
        .scheduled_start_time = "2026-06-23T12:00:00Z",
        .privacy_level = DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY,
        .entity_type = DCC_SCHEDULED_EVENT_ENTITY_VOICE,
    };
    dcc_webhook_params_t webhook_params = {
        .size = sizeof(webhook_params),
        .webhook_id = 4U,
        .channel_id = 1U,
        .name = "package-webhook",
    };
    dcc_webhook_params_t webhook_token_params = {
        .size = sizeof(webhook_token_params),
        .webhook_id = 4U,
        .channel_id = 1U,
        .name = "package-webhook",
        .token = "token",
    };
    dcc_current_user_params_t user_params = {
        .size = sizeof(user_params),
        .nickname = "package-user",
    };
    dcc_dm_channel_params_t dm_params = {
        .size = sizeof(dm_params),
        .user_id = 2U,
    };
    dcc_stage_instance_params_t stage_params = {
        .size = sizeof(stage_params),
        .channel_id = 3U,
        .topic = "package-stage",
        .privacy_level = DCC_STAGE_PRIVACY_GUILD_ONLY,
    };
    dcc_template_params_t template_params = {
        .size = sizeof(template_params),
        .guild_id = 1U,
        .code = "template-code",
        .name = "package-template",
    };
    dcc_guild_widget_params_t widget_params = {
        .size = sizeof(widget_params),
        .channel_id = 3U,
        .enabled = 1U,
    };
    dcc_onboarding_params_t onboarding_params = {
        .size = sizeof(onboarding_params),
        .guild_id = 1U,
        .enabled = 1U,
        .mode = DCC_ONBOARDING_MODE_DEFAULT,
    };
    dcc_welcome_screen_params_t welcome_params = {
        .size = sizeof(welcome_params),
        .description = "Welcome",
        .enabled = 1U,
    };
    dcc_auto_moderation_action_params_t auto_mod_action = {
        .size = sizeof(auto_mod_action),
        .type = DCC_AUTO_MODERATION_ACTION_BLOCK_MESSAGE,
    };
    dcc_auto_moderation_rule_params_t auto_mod_rule_params = {
        .size = sizeof(auto_mod_rule_params),
        .rule_id = 2U,
        .name = "package-auto-mod",
        .event_type = DCC_AUTO_MODERATION_EVENT_MESSAGE_SEND,
        .trigger_type = DCC_AUTO_MODERATION_TRIGGER_SPAM,
        .actions = &auto_mod_action,
        .action_count = 1U,
        .enabled = 1U,
    };
    dcc_emoji_params_t emoji_params = {
        .size = sizeof(emoji_params),
        .emoji_id = 2U,
        .name = "package_emoji",
        .image = "data:image/png;base64,AA==",
    };
    const unsigned char sticker_data[] = { 0x89U, 'P', 'N', 'G' };
    dcc_guild_sticker_params_t sticker_params = {
        .size = sizeof(sticker_params),
        .guild_id = 1U,
        .name = "package-sticker",
        .description = "Package sticker",
        .tags = "package",
        .filename = "package.png",
        .content_type = "image/png",
        .data = sticker_data,
        .data_len = sizeof(sticker_data),
    };
    dcc_message_search_params_t message_search_params = {
        .size = sizeof(message_search_params),
        .content = "package",
        .has_limit = 1U,
        .limit = 25U,
    };
    dcc_group_dm_recipient_params_t group_dm_recipient_params = {
        .size = sizeof(group_dm_recipient_params),
        .channel_id = 3U,
        .user_id = 4U,
        .access_token = "access-token",
        .nick = "package-group",
    };
    dcc_voice_state_params_t voice_state_params = {
        .size = sizeof(voice_state_params),
        .guild_id = 1U,
        .user_id = 2U,
        .channel_id = 3U,
        .suppress = 1U,
        .request_to_speak_timestamp = "2026-06-23T12:00:00Z",
    };
    dcc_test_entitlement_params_t test_entitlement_params = {
        .size = sizeof(test_entitlement_params),
        .application_id = 1U,
        .sku_id = 2U,
        .owner_id = 3U,
        .owner_type = 1U,
    };
    dcc_snowflake_t entitlement_sku_ids[] = { 2U };
    dcc_user_t display_user = {
        .id = 42U,
        .username = "package-user",
        .global_name = "Package User",
        .avatar = "package_avatar",
    };
    dcc_member_t display_member = {
        .guild_id = 24U,
        .user = display_user,
        .nick = "Package Nick",
        .avatar = "package_member_avatar",
    };
    dcc_guild_t display_guild = {
        .id = 24U,
        .name = "Package Guild",
        .icon = "package_guild_icon",
    };
    dcc_channel_t display_channel = {
        .id = 25U,
        .guild_id = 24U,
        .name = "package-channel",
    };
    dcc_role_t display_role = {
        .id = 26U,
        .guild_id = 24U,
        .name = "package-role",
    };

    dcc_package_consumer_validation_reply_helper();
    dcc_package_consumer_try_ok_helper(&try_after);

    int ok =
        dcc_app_client(app) != NULL &&
        dcc_app_command_registry(app) != NULL &&
        dcc_app_store(app) == NULL &&
        store_options.store_file_path != NULL &&
        strcmp(store_options.store_file_path, "package-state.kv") == 0 &&
        dcc_app_set_state(app, &app_state, NULL) == DCC_OK &&
        dcc_app_state(app) == &app_state &&
        DCC_LOAD_COG(NULL, package_macro) == DCC_ERR_INVALID_ARG &&
        macro_cog_module.setup == DCC_COG_SETUP(package_macro) &&
        strcmp(macro_cog_module.name, "package_macro") == 0 &&
        public_macro_cog_module.setup == DCC_COG_SETUP(package_public_macro) &&
        strcmp(public_macro_cog_module.name, "package.public_macro") == 0 &&
        dcc_app_module_register(app, &module) == DCC_OK &&
        dcc_app_module_count(app) == 1U &&
        dcc_app_module_register(app, &macro_cog_module) == DCC_OK &&
        dcc_app_module_count(app) == 2U &&
        dcc_app_module_register(app, &public_macro_cog_module) == DCC_OK &&
        dcc_app_module_count(app) == 3U &&
        app_state.setup_count == 1 &&
        wait_filter.size == sizeof(wait_filter) &&
        wait_filter.custom_id != NULL &&
        initialized_wait_filter.size == sizeof(initialized_wait_filter) &&
        DCC_AUTOCOMPLETE_MAX_CHOICES == 25U &&
        dcc_ctx_focused_option(NULL) == NULL &&
        dcc_ctx_focused_option_name(NULL) == NULL &&
        strcmp(dcc_ctx_focused_option_string(NULL, "package"), "package") == 0 &&
        dcc_ctx_response_state(NULL) == DCC_INTERACTION_FLOW_FAILED &&
        strcmp(dcc_ctx_response_state_string(NULL), "failed") == 0 &&
        dcc_ctx_response_sent(NULL) == 0U &&
        dcc_ctx_response_failed(NULL) == 1U &&
        DCC_CTX_RESPONDED(NULL) == 0U &&
        DCC_CTX_RESPONSE_FAILED(NULL) == 1U &&
        strcmp(DCC_USER_USERNAME(&display_user), "package-user") == 0 &&
        strcmp(DCC_USER_GLOBAL_NAME(&display_user), "Package User") == 0 &&
        strcmp(DCC_USER_DISPLAY_NAME(&display_user), "Package User") == 0 &&
        strcmp(DCC_MEMBER_DISPLAY_NAME(&display_member), "Package Nick") == 0 &&
        strcmp(DCC_GUILD_NAME(&display_guild), "Package Guild") == 0 &&
        strcmp(DCC_CHANNEL_NAME(&display_channel), "package-channel") == 0 &&
        strcmp(DCC_ROLE_NAME(&display_role), "package-role") == 0 &&
        strcmp(DCC_USER_AVATAR_URL_EXT(&display_user, "webp", 128U),
            DCC_CDN_BASE_URL "/avatars/42/package_avatar.webp?size=128") == 0 &&
        strcmp(DCC_MEMBER_AVATAR_URL_EXT(&display_member, &display_user, "png", 64U),
            DCC_CDN_BASE_URL
                "/guilds/24/users/42/avatars/package_member_avatar.png?size=64") == 0 &&
        strcmp(DCC_GUILD_ICON_URL_EXT(&display_guild, "png", 256U),
            DCC_CDN_BASE_URL "/icons/24/package_guild_icon.png?size=256") == 0 &&
        strcmp(DCC_CTX_GUILD_NAME_OR(NULL, "fallback"), "fallback") == 0 &&
        strcmp(DCC_CTX_CHANNEL_NAME_OR(NULL, "fallback"), "fallback") == 0 &&
        strcmp(DCC_CTX_AUTHOR_DISPLAY_NAME_OR(NULL, "fallback"), "fallback") == 0 &&
        strcmp(DCC_CTX_TARGET_DISPLAY_NAME_OR(NULL, "fallback"), "fallback") == 0 &&
        strcmp(DCC_CTX_ARG_USER_DISPLAY_NAME_OR(NULL, "user", "fallback"), "fallback") == 0 &&
        strcmp(DCC_CTX_AUTHOR_AVATAR_URL(NULL), "") == 0 &&
        package_autocomplete_filter_status == DCC_OK &&
        package_autocomplete_filtered_count == 1U &&
        strcmp(package_autocomplete_filtered[0].name, "Package") == 0 &&
        dcc_ctx_reply_autocomplete_matching(
            NULL,
            package_autocomplete_choices,
            sizeof(package_autocomplete_choices) / sizeof(package_autocomplete_choices[0]),
            DCC_AUTOCOMPLETE_MAX_CHOICES + 1U,
            NULL,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        try_after == 1U &&
        dcc_app_load_env_file(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        dcc_app_load_env_file("dcc_package_consumer_missing.env", 1U) == DCC_ERR_NOT_FOUND &&
        dcc_app_auto_defer_ephemeral(app, 1500U) == DCC_OK &&
        dcc_app_disable_auto_defer(app) == DCC_OK &&
        dcc_app_use_default_error_responses(NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_use_default_error_responses(app) == DCC_OK &&
        dcc_app_apply(NULL, &app_definition) == DCC_ERR_INVALID_ARG &&
        dcc_app_apply(app, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_apply(app, &(dcc_app_definition_t){0}) == DCC_ERR_INVALID_ARG &&
        dcc_app_apply(app, &app_definition) == DCC_OK &&
        route_policy.guild_only == 1U &&
        route_policy.channel_type_count == 1U &&
        route_policy.required_role_id_count == 1U &&
        route_policy.any_role_id_count == 2U &&
        app_definition.channel_type_count == 1U &&
        app_definition.required_role_id_count == 1U &&
        app_definition.any_role_id_count == 2U &&
        app_definition.check_count == 1U &&
        app_definition.checks != NULL &&
        app_definition.checks[0].check == dcc_package_consumer_app_check &&
        app_definition.cooldown.bucket == DCC_APP_COOLDOWN_USER &&
        app_definition.cooldown.limit == 2U &&
        app_definition.cooldown.window_ms == 1000U &&
        app_definition.task_count == 1U &&
        app_definition.tasks != NULL &&
        app_definition.tasks[0].type == DCC_APP_EXTENSION_TASK_EVERY_MINUTES &&
        app_definition.tasks[0].interval == 10U &&
        app_definition.tasks[0].user_data == &app_state &&
        app_definition.view_count == 1U &&
        app_definition.views != NULL &&
        app_definition.views[0].item_count == 1U &&
        strcmp(app_definition.views[0].items[0].custom_id, "package.definition.view") == 0 &&
        namespace_view.user_data == &app_state &&
        namespace_view.item_count == 4U &&
        strcmp(namespace_view.items[0].custom_id, "package.view.ns.open") == 0 &&
        strcmp(namespace_view.items[1].custom_id, "package.view.ns.edit") == 0 &&
        namespace_view.items[1].user_data == &app_state &&
        namespace_view.items[2].type == DCC_APP_VIEW_SELECT_PREFIX &&
        strcmp(namespace_view.items[2].custom_id, "package.view.ns.select:") == 0 &&
        namespace_view.items[3].type == DCC_APP_VIEW_MODAL &&
        strcmp(namespace_view.items[3].custom_id, "package.view.ns.modal") == 0 &&
        namespace_action_view.user_data == &app_state &&
        namespace_action_view.item_count == 2U &&
        strcmp(namespace_action_view.items[0].custom_id, "package.view.action.open") == 0 &&
        strcmp(namespace_action_view.items[1].custom_id, "package.view.action.edit") == 0 &&
        namespace_action_view.items[1].user_data == &app_state &&
        app_definition.component_session_count == 2U &&
        app_definition.component_sessions != NULL &&
        app_definition.component_sessions[0].session == &definition_session &&
        app_definition.component_sessions[0].options == &definition_session_listener &&
        app_definition.component_sessions[1].store == &definition_session_store &&
        app_definition.message_command_count == 2U &&
        app_definition.message_commands != NULL &&
        strcmp(app_definition.message_commands[0].prefix, "!") == 0 &&
        strcmp(app_definition.message_commands[0].name, "package_top") == 0 &&
        strcmp(app_definition.message_commands[1].prefix, "?") == 0 &&
        strcmp(app_definition.message_commands[1].name, "package_alias") == 0 &&
        app_definition.event_count == 2U &&
        app_definition.events != NULL &&
        app_definition.events[0].type == DCC_APP_EXTENSION_READY &&
        app_definition.events[1].type == DCC_APP_EXTENSION_MESSAGE_UPDATE &&
        app_definition.events[1].user_data == &app_state &&
        dm_route_policy.dm_only == 1U &&
        nsfw_route_policy.nsfw_only == 1U &&
        dm_app_definition.dm_only == 1U &&
        nsfw_app_definition.nsfw_only == 1U &&
        preset_dev_options.client.enable_cache == 1U &&
        preset_dev_options.client.infer_guild_id_from_channel == 1U &&
        preset_dev_options.command_registry.guild_id == 42ULL &&
        preset_dev_options.command_sync.command_registry.guild_id == 42ULL &&
        preset_dev_options.auto_defer_ephemeral == 1U &&
        default_auto_defer_options.auto_defer_after_ms == 1500U &&
        default_auto_defer_options.auto_defer_ephemeral == 0U &&
        private_auto_defer_options.auto_defer_after_ms == 1750U &&
        private_auto_defer_options.auto_defer_ephemeral == 1U &&
        command_group_alias.options_count == 1U &&
        command_group_alias.options[0].type == DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND_GROUP &&
        command_group_alias.options[0].options[0].type == DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND &&
        strcmp(command_tree_alias.name, "package_tree") == 0 &&
        command_tree_alias.options_count == 1U &&
        strcmp(command_tree_alias.options[0].name, "tools") == 0 &&
        strcmp(command_tree_alias.options[0].options[0].name, "sync") == 0 &&
        package_env.name != NULL &&
        package_env.description != NULL &&
        env_definition.required_env_count == 1U &&
        env_bindings[0].type == DCC_APP_ENV_BIND_STRING &&
        env_bindings[0].required == 1U &&
        env_bindings[1].type == DCC_APP_ENV_BIND_BOOL &&
        env_bindings[1].has_fallback == 1U &&
        env_bindings[1].fallback_bool == 1U &&
        env_bindings[2].fallback_u64 == 64U &&
        env_bindings[3].fallback_i64 == -64 &&
        env_bindings[4].type == DCC_APP_ENV_BIND_CHANNEL &&
        env_bindings[5].type == DCC_APP_ENV_BIND_INTENTS &&
        env_bindings[5].fallback_intents == DCC_INTENTS_DEFAULT &&
        env_get_token_fn != NULL &&
        env_get_string_fn != NULL &&
        env_get_bool_fn != NULL &&
        env_get_u64_fn != NULL &&
        env_get_u32_fn != NULL &&
        env_get_u32_range_or_fn != NULL &&
        env_get_i64_fn != NULL &&
        env_get_snowflake_fn != NULL &&
        env_get_intents_fn != NULL &&
        env_bind_fn != NULL &&
        dcc_app_validate_env_requirements(NULL, 0U) == DCC_OK &&
        dcc_app_validate_definition_env(NULL) == DCC_ERR_INVALID_ARG &&
        run_env_defined_fn != NULL &&
        run_dotenv_defined_fn != NULL &&
        invalid_run_defined == DCC_ERR_INVALID_ARG &&
        invalid_run_macro == DCC_ERR_INVALID_ARG &&
        dcc_app_create_defined(&options, NULL, &created_defined_app) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_defined(&options, &app_definition, &created_defined_app) == DCC_OK &&
        created_defined_app != NULL &&
        dcc_app_every_minutes(app, 5U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_every_hours(app, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_every_kst(app, "00:00", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_on_error(app, dcc_package_consumer_app_error_handler, NULL) == DCC_OK &&
        dcc_app_on(app, DCC_EVENT_RESUMED, dcc_package_consumer_app_event_handler, NULL) == DCC_OK &&
        dcc_app_on_ready(app, dcc_package_consumer_app_ready_handler, NULL) == DCC_OK &&
        DCC_APP_ON_READY_ONCE(app, dcc_package_consumer_app_ready_handler) == DCC_OK &&
        dcc_app_on_message_create(app, dcc_package_consumer_app_message_handler, NULL) == DCC_OK &&
        dcc_app_on_message_command(
            app,
            "!",
            "package",
            dcc_package_consumer_app_message_command_handler,
            NULL
        ) == DCC_OK &&
        dcc_app_on_message_update(app, dcc_package_consumer_app_message_handler, NULL) == DCC_OK &&
        dcc_app_on_message_delete(app, dcc_package_consumer_app_message_handler, NULL) == DCC_OK &&
        dcc_app_extension_register(app, &extension) == DCC_OK &&
        extension.context_menu_count == 2U &&
        extension.context_menus[0].type == DCC_APP_EXTENSION_USER_CONTEXT_MENU &&
        extension.context_menus[1].type == DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU &&
        DCC_APP_ON_USER_CONTEXT_MENU_DATA(app, "Package direct user", dcc_package_consumer_app_handler, &app_state) ==
            DCC_OK &&
        DCC_APP_ON_MESSAGE_CONTEXT_MENU(app, "Package direct message", dcc_package_consumer_app_handler) == DCC_OK &&
        dcc_app_slash_typed(NULL, &typed_slash) == DCC_ERR_INVALID_ARG &&
        dcc_app_slash_typed(app, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_REGISTER_COMMAND(app, typed_slash) == DCC_OK &&
        DCC_APP_REGISTER_COMMAND(app, policy_typed_slash) == DCC_OK &&
        DCC_APP_REGISTER_COMMAND(app, inline_typed_slash) == DCC_OK &&
        DCC_APP_REGISTER_COMMAND(app, on_typed_slash_alias) == DCC_OK &&
        DCC_APP_REGISTER_COMMAND(app, params_typed_slash) == DCC_OK &&
        params_typed_slash.command->options_count == 5U &&
        params_typed_slash.bindings.count == 5U &&
        params_typed_slash.validators.count == 2U &&
        params_typed_slash.command->options[1].type == DCC_APPLICATION_COMMAND_OPTION_INTEGER &&
        params_typed_slash.command->options[1].min_integer_value == 1 &&
        params_typed_slash.command->options[1].max_integer_value == 100 &&
        params_typed_slash.command->options[3].required == 1U &&
        strcmp(params_schema_command.name, "package_params_schema") == 0 &&
        params_schema_command.options_count == 5U &&
        params_schema_command.options[1].has_min_integer_value == 1U &&
        params_schema_command.options[1].has_max_integer_value == 1U &&
        params_schema_command.options[4].type == DCC_APPLICATION_COMMAND_OPTION_BOOLEAN &&
        strcmp(params_subcommand_command.name, "package_admin_params") == 0 &&
        params_subcommand_command.options_count == 1U &&
        params_subcommand_command.options[0].type == DCC_APPLICATION_COMMAND_OPTION_SUB_COMMAND &&
        strcmp(params_subcommand_command.options[0].name, "inspect") == 0 &&
        params_subcommand_command.options[0].options_count == 5U &&
        params_subcommand_command.options[0].options[0].required == 1U &&
        params_subcommand_command.options[0].options[1].max_integer_value == 100 &&
        dcc_app_subcommand_typed(NULL, &typed_subcommand) == DCC_ERR_INVALID_ARG &&
        dcc_app_subcommand_typed(app, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_REGISTER_SUBCOMMAND(app, typed_subcommand) == DCC_OK &&
        DCC_APP_REGISTER_SUBCOMMAND(app, params_typed_subcommand) == DCC_OK &&
        params_typed_subcommand.command == &params_subcommand_command &&
        params_typed_subcommand.command_name == NULL &&
        strcmp(params_typed_subcommand.subcommand_path, "inspect") == 0 &&
        params_typed_subcommand.bindings.count == 5U &&
        params_typed_subcommand.validators.count == 0U &&
        params_typed_subcommand.user_data == &app_state &&
        named_params_typed_subcommand.command == NULL &&
        strcmp(named_params_typed_subcommand.command_name, "package_admin") == 0 &&
        strcmp(named_params_typed_subcommand.subcommand_path, "reload/named") == 0 &&
        named_params_typed_subcommand.bindings.count == 5U &&
        named_params_typed_subcommand.validators.count == 2U &&
        strcmp(DCC_SUBCOMMAND_PATH("reload", "in_named"), "reload/in_named") == 0 &&
        in_params_typed_subcommand.command == &params_subcommand_command &&
        strcmp(in_params_typed_subcommand.subcommand_path, "tools/inspect") == 0 &&
        in_params_typed_subcommand.bindings.count == 5U &&
        in_params_typed_subcommand.user_data == &app_state &&
        strcmp(in_named_params_typed_subcommand.command_name, "package_admin") == 0 &&
        strcmp(in_named_params_typed_subcommand.subcommand_path, "reload/in_named") == 0 &&
        in_named_params_typed_subcommand.bindings.count == 5U &&
        in_named_params_typed_subcommand.user_data == &app_state &&
        dcc_app_autocomplete_typed(NULL, &typed_autocomplete) == DCC_ERR_INVALID_ARG &&
        dcc_app_autocomplete_typed(app, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_REGISTER_AUTOCOMPLETE(app, typed_autocomplete) == DCC_OK &&
        DCC_APP_REGISTER_AUTOCOMPLETE(app, typed_subcommand_autocomplete) == DCC_OK &&
        params_typed_autocomplete.command == &typed_command &&
        params_typed_autocomplete.command_name == NULL &&
        params_typed_autocomplete.bindings.count == 5U &&
        params_typed_autocomplete.user_data == &app_state &&
        strcmp(policy_params_typed_autocomplete.command_name, "package_typed_policy") == 0 &&
        policy_params_typed_autocomplete.policy.guild_only == 1U &&
        policy_params_typed_autocomplete.bindings.count == 5U &&
        strcmp(params_typed_subcommand_autocomplete.command_name, "package_admin") == 0 &&
        strcmp(params_typed_subcommand_autocomplete.subcommand_path, "reload") == 0 &&
        params_typed_subcommand_autocomplete.bindings.count == 5U &&
        params_typed_subcommand_autocomplete.user_data == &app_state &&
        strcmp(in_params_typed_subcommand_autocomplete.command_name, "package_admin") == 0 &&
        strcmp(in_params_typed_subcommand_autocomplete.subcommand_path, "reload/complete") == 0 &&
        in_params_typed_subcommand_autocomplete.bindings.count == 5U &&
        in_params_typed_subcommand_autocomplete.user_data == &app_state &&
        dcc_app_modal_typed(NULL, &typed_modal) == DCC_ERR_INVALID_ARG &&
        dcc_app_modal_typed(app, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_REGISTER_MODAL(app, typed_modal) == DCC_OK &&
        params_typed_modal.bindings.count == 2U &&
        params_typed_modal.user_data == &app_state &&
        DCC_APP_REGISTER_MODAL(app, params_typed_modal) == DCC_OK &&
        strcmp(ns_params_typed_modal.custom_id, "package.ns.modal") == 0 &&
        ns_params_typed_modal.bindings.count == 2U &&
        ns_params_typed_modal.user_data == &app_state &&
        DCC_APP_REGISTER_MODAL(app, ns_params_typed_modal) == DCC_OK &&
        DCC_APP_REGISTER_MODAL_FLOW(app, invalid_package_modal_flow) == DCC_ERR_INVALID_ARG &&
        dcc_app_modal_flow_register(NULL, &package_modal_flow) == DCC_ERR_INVALID_ARG &&
        dcc_app_modal_flow_register(app, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_REGISTER_MODAL_FLOW(app, package_modal_flow) == DCC_OK &&
        params_modal_flow.route.bindings.count == 2U &&
        params_modal_flow.route.user_data == &app_state &&
        params_modal_flow.modal.components_v2_count == 1U &&
        DCC_APP_REGISTER_MODAL_FLOW(app, params_modal_flow) == DCC_OK &&
        strcmp(ns_params_modal_flow.modal.custom_id, "package.ns.flow") == 0 &&
        strcmp(ns_params_modal_flow.route.custom_id, "package.ns.flow") == 0 &&
        ns_params_modal_flow.route.bindings.count == 2U &&
        ns_params_modal_flow.route.user_data == &app_state &&
        DCC_APP_REGISTER_MODAL_FLOW(app, ns_params_modal_flow) == DCC_OK &&
        dcc_app_button_typed(NULL, &typed_button) == DCC_ERR_INVALID_ARG &&
        dcc_app_button_typed(app, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_REGISTER_BUTTON(app, typed_button) == DCC_OK &&
        params_typed_button.bindings.count == 1U &&
        params_typed_button.user_data == &app_state &&
        DCC_APP_REGISTER_BUTTON(app, params_typed_button) == DCC_OK &&
        strcmp(ns_typed_button.custom_id, "package.ns.button") == 0 &&
        DCC_APP_REGISTER_BUTTON(app, ns_typed_button) == DCC_OK &&
        strcmp(ns_params_typed_button.custom_id, "package.ns.button:") == 0 &&
        ns_params_typed_button.prefix == 1U &&
        ns_params_typed_button.bindings.count == 1U &&
        ns_params_typed_button.user_data == &app_state &&
        DCC_APP_REGISTER_BUTTON(app, ns_params_typed_button) == DCC_OK &&
        dcc_app_select_typed(NULL, &typed_select) == DCC_ERR_INVALID_ARG &&
        dcc_app_select_typed(app, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_REGISTER_SELECT(app, typed_select) == DCC_OK &&
        params_typed_select.bindings.count == 2U &&
        params_typed_select.user_data == &app_state &&
        DCC_APP_REGISTER_SELECT(app, params_typed_select) == DCC_OK &&
        strcmp(ns_typed_select.custom_id, "package.ns.select") == 0 &&
        DCC_APP_REGISTER_SELECT(app, ns_typed_select) == DCC_OK &&
        strcmp(ns_params_typed_select.custom_id, "package.ns.select.params") == 0 &&
        ns_params_typed_select.bindings.count == 2U &&
        ns_params_typed_select.user_data == &app_state &&
        DCC_APP_REGISTER_SELECT(app, ns_params_typed_select) == DCC_OK &&
        dcc_app_send(NULL, 1U, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_send_with_id(NULL, 1U, &managed_message, dcc_package_consumer_app_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_send_text(app, 0U, "hello", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_send_text_with_id(app, 0U, "hello", dcc_package_consumer_app_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_send_json(app, 0U, "{}", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_edit_message(app, 0U, 1U, &managed_message, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_edit_message_text(app, 0U, 1U, "hello", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_delete_message(app, 0U, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_thread_from_message(app, 0U, 1U, &thread_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_create_thread_from_message_name(app, 0U, 1U, "thread", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_modify_thread(app, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_archive_thread(app, 0U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_lock_thread(app, 0U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_unlock_thread(app, 0U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_add_member_role(app, 0U, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_remove_member_role(app, 0U, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_managed_message_publish_latest(NULL, &managed, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_managed_message_publish_latest(app, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_managed_message_publish_latest_store(NULL, "latest", 123U, &managed_message, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_managed_message_publish_latest_store(app, NULL, 123U, &managed_message, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_managed_message_publish_latest_store(app, "latest", 0U, &managed_message, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_managed_message_publish_latest_store(app, "latest", 123U, NULL, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_managed_message_publish_latest_store(app, "latest", 123U, &managed_message, NULL, NULL) ==
            DCC_ERR_STATE &&
        DCC_APP_PUBLISH_LATEST(NULL, &managed, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_PUBLISH_LATEST_STORE(app, "latest", 123U, &managed_message, NULL, NULL) == DCC_ERR_STATE &&
        dcc_app_store(NULL) == NULL &&
        dcc_app_store_open_file(NULL, "package-state.kv") == DCC_ERR_INVALID_ARG &&
        dcc_store_init(NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_store_get(NULL, "key", NULL, 0U, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_store_file_open(NULL, &package_store) == DCC_ERR_INVALID_ARG &&
        store_binding.size == sizeof(store_binding) &&
        store_binding.store == &package_store &&
        app_store_binding.size == sizeof(app_store_binding) &&
        app_store_binding.store == NULL &&
        strcmp(app_store_binding.key, "package.app.latest") == 0 &&
        store_managed.load == dcc_store_load_managed_message_ref &&
        store_managed.save == dcc_store_save_managed_message_ref &&
        ui_message.flags == DCC_MESSAGE_FLAG_IS_COMPONENTS_V2 &&
        ui_message.has_flags == 1U &&
        ui_message.components_v2_count == 1U &&
        ui_message.components_v2[0].type == DCC_COMPONENT_V2_CONTAINER &&
        ui_message.components_v2[0].accent_color == 0x5865F2U &&
        ui_message.components_v2[0].children_count == 2U &&
        ui_message.components_v2[0].children[1].children_count == 1U &&
        strcmp(ui_message.components_v2[0].children[1].children[0].label, "Open") == 0 &&
        strcmp(legacy_button_ns.custom_id, "package.legacy.open") == 0 &&
        strcmp(ui_button_ns.custom_id, "package.ui.open") == 0 &&
        strcmp(ui_select_ns.custom_id, "package.ui.select") == 0 &&
        strcmp(ui_input_ns.custom_id, "package.ui.input") == 0 &&
        ui_ephemeral.flags == (DCC_MESSAGE_FLAG_EPHEMERAL | DCC_MESSAGE_FLAG_IS_COMPONENTS_V2) &&
        ui_ephemeral.components_v2_count == 1U &&
        strcmp(ui_ephemeral.components_v2[0].content, "Private") == 0 &&
        confirm_message.has_content == 1U &&
        confirm_message.components_count == 1U &&
        strcmp(confirm_message.components[0].children[0].custom_id, "package.confirm") == 0 &&
        paginator_message.flags == DCC_MESSAGE_FLAG_IS_COMPONENTS_V2 &&
        paginator_message.components_v2_count == 1U &&
        paginator_message.components_v2[0].children[1].children[0].disabled == 1U &&
        paginator_message.components_v2[0].children[1].children[1].disabled == 0U &&
        settings_panel.components_count == 1U &&
        settings_panel.components[0].children_count == 4U &&
        settings_panel.components[0].children[1].disabled == 1U &&
        settings_panel.components[0].children[2].disabled == 0U &&
        settings_panel_v2.flags == DCC_MESSAGE_FLAG_IS_COMPONENTS_V2 &&
        settings_panel_v2.components_v2_count == 1U &&
        settings_panel_v2.components_v2[0].children[1].children_count == 4U &&
        settings_panel_v2.components_v2[0].children[1].children[1].disabled == 0U &&
        settings_panel_v2.components_v2[0].children[1].children[2].disabled == 1U &&
        sugar_flow.started_at_ms == 1000U &&
        sugar_flow.auto_defer_after_ms == 1500U &&
        DCC_FLOW_REPLY_TEXT(NULL, "package flow") == DCC_ERR_INVALID_ARG &&
        strcmp(DCC_FLOW_STATE_NAME(NULL), "failed") == 0 &&
        DCC_APP_SEND(NULL, 1U, DCC_MESSAGE_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_ID(NULL, 1U, DCC_MESSAGE_TEXT("hello"), dcc_package_consumer_app_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2(NULL, 1U, DCC_V2_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2_ID(NULL, 1U, dcc_package_consumer_app_message_id_cb, NULL, DCC_V2_TEXT("hello")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_UI(NULL, 1U, DCC_UI_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_UI_ID(NULL, 1U, dcc_package_consumer_app_message_id_cb, NULL, DCC_UI_TEXT("hello")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_SEND_V2_CB(NULL, 1U, NULL, NULL, DCC_V2_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_INFER_GUILD_FROM_CHANNEL(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_infer_guild_id_from_channel(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_channel(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_channels(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_guild_channel(NULL, 1U, "{\"name\":\"general\"}", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_guild_channel_params(NULL, &channel_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_channel(NULL, 1U, "{\"name\":\"general\"}", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_channel_params(NULL, &channel_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_delete_channel(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_trigger_channel_typing(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_follow_news_channel(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_set_channel_voice_status(NULL, 1U, "Streaming", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_current_user_voice_state(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_current_user_voice_state_params(NULL, &voice_state_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_get_user_voice_state(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_user_voice_state_params(NULL, &voice_state_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_voice_regions(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_voice_regions(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_guild_channel_positions(NULL, 1U, "[{\"id\":\"2\",\"position\":1}]", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_modify_guild_channel_positions_params(NULL, &channel_positions_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_get_channel_invites(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_channel_invite(NULL, 1U, "{\"max_age\":3600}", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_channel_invite_params(NULL, &invite_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_channel_permission(NULL, 1U, 2U, "{\"allow\":\"1024\"}", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_modify_channel_permission_params(NULL, 1U, &permission_overwrite, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_delete_channel_permission(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_CHANNEL(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_CHANNELS(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_CREATE_GUILD_CHANNEL(NULL, 1U, "{\"name\":\"general\"}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_CREATE_GUILD_CHANNEL_PARAMS(NULL, &channel_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_CHANNEL(NULL, 1U, "{\"name\":\"general\"}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_CHANNEL_PARAMS(NULL, &channel_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_CHANNEL(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_TRIGGER_CHANNEL_TYPING(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_FOLLOW_NEWS_CHANNEL(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SET_CHANNEL_VOICE_STATUS(NULL, 1U, "Streaming") == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_MY_VOICE_STATE(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_MY_VOICE_STATE_PARAMS(NULL, &voice_state_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_USER_VOICE_STATE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_USER_VOICE_STATE_PARAMS(NULL, &voice_state_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_VOICE_REGIONS(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_VOICE_REGIONS(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_REORDER_GUILD_CHANNELS(NULL, 1U, "[{\"id\":\"2\",\"position\":1}]") == DCC_ERR_INVALID_ARG &&
        DCC_APP_REORDER_GUILD_CHANNELS_PARAMS(NULL, &channel_positions_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_CHANNEL_INVITES(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_CREATE_CHANNEL_INVITE(NULL, 1U, "{\"max_age\":3600}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_CREATE_CHANNEL_INVITE_PARAMS(NULL, &invite_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_CHANNEL_PERMISSION(NULL, 1U, 2U, "{\"allow\":\"1024\"}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_CHANNEL_PERMISSION_PARAMS(NULL, 1U, &permission_overwrite) == DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_CHANNEL_PERMISSION(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND(NULL, 1U, DCC_MESSAGE_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_TEXT(NULL, 1U, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_SAFE(NULL, 1U, "@everyone safe") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_V2(NULL, 1U, DCC_V2_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_UI(NULL, 1U, DCC_UI_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SEND_TEXT_F(NULL, 1U, "hello %d", 1) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_EDIT(NULL, 1U, "{\"name\":\"general\"}") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_EDIT_PARAMS(NULL, &channel_params) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_DELETE(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_TYPING(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_FOLLOW(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_SET_VOICE_STATUS(NULL, 1U, "Streaming") == DCC_ERR_INVALID_ARG &&
        DCC_MY_VOICE_STATE_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_MY_VOICE_STATE_EDIT_PARAMS(NULL, &voice_state_params) == DCC_ERR_INVALID_ARG &&
        DCC_USER_VOICE_STATE_FETCH(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_USER_VOICE_STATE_EDIT_PARAMS(NULL, &voice_state_params) == DCC_ERR_INVALID_ARG &&
        DCC_VOICE_REGIONS_FETCH(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_VOICE_REGIONS_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_INVITES_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_INVITE_CREATE(NULL, 1U, "{\"max_age\":3600}") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_INVITE_CREATE_PARAMS(NULL, &invite_params) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_PERMISSION_EDIT(NULL, 1U, 2U, "{\"allow\":\"1024\"}") == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_PERMISSION_EDIT_PARAMS(NULL, 1U, &permission_overwrite) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_PERMISSION_DELETE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_CHANNELS_REORDER(NULL, 1U, "[{\"id\":\"2\",\"position\":1}]") == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_CHANNELS_REORDER_PARAMS(NULL, &channel_positions_params) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_CHANNELS_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_CHANNEL_CREATE(NULL, 1U, "{\"name\":\"general\"}") == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_CHANNEL_CREATE_PARAMS(NULL, &channel_params) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_roles(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_guild_role(NULL, 1U, "{\"name\":\"role\"}", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_guild_role_params(NULL, &role_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_guild_role(NULL, 1U, 2U, "{\"name\":\"role\"}", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_guild_role_params(NULL, &role_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_guild_role_positions(NULL, 1U, role_positions, DCC_ARRAY_LEN(role_positions), NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_delete_guild_role(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_ROLES(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_CREATE_GUILD_ROLE(NULL, 1U, "{\"name\":\"role\"}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_CREATE_GUILD_ROLE_PARAMS(NULL, &role_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_GUILD_ROLE(NULL, 1U, 2U, "{\"name\":\"role\"}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_GUILD_ROLE_PARAMS(NULL, &role_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_REORDER_GUILD_ROLES(NULL, 1U, role_positions, DCC_ARRAY_LEN(role_positions)) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_GUILD_ROLE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_ROLES_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_ROLES_REORDER(NULL, 1U, role_positions, DCC_ARRAY_LEN(role_positions)) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_ROLE_CREATE(NULL, 1U, "{\"name\":\"role\"}") == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_ROLE_CREATE_PARAMS(NULL, &role_params) == DCC_ERR_INVALID_ARG &&
        DCC_ROLE_EDIT(NULL, 1U, 2U, "{\"name\":\"role\"}") == DCC_ERR_INVALID_ARG &&
        DCC_ROLE_EDIT_PARAMS(NULL, &role_params) == DCC_ERR_INVALID_ARG &&
        DCC_ROLE_DELETE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_MESSAGE(NULL, 1U, 2U, DCC_MESSAGE_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_V2(NULL, 1U, 2U, DCC_V2_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_UI(NULL, 1U, 2U, DCC_UI_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_V2_CB(NULL, 1U, 2U, NULL, NULL, DCC_V2_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_EDIT(NULL, 1U, 2U, DCC_MESSAGE_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_EDIT_TEXT(NULL, 1U, 2U, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_EDIT_V2(NULL, 1U, 2U, DCC_V2_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_EDIT_UI(NULL, 1U, 2U, DCC_UI_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_EDIT_TEXT_F(NULL, 1U, 2U, "hello %d", 1) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_DELETE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        dcc_app_crosspost_message(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_pin_message(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_unpin_message(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_add_message_reaction(NULL, 1U, 2U, "thumbsup", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_delete_own_message_reaction(NULL, 1U, 2U, "thumbsup", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_delete_user_message_reaction(NULL, 1U, 2U, "thumbsup", 3U, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_delete_all_message_reactions(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_delete_all_message_reactions_for_emoji(NULL, 1U, 2U, "thumbsup", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_CROSSPOST_MESSAGE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_PIN_MESSAGE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_UNPIN_MESSAGE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ADD_MESSAGE_REACTION(NULL, 1U, 2U, "thumbsup") == DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_OWN_MESSAGE_REACTION(NULL, 1U, 2U, "thumbsup") == DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_USER_MESSAGE_REACTION(NULL, 1U, 2U, "thumbsup", 3U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_CLEAR_MESSAGE_REACTIONS(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI(NULL, 1U, 2U, "thumbsup") == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_CROSSPOST(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_PIN(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_UNPIN(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_REACT(NULL, 1U, 2U, "thumbsup") == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_UNREACT(NULL, 1U, 2U, "thumbsup") == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_REMOVE_REACTION(NULL, 1U, 2U, "thumbsup", 3U) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_CLEAR_REACTIONS(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_CLEAR_REACTION_EMOJI(NULL, 1U, 2U, "thumbsup") == DCC_ERR_INVALID_ARG &&
        DCC_APP_THREAD_FROM_MESSAGE(NULL, 1U, 2U, "thread") == DCC_ERR_INVALID_ARG &&
        DCC_APP_THREAD_FROM_MESSAGE_PARAMS(NULL, 1U, 2U, &thread_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_LOCK_THREAD(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_MESSAGE_THREAD(NULL, 1U, 2U, "thread") == DCC_ERR_INVALID_ARG &&
        DCC_THREAD_FROM_MESSAGE(NULL, 1U, 2U, "thread") == DCC_ERR_INVALID_ARG &&
        DCC_THREAD_LOCK(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_THREAD_UNLOCK(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_THREAD_FROM_MESSAGE(NULL, "thread") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_THREAD_FROM_MESSAGE_PARAMS(NULL, &thread_params) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_CREATE_THREAD(NULL, "thread") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_CREATE_THREAD_PARAMS(NULL, &thread_params) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_ARCHIVE_THREAD(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_LOCK_THREAD(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UNLOCK_THREAD(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_CLOSE_THREAD(NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_create_thread_from_message_name(NULL, "thread", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_create_thread_from_message(NULL, &thread_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_archive_current_thread(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_lock_current_thread(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_unlock_current_thread(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ADD_MEMBER_ROLE(NULL, 1U, 2U, 3U) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_ADD_ROLE(NULL, 1U, 2U, 3U) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_REMOVE_ROLE(NULL, 1U, 2U, 3U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_ADD_ROLE(NULL, 1U, 2U, 3U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_REMOVE_ROLE(NULL, 1U, 2U, 3U) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_member(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_set_member_timeout(NULL, 1U, 2U, "2026-06-23T00:00:00Z", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_clear_member_timeout(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_move_member(NULL, 1U, 2U, 3U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_remove_member(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_FETCH(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_TIMEOUT(NULL, 1U, 2U, "2026-06-23T00:00:00Z") == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_CLEAR_TIMEOUT(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_MOVE(NULL, 1U, 2U, 3U) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_KICK(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_FETCH(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_TIMEOUT(NULL, 1U, 2U, "2026-06-23T00:00:00Z") == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_bans(NULL, 1U, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_bans_page(NULL, 1U, 0U, 0U, 50U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_ban(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_guild_ban(NULL, 1U, 2U, "{\"delete_message_seconds\":60}", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_create_guild_ban_seconds(NULL, 1U, 2U, 60U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_guild_ban_params(NULL, &ban_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_delete_guild_ban(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_current_guild_member(NULL, 1U, "{\"nick\":\"package\"}", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_modify_current_guild_member_params(NULL, &current_member_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_set_current_guild_member_nickname(NULL, 1U, "{\"nick\":\"package\"}", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_set_current_guild_member_nickname_params(NULL, &nickname_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_prune_count_options(NULL, 1U, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles), NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_begin_guild_prune_options(NULL, 1U, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles), 1U, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_begin_guild_prune_params(NULL, &prune_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_BANS(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_BANS_QUERY(NULL, 1U, "limit=50") == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_BANS_PAGE(NULL, 1U, 0U, 0U, 50U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_BAN(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_BAN_MEMBER(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_BAN_MEMBER_SECONDS(NULL, 1U, 2U, 60U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_BAN_MEMBER_JSON(NULL, 1U, 2U, "{\"delete_message_seconds\":60}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_BAN_MEMBER_PARAMS(NULL, &ban_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_UNBAN_MEMBER(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_CURRENT_MEMBER(NULL, 1U, "{\"nick\":\"package\"}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_CURRENT_MEMBER_PARAMS(NULL, &current_member_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SET_MY_NICK_JSON(NULL, 1U, "{\"nick\":\"package\"}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_SET_MY_NICK_PARAMS(NULL, &nickname_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_SET_MY_NICK(NULL, 1U, "package") == DCC_ERR_INVALID_ARG &&
        DCC_CURRENT_MEMBER_EDIT(NULL, 1U, "{\"nick\":\"package\"}") == DCC_ERR_INVALID_ARG &&
        DCC_CURRENT_MEMBER_EDIT_PARAMS(NULL, &current_member_params) == DCC_ERR_INVALID_ARG &&
        DCC_CURRENT_MEMBER_SET_NICK(NULL, 1U, "package") == DCC_ERR_INVALID_ARG &&
        DCC_ME_EDIT(NULL, 1U, "{\"nick\":\"package\"}") == DCC_ERR_INVALID_ARG &&
        DCC_ME_EDIT_PARAMS(NULL, &current_member_params) == DCC_ERR_INVALID_ARG &&
        DCC_ME_SET_NICK(NULL, 1U, "package") == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_PRUNE_COUNT(NULL, 1U, 7U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_PRUNE_COUNT_OPTIONS(NULL, 1U, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles)) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_BEGIN_GUILD_PRUNE(NULL, 1U, 7U, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_BEGIN_GUILD_PRUNE_OPTIONS(NULL, 1U, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles), 1U) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_BEGIN_GUILD_PRUNE_PARAMS(NULL, &prune_params) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_BANS_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_BANS_FETCH_PAGE(NULL, 1U, 0U, 0U, 50U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_BAN_FETCH(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_BAN(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_BAN_SECONDS(NULL, 1U, 2U, 60U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_BAN_PARAMS(NULL, &ban_params) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_UNBAN(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_BAN(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_BAN_SECONDS(NULL, 1U, 2U, 60U) == DCC_ERR_INVALID_ARG &&
        DCC_MEMBER_UNBAN(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_PRUNE_COUNT(NULL, 1U, 7U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_PRUNE_COUNT_OPTIONS(NULL, 1U, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles)) ==
            DCC_ERR_INVALID_ARG &&
        DCC_GUILD_PRUNE_BEGIN(NULL, 1U, 7U, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_PRUNE_BEGIN_OPTIONS(NULL, 1U, 7U, prune_roles, DCC_ARRAY_LEN(prune_roles), 1U) ==
            DCC_ERR_INVALID_ARG &&
        DCC_GUILD_PRUNE_BEGIN_PARAMS(NULL, &prune_params) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_scheduled_events(NULL, 1U, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_scheduled_events_with_user_count(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_scheduled_event(NULL, 1U, 2U, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_scheduled_event_with_user_count(NULL, 1U, 2U, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_create_guild_scheduled_event(NULL, 1U, "{\"name\":\"event\"}", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_create_guild_scheduled_event_params(NULL, &scheduled_event_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_modify_guild_scheduled_event(NULL, 1U, 2U, "{\"name\":\"event\"}", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_modify_guild_scheduled_event_params(NULL, &scheduled_event_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_delete_guild_scheduled_event(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_scheduled_event_users(NULL, 1U, 2U, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_scheduled_event_users_page(NULL, 1U, 2U, 25U, 0U, 0U, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_SCHEDULED_EVENTS(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_SCHEDULED_EVENTS_QUERY(NULL, 1U, "with_user_count=true") == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_SCHEDULED_EVENTS_WITH_USER_COUNT(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_SCHEDULED_EVENT(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_SCHEDULED_EVENT_QUERY(NULL, 1U, 2U, "with_user_count=true") ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_SCHEDULED_EVENT_WITH_USER_COUNT(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_CREATE_GUILD_SCHEDULED_EVENT(NULL, 1U, "{\"name\":\"event\"}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_CREATE_GUILD_SCHEDULED_EVENT_PARAMS(NULL, &scheduled_event_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_GUILD_SCHEDULED_EVENT(NULL, 1U, 2U, "{\"name\":\"event\"}") == DCC_ERR_INVALID_ARG &&
        DCC_APP_EDIT_GUILD_SCHEDULED_EVENT_PARAMS(NULL, &scheduled_event_params) == DCC_ERR_INVALID_ARG &&
        DCC_APP_DELETE_GUILD_SCHEDULED_EVENT(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS_QUERY(NULL, 1U, 2U, "limit=25") == DCC_ERR_INVALID_ARG &&
        DCC_APP_GET_GUILD_SCHEDULED_EVENT_USERS_PAGE(NULL, 1U, 2U, 25U, 0U, 0U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_SCHEDULED_EVENTS_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_SCHEDULED_EVENTS_FETCH_WITH_USER_COUNT(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_SCHEDULED_EVENT_FETCH(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_SCHEDULED_EVENT_FETCH_WITH_USER_COUNT(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_SCHEDULED_EVENT_CREATE(NULL, 1U, "{\"name\":\"event\"}") == DCC_ERR_INVALID_ARG &&
        DCC_SCHEDULED_EVENT_CREATE_PARAMS(NULL, &scheduled_event_params) == DCC_ERR_INVALID_ARG &&
        DCC_SCHEDULED_EVENT_EDIT(NULL, 1U, 2U, "{\"name\":\"event\"}") == DCC_ERR_INVALID_ARG &&
        DCC_SCHEDULED_EVENT_EDIT_PARAMS(NULL, &scheduled_event_params) == DCC_ERR_INVALID_ARG &&
        DCC_SCHEDULED_EVENT_DELETE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_SCHEDULED_EVENT_USERS_FETCH(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_SCHEDULED_EVENT_USERS_FETCH_PAGE(NULL, 1U, 2U, 25U, 0U, 0U) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_channel_webhooks(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_webhooks(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_webhook(NULL, 4U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_webhook_with_token(NULL, 4U, "token", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_webhook(NULL, 1U, "{\"name\":\"package\"}", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_webhook_params(NULL, &webhook_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_webhook(NULL, 4U, "{\"name\":\"package\"}", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_webhook_params(NULL, &webhook_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_webhook_with_token(NULL, 4U, "token", "{\"name\":\"package\"}", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_modify_webhook_with_token_params(NULL, &webhook_token_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_delete_webhook(NULL, 4U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_delete_webhook_with_token(NULL, 4U, "token", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_execute_webhook_message(NULL, 4U, "token", 1U, 0U, 0U, &managed_message, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_execute_webhook_text(NULL, 4U, "token", 1U, 0U, "hello", NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_get_webhook_message(NULL, 4U, "token", 5U, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_webhook_message_builder(NULL, 4U, "token", 5U, NULL, &managed_message, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_delete_webhook_message(NULL, 4U, "token", 5U, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CHANNEL_WEBHOOKS_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_WEBHOOKS_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_FETCH(NULL, 4U) == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_FETCH_TOKEN(NULL, 4U, "token") == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_CREATE_PARAMS(NULL, &webhook_params) == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_EDIT_PARAMS(NULL, &webhook_params) == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_EDIT_TOKEN_PARAMS(NULL, &webhook_token_params) == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_DELETE(NULL, 4U) == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_DELETE_TOKEN(NULL, 4U, "token") == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_SEND_TEXT(NULL, 4U, "token", "hello") == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_SEND(NULL, 4U, "token", DCC_MESSAGE_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_SEND_UI(NULL, 4U, "token", DCC_UI_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_MESSAGE_FETCH(NULL, 4U, "token", 5U) == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_MESSAGE_EDIT(NULL, 4U, "token", 5U, DCC_MESSAGE_TEXT("edit")) == DCC_ERR_INVALID_ARG &&
        DCC_WEBHOOK_MESSAGE_DELETE(NULL, 4U, "token", 5U) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_gateway_bot(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_skus(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_entitlements_page(
            NULL,
            1U,
            3U,
            entitlement_sku_ids,
            DCC_ARRAY_LEN(entitlement_sku_ids),
            0U,
            0U,
            25U,
            1U,
            1U,
            NULL,
            NULL
        ) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_test_entitlement_params(NULL, &test_entitlement_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_consume_entitlement(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_current_user(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_modify_current_user_params(NULL, &user_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_user(NULL, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_dm_channel_params(NULL, &dm_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_add_group_dm_recipient_params(NULL, &group_dm_recipient_params, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_remove_group_dm_recipient(NULL, 3U, 4U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_direct_message_builder(NULL, 2U, &managed_message, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_create_direct_message_text(NULL, 2U, "hello", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_guild_invites(NULL, 1U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_invite_full(NULL, "abc", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_stage_instance_params(NULL, &stage_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_stage_instance(NULL, 3U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_get_template(NULL, "abc", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_guild_template_params(NULL, &template_params, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_GATEWAY_BOT_FETCH(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_SKUS_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_ENTITLEMENTS_FETCH_PAGE(
            NULL,
            1U,
            3U,
            entitlement_sku_ids,
            DCC_ARRAY_LEN(entitlement_sku_ids),
            0U,
            0U,
            25U,
            1U,
            1U
        ) == DCC_ERR_INVALID_ARG &&
        DCC_TEST_ENTITLEMENT_CREATE_PARAMS(NULL, &test_entitlement_params) == DCC_ERR_INVALID_ARG &&
        DCC_ENTITLEMENT_CONSUME(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_ME_FETCH(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_ME_EDIT_USER_PARAMS(NULL, &user_params) == DCC_ERR_INVALID_ARG &&
        DCC_USER_FETCH(NULL, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_USER_DM_CHANNEL_CREATE(NULL, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GROUP_DM_RECIPIENT_ADD_PARAMS(NULL, &group_dm_recipient_params) == DCC_ERR_INVALID_ARG &&
        DCC_GROUP_DM_RECIPIENT_REMOVE(NULL, 3U, 4U) == DCC_ERR_INVALID_ARG &&
        DCC_USER_DM_SEND(NULL, 2U, DCC_MESSAGE_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_USER_DM_SEND_TEXT(NULL, 2U, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_USER_DM_SEND_UI(NULL, 2U, DCC_UI_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_DM_SEND_TEXT(NULL, 2U, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_INVITES_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_INVITE_FETCH_FULL(NULL, "abc") == DCC_ERR_INVALID_ARG &&
        DCC_INVITE_DELETE(NULL, "abc") == DCC_ERR_INVALID_ARG &&
        DCC_STAGE_CREATE_PARAMS(NULL, &stage_params) == DCC_ERR_INVALID_ARG &&
        DCC_STAGE_FETCH(NULL, 3U) == DCC_ERR_INVALID_ARG &&
        DCC_STAGE_DELETE(NULL, 3U) == DCC_ERR_INVALID_ARG &&
        DCC_TEMPLATE_FETCH(NULL, "abc") == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_TEMPLATE_CREATE_PARAMS(NULL, &template_params) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_TEMPLATE_SYNC(NULL, 1U, "abc") == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_AUDIT_LOG_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_AUDIT_LOG_FETCH_PAGE(NULL, 1U, 2U, 20U, 0U, 0U, 50U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_INTEGRATIONS_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_INTEGRATION_SYNC(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_WIDGET_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_WIDGET_EDIT_PARAMS(NULL, 1U, &widget_params) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_VANITY_URL_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_ONBOARDING_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_ONBOARDING_EDIT_PARAMS(NULL, &onboarding_params) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_WELCOME_SCREEN_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_WELCOME_SCREEN_EDIT_PARAMS(NULL, 1U, &welcome_params) == DCC_ERR_INVALID_ARG &&
        DCC_AUTO_MOD_RULES_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_AUTO_MOD_RULE_FETCH(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_AUTO_MOD_RULE_CREATE_PARAMS(NULL, 1U, &auto_mod_rule_params) == DCC_ERR_INVALID_ARG &&
        DCC_AUTO_MOD_RULE_EDIT_PARAMS(NULL, 1U, &auto_mod_rule_params) == DCC_ERR_INVALID_ARG &&
        DCC_AUTO_MOD_RULE_DELETE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_EMOJIS_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_EMOJI_FETCH(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_EMOJI_CREATE_PARAMS(NULL, 1U, &emoji_params) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_EMOJI_EDIT_PARAMS(NULL, 1U, &emoji_params) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_EMOJI_DELETE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_STICKERS_FETCH(NULL, 1U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_STICKER_FETCH(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_STICKER_CREATE_PARAMS(NULL, &sticker_params) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_STICKER_DELETE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_STICKER_FETCH(NULL, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_STICKER_PACKS_FETCH(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MESSAGES_SEARCH(NULL, 1U, "content=package") == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MESSAGES_SEARCH_PARAMS(NULL, 1U, &message_search_params) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_CLEAR_TIMEOUT(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_MOVE(NULL, 1U, 2U, 3U) == DCC_ERR_INVALID_ARG &&
        DCC_GUILD_MEMBER_KICK(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        dcc_app_use(app, dcc_package_consumer_app_middleware, NULL) == DCC_OK &&
        dcc_app_use_guild_only(app) == DCC_OK &&
        dcc_app_use_dm_only(app) == DCC_OK &&
        dcc_app_use_nsfw_only(app) == DCC_OK &&
        dcc_app_use_channel_types(app, (uint32_t[]){DCC_CHANNEL_TEXT}, 1U) == DCC_OK &&
        dcc_app_use_owner(app, 123U) == DCC_OK &&
        dcc_app_use_required_permissions(app, DCC_PERMISSION_MANAGE_MESSAGES) == DCC_OK &&
        dcc_app_use_required_roles(app, (dcc_snowflake_t[]){789U}, 1U) == DCC_OK &&
        dcc_app_use_any_role(app, (dcc_snowflake_t[]){789U, 790U}, 2U) == DCC_OK &&
        dcc_app_use_check(app, &route_check) == DCC_OK &&
        dcc_app_use_cooldown(app, &cooldown) == DCC_OK &&
        dcc_app_slash_route(
            app,
            "package",
            "Package command",
            dcc_package_consumer_app_handler,
            NULL,
            &route_id
        ) ==
            DCC_OK &&
        route_id != DCC_APP_ROUTE_INVALID &&
        dcc_app_subcommand_route(
            app,
            "package",
            "admin/reload",
            dcc_package_consumer_app_handler,
            NULL,
            &subcommand_route_id
        ) ==
            DCC_OK &&
        subcommand_route_id != DCC_APP_ROUTE_INVALID &&
        dcc_app_route_use(app, route_id, dcc_package_consumer_app_middleware, NULL) == DCC_OK &&
        dcc_app_route_use_dm_only(app, route_id) == DCC_OK &&
        dcc_app_route_use_nsfw_only(app, route_id) == DCC_OK &&
        dcc_app_route_use_channel_types(app, route_id, (uint32_t[]){DCC_CHANNEL_TEXT}, 1U) == DCC_OK &&
        dcc_app_route_use_required_permissions(app, route_id, DCC_PERMISSION_MANAGE_MESSAGES) == DCC_OK &&
        dcc_app_route_use_required_roles(app, route_id, (dcc_snowflake_t[]){789U}, 1U) == DCC_OK &&
        dcc_app_route_use_any_role(app, route_id, (dcc_snowflake_t[]){789U, 790U}, 2U) == DCC_OK &&
        dcc_app_route_use_check(app, route_id, &route_check) == DCC_OK &&
        dcc_app_route_use_cooldown(app, route_id, &cooldown) == DCC_OK &&
        dcc_app_route_apply_policy(app, DCC_APP_ROUTE_INVALID, &route_policy) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SLASH_POLICY(
            app,
            "package_policy_direct",
            "Package policy direct command",
            dcc_package_consumer_app_handler,
            route_policy
        ) == DCC_OK &&
        DCC_APP_ON_BUTTON_POLICY(
            app,
            "package.policy.button.direct",
            dcc_package_consumer_app_handler,
            route_policy
        ) == DCC_OK &&
        DCC_APP_ON_BUTTON_NS(
            app,
            "package",
            "direct.button.ns",
            dcc_package_consumer_app_handler
        ) == DCC_OK &&
        DCC_APP_ON_BUTTON_NS_DATA_POLICY(
            app,
            "package",
            "policy.button.ns",
            dcc_package_consumer_app_handler,
            &app_state,
            route_policy
        ) == DCC_OK &&
        DCC_APP_ON_SELECT_NS_GUARDED(
            app,
            "package",
            "guarded.select.ns",
            dcc_package_consumer_app_handler,
            DCC_ONLY_DM()
        ) == DCC_OK &&
        DCC_APP_ON_MODAL_NS_PREFIX_DATA(
            app,
            "package",
            "direct.modal.ns:",
            dcc_package_consumer_app_handler,
            &app_state
        ) == DCC_OK &&
        DCC_APP_ON_MODAL_PREFIX_DATA_POLICY(
            app,
            "package.policy.modal:",
            dcc_package_consumer_app_handler,
            &app_state,
            route_policy
        ) == DCC_OK &&
        DCC_APP_ON_MODAL_NS_PREFIX_DATA_GUARDED(
            app,
            "package",
            "guarded.modal.ns:",
            dcc_package_consumer_app_handler,
            &app_state,
            DCC_ONLY_NSFW()
        ) == DCC_OK &&
        DCC_APP_ON_COMMAND_GUARDED(
            app,
            "package_guarded_command",
            "Package guarded command",
            dcc_package_consumer_app_handler,
            DCC_ONLY_GUILD(),
            DCC_CAN(DCC_PERMISSION_MANAGE_MESSAGES),
            DCC_RATE_LIMIT_USER(1U, 1000U)
        ) == DCC_OK &&
        DCC_APP_ON_SUBCOMMAND_DATA_GUARDED(
            app,
            "package",
            "admin/guarded",
            dcc_package_consumer_app_handler,
            &app_state,
            DCC_ONLY_GUILD(),
            DCC_OWNER_ONLY(123U)
        ) == DCC_OK &&
        DCC_APP_ON_AUTOCOMPLETE_GUARDED(
            app,
            "package_guarded_command",
            dcc_package_consumer_app_handler,
            DCC_RATE_LIMIT_GLOBAL(2U, 1000U)
        ) == DCC_OK &&
        DCC_APP_ON_BUTTON_PREFIX_DATA_GUARDED(
            app,
            "package.guarded.button:",
            dcc_package_consumer_app_handler,
            &app_state,
            DCC_HAS_ANY_ROLE(789U, 790U)
        ) == DCC_OK &&
        DCC_APP_ON_SELECT_GUARDED(
            app,
            "package.guarded.select",
            dcc_package_consumer_app_handler,
            DCC_ONLY_DM()
        ) == DCC_OK &&
        DCC_APP_ON_MODAL_PREFIX_DATA_GUARDED(
            app,
            "package.guarded.modal:",
            dcc_package_consumer_app_handler,
            &app_state,
            DCC_ONLY_NSFW()
        ) == DCC_OK &&
        dcc_app_autocomplete_route(
            app,
            "package",
            dcc_package_consumer_app_handler,
            NULL,
            &autocomplete_route_id
        ) ==
            DCC_OK &&
        autocomplete_route_id != DCC_APP_ROUTE_INVALID &&
        dcc_app_view_register(app, &view) == DCC_OK &&
        dcc_app_view_register(app, &namespace_view) == DCC_OK &&
        dcc_app_view_register(app, &namespace_action_view) == DCC_OK &&
        DCC_GROUP_INIT(&group, app, "package:") == DCC_OK &&
        DCC_GROUP_BUTTON_ROUTE(
            &group,
            "button",
            dcc_package_consumer_app_handler,
            &group_route_id
        ) == DCC_OK &&
        group_route_id != DCC_APP_ROUTE_INVALID &&
        DCC_GROUP_SELECT(&group, "select", dcc_package_consumer_app_handler) == DCC_OK &&
        DCC_GROUP_MODAL(&group, "modal", dcc_package_consumer_app_handler) == DCC_OK &&
        DCC_GROUP_BUTTON_PREFIX(&group, "page:", dcc_package_consumer_app_handler) == DCC_OK &&
        DCC_GROUP_VIEW(&group, &group_view) == DCC_OK &&
        DCC_GROUP_USE(&group, dcc_package_consumer_app_middleware, NULL) == DCC_OK &&
        DCC_GROUP_GUILD_ONLY(&group) == DCC_OK &&
        DCC_GROUP_DM_ONLY(&group) == DCC_OK &&
        DCC_GROUP_NSFW_ONLY(&group) == DCC_OK &&
        DCC_GROUP_CHANNEL_TYPES(&group, DCC_CHANNEL_TEXT) == DCC_OK &&
        DCC_GROUP_OWNER(&group, 123U) == DCC_OK &&
        DCC_GROUP_OWNERS(&group, 123U, 456U) == DCC_OK &&
        DCC_GROUP_REQUIRED_PERMISSIONS(&group, DCC_PERMISSION_MANAGE_MESSAGES) == DCC_OK &&
        DCC_GROUP_REQUIRE_ROLE(&group, 789U) == DCC_OK &&
        DCC_GROUP_REQUIRE_ANY_ROLE(&group, 789U, 790U) == DCC_OK &&
        DCC_GROUP_CHECK(&group, &route_check) == DCC_OK &&
        DCC_GROUP_COOLDOWN(&group, &cooldown) == DCC_OK &&
        DCC_GROUP_BUTTON(&policy_group, "button", dcc_package_consumer_app_handler) == DCC_OK &&
        DCC_GROUP_SELECT(&policy_group, "select", dcc_package_consumer_app_handler) == DCC_OK &&
        DCC_GROUP_MODAL_PREFIX(&policy_group, "modal:", dcc_package_consumer_app_handler) == DCC_OK &&
        DCC_GROUP_POLICY(&policy_group, route_policy) == DCC_OK &&
        dcc_ctx_bind_options(NULL, NULL, 0U) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_bind_option_fields(NULL, &args, option_fields, 2U) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_bind_form(NULL, NULL, 0U) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_bind_form_fields(NULL, &args, form_fields, 2U) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_bind_component(NULL, NULL, 0U) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_bind_component_fields(NULL, &args, component_fields, 2U) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_validate_fields(NULL, validators, DCC_ARRAY_LEN(validators), NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_validate_fields(&args, validators, DCC_ARRAY_LEN(validators), &validation_error) ==
            DCC_OK &&
        validation_error.status == DCC_OK &&
        DCC_CTX_VALIDATE_FIELDS(
            &invalid_args,
            &invalid_error,
            DCC_VALIDATE_STRING_REQUIRED(dcc_package_consumer_app_args_t, name, "name")
        ) == DCC_ERR_INVALID_ARG &&
        strcmp(invalid_error.field, "name") == 0 &&
        DCC_PARSE_BIRTHDAY("0229", &parsed_date) == DCC_OK &&
        parsed_date.month == 2U &&
        parsed_date.day == 29U &&
        parsed_date.has_year == 0U &&
        DCC_PARSE_DATE_YYYYMMDD("20040229", &parsed_date) == DCC_OK &&
        parsed_date.year == 2004 &&
        parsed_date.month == 2U &&
        parsed_date.day == 29U &&
        parsed_date.has_year == 1U &&
        DCC_PARSE_BIRTHDAY("2007", &parsed_date) == DCC_ERR_INVALID_ARG &&
        DCC_PARSE_DATE_YYYYMMDD("20010229", &parsed_date) == DCC_ERR_INVALID_ARG &&
        DCC_PARSE_DATE_MMDD("1301", &parsed_date) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_send(NULL, NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_send_with_id(NULL, &managed_message, dcc_package_consumer_app_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_ctx_send_text(NULL, "hello", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_send_text_with_id(NULL, "hello", dcc_package_consumer_app_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_ctx_add_member_role(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_remove_member_role(NULL, 1U, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_add_author_role(NULL, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_remove_author_role(NULL, 2U, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_MEMBER_ADD_ROLE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_MEMBER_REMOVE_ROLE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_AUTHOR_ADD_ROLE(NULL, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_AUTHOR_REMOVE_ROLE(NULL, 2U) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_ok(NULL, "ok", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_fail(NULL, "bad", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY(NULL, DCC_MESSAGE_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLY(NULL, DCC_MESSAGE_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLY_UI(NULL, DCC_UI_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_TEXT(NULL, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLY_TEXT(NULL, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REPLY_TEXT_F(NULL, "hello %d", 1) == DCC_ERR_INVALID_ARG &&
        DCC_PUBLIC(NULL, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_PUBLIC(NULL, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_EPHEMERAL(NULL, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_EPHEMERAL(NULL, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EPHEMERAL(NULL, "hello") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EPHEMERAL_F(NULL, "hello %d", 2) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EPHEMERAL_UI(NULL, DCC_UI_TEXT("hello")) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_PUBLIC_EMBED(NULL, "Title", "hello", 0x57F287U) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_EPHEMERAL_EMBED(NULL, "Title", "hello", 0x57F287U) == DCC_ERR_INVALID_ARG &&
        DCC_ERROR(NULL, "Error", "hello") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_ERROR(NULL, "Error", "hello") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_VALIDATION_ERROR(NULL, DCC_ERR_INVALID_ARG) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_VALIDATION_RESULT(NULL, DCC_ERR_INVALID_ARG, &invalid_error) == DCC_ERR_INVALID_ARG &&
        DCC_SUCCESS(NULL, "Done", "hello") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SUCCESS(NULL, "Done", "hello") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SUCCESS_F(NULL, "Done", "hello %d", 3) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_DONE(NULL, "ok") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_DONE_CB(NULL, "ok", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_DONE(NULL, "ok") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_DONE(NULL, "ok") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_OK(NULL, "ok") == DCC_ERR_INVALID_ARG &&
        DCC_DONE_CB(NULL, "ok", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_ACK(NULL, "ok") == DCC_ERR_INVALID_ARG &&
        DCC_ACK_CB(NULL, "ok", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_FAIL(NULL, "bad") == DCC_ERR_INVALID_ARG &&
        DCC_REPLY_FAIL_CB(NULL, "bad", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_FAIL(NULL, "bad") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FAIL(NULL, "bad") == DCC_ERR_INVALID_ARG &&
        DCC_FAIL_CB(NULL, "bad", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_DENY(NULL, "denied") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_DENY(NULL, "denied") == DCC_ERR_INVALID_ARG &&
        DCC_DENY_CB(NULL, "denied", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_BAD_INPUT(NULL, "bad input") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_BAD_INPUT(NULL, "bad input") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_BAD_INPUT_F(NULL, "bad input %d", 4) == DCC_ERR_INVALID_ARG &&
        DCC_BAD_INPUT_CB(NULL, "bad input", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_NOT_FOUND(NULL, "missing") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_NOT_FOUND(NULL, "missing") == DCC_ERR_INVALID_ARG &&
        DCC_NOT_FOUND_CB(NULL, "missing", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_BUSY(NULL, "busy") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_BUSY(NULL, "busy") == DCC_ERR_INVALID_ARG &&
        DCC_BUSY_CB(NULL, "busy", NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_INTERNAL_ERROR(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_INTERNAL_ERROR(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_INTERNAL_ERROR_CB(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_DEFER(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_DEFER(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_DEFER_EPHEMERAL(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_DEFER_EPHEMERAL(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP(NULL, DCC_MESSAGE_TEXT("next")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP(NULL, DCC_MESSAGE_TEXT("next")) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_PUBLIC(NULL, "next") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_TEXT(NULL, "next") == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_EPHEMERAL_TEXT(NULL, "next") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_EPHEMERAL(NULL, "next") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT(NULL, DCC_MESSAGE_TEXT("reply")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_TEXT(NULL, "reply") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_PRIVATE(NULL, "reply") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_DONE(NULL, "done") == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP(NULL, DCC_MESSAGE_TEXT("reply")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_TEXT(NULL, "reply") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_EDIT_TEXT(NULL, "reply") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_FOLLOWUP_TEXT(NULL, "reply") == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_V2(NULL, DCC_V2_TEXT("next")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_V2(NULL, DCC_V2_TEXT("next")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_UI(NULL, DCC_UI_TEXT("next")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_V2(NULL, DCC_V2_TEXT("reply")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_PRIVATE_V2(NULL, DCC_V2_TEXT("reply")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_V2(NULL, DCC_V2_TEXT("reply")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_EDIT_UI(NULL, DCC_UI_TEXT("reply")) == DCC_ERR_INVALID_ARG &&
        DCC_RESPOND_OR_FOLLOWUP_UI(NULL, DCC_UI_TEXT("reply")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_RESPOND_OR_EDIT_UI(NULL, DCC_UI_TEXT("reply")) == DCC_ERR_INVALID_ARG &&
        DCC_FOLLOWUP_EPHEMERAL_V2(NULL, DCC_V2_TEXT("next")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_FOLLOWUP_EPHEMERAL_V2(NULL, DCC_V2_TEXT("next")) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATE_MESSAGE(NULL, DCC_MESSAGE_TEXT("updated")) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATE(NULL, DCC_MESSAGE_TEXT("updated")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATE(NULL, DCC_MESSAGE_TEXT("updated")) == DCC_ERR_INVALID_ARG &&
        DCC_UPDATE_V2(NULL, DCC_V2_TEXT("updated")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATE_V2(NULL, DCC_V2_TEXT("updated")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_UPDATE_UI(NULL, DCC_UI_TEXT("updated")) == DCC_ERR_INVALID_ARG &&
        DCC_EDIT_ORIGINAL(NULL, DCC_MESSAGE_TEXT("edited")) == DCC_ERR_INVALID_ARG &&
        DCC_EDIT(NULL, DCC_MESSAGE_TEXT("edited")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EDIT(NULL, DCC_MESSAGE_TEXT("edited")) == DCC_ERR_INVALID_ARG &&
        DCC_DELETE_ORIGINAL(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_DELETE_ORIGINAL(NULL) == DCC_ERR_INVALID_ARG &&
        DCC_SEND(NULL, DCC_MESSAGE_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_SEND_ID(NULL, DCC_MESSAGE_TEXT("send"), dcc_package_consumer_app_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND(NULL, DCC_MESSAGE_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_ID(NULL, DCC_MESSAGE_TEXT("send"), dcc_package_consumer_app_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_TEXT(NULL, "send") == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_TEXT_ID(NULL, "send", dcc_package_consumer_app_message_id_cb, NULL) ==
            DCC_ERR_INVALID_ARG &&
        DCC_SEND_V2(NULL, DCC_V2_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_SEND_V2_ID(NULL, dcc_package_consumer_app_message_id_cb, NULL, DCC_V2_TEXT("send")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_V2(NULL, DCC_V2_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_V2_ID(NULL, dcc_package_consumer_app_message_id_cb, NULL, DCC_V2_TEXT("send")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_UI(NULL, DCC_UI_TEXT("send")) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SEND_UI_ID(NULL, dcc_package_consumer_app_message_id_cb, NULL, DCC_UI_TEXT("send")) ==
            DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL_V2(
            NULL,
            "package.modal.v2",
            "Package modal",
            DCC_V2_LABEL("Name", DCC_MODAL_V2_TEXT_INPUT("name", "Name", 1U))
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SHOW_MODAL(
            NULL,
            DCC_MODAL("package.ctx.modal", "Package context modal", DCC_MODAL_FIELD_TEXT("name", "Name"))
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SHOW_MODAL_V2(
            NULL,
            "package.ctx.modal.v2",
            "Package context modal",
            DCC_V2_LABEL("Name", DCC_MODAL_V2_TEXT_INPUT("name", "Name", 1U))
        ) == DCC_ERR_INVALID_ARG &&
        DCC_SHOW_MODAL_FLOW(NULL, package_modal_flow) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_SHOW_MODAL_FLOW(NULL, package_modal_flow) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_show_modal_flow(NULL, &invalid_package_modal_flow, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_show_modal_flow(NULL, &package_modal_flow, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_AUTOCOMPLETE(
            NULL,
            DCC_AUTOCOMPLETE_CHOICES(DCC_AUTOCOMPLETE_STRING_CHOICE("Package", "package"))
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_AUTOCOMPLETE_MATCHING(
            NULL,
            DCC_AUTOCOMPLETE_STRING_CHOICE("Package", "package")
        ) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_HANDLE_ERROR(NULL, DCC_OK, NULL) == DCC_OK &&
        DCC_CTX_HANDLE_ERROR(NULL, DCC_ERR_STATE, "package") == DCC_ERR_INVALID_ARG &&
        dcc_ctx_handle_error(NULL, DCC_ERR_STATE, "package") == DCC_ERR_INVALID_ARG &&
        dcc_ctx_reply_validation_error(NULL, DCC_ERR_NOT_FOUND, NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_ctx_reply_validation_result(NULL, DCC_ERR_INVALID_ARG, &invalid_error, NULL, NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_ctx_subcommand(NULL) == NULL &&
        dcc_ctx_subcommand_group(NULL) == NULL &&
        dcc_ctx_subcommand_path(NULL, NULL, 0U) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_EVENT_TYPE(NULL) == DCC_EVENT_MAX &&
        DCC_CTX_AUTHOR_ID(NULL) == 0U &&
        DCC_CTX_CHANNEL_ID(NULL) == 0U &&
        DCC_CTX_GUILD(NULL) == NULL &&
        DCC_CTX_CHANNEL(NULL) == NULL &&
        DCC_CTX_USER(NULL) == NULL &&
        DCC_CTX_AUTHOR(NULL) == NULL &&
        DCC_CTX_MEMBER(NULL) == NULL &&
        DCC_CTX_RESOLVED_USER(NULL, 1U) == NULL &&
        DCC_CTX_RESOLVED_MEMBER(NULL, 1U) == NULL &&
        DCC_CTX_RESOLVED_ROLE(NULL, 1U) == NULL &&
        DCC_CTX_RESOLVED_CHANNEL(NULL, 1U) == NULL &&
        DCC_CTX_RESOLVED_MESSAGE(NULL, 1U) == NULL &&
        DCC_CTX_RESOLVED_ATTACHMENT(NULL, 1U) == NULL &&
        DCC_CTX_TARGET_USER(NULL) == NULL &&
        DCC_CTX_TARGET_MEMBER(NULL) == NULL &&
        DCC_CTX_TARGET_ROLE(NULL) == NULL &&
        DCC_CTX_TARGET_CHANNEL(NULL) == NULL &&
        DCC_CTX_TARGET_MESSAGE(NULL) == NULL &&
        DCC_CTX_IS_GUILD(NULL) == 0U &&
        DCC_CTX_IS_DM(NULL) == 0U &&
        strcmp(DCC_CTX_NAME_OR(NULL, "fallback"), "fallback") == 0 &&
        strcmp(DCC_CTX_CUSTOM_ID_OR(NULL, "custom"), "custom") == 0 &&
        DCC_CTX_HAS_CUSTOM_ID(NULL, "custom") == 0U &&
        DCC_CTX_CUSTOM_ID_STARTS_WITH(NULL, "package:") == 0U &&
        DCC_CTX_ACTION(NULL, "package:") == NULL &&
        DCC_CTX_COMPONENT_ACTION(NULL, "package") == NULL &&
        strcmp(DCC_CTX_COMPONENT_ACTION_OR(NULL, "package", "fallback"), "fallback") == 0 &&
        DCC_CTX_COMPONENT_IN_NS(NULL, "package") == 0U &&
        DCC_CTX_COMPONENT_IN_NAMESPACE(NULL, "package") == 0U &&
        DCC_CTX_COMPONENT_HAS_ACTION(NULL, "package", "button") == 0U &&
        DCC_CTX_COMPONENT_IS(NULL, "package", "button") == 0U &&
        DCC_CTX_ACTION_IN_NS(NULL, "package") == NULL &&
        DCC_CTX_ADD_MEMBER_ROLE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REMOVE_MEMBER_ROLE(NULL, 1U, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_ADD_AUTHOR_ROLE(NULL, 2U) == DCC_ERR_INVALID_ARG &&
        DCC_CTX_REMOVE_AUTHOR_ROLE(NULL, 2U) == DCC_ERR_INVALID_ARG &&
        strcmp(DCC_CTX_OPT_STRING(NULL, "name", "fallback"), "fallback") == 0 &&
        DCC_CTX_OPT_INT(NULL, "limit", 25) == 25 &&
        DCC_CTX_OPT_BOOL(NULL, "visible", 1U) == 1U &&
        DCC_CTX_OPT_USER(NULL, "user") == NULL &&
        DCC_CTX_OPT_MEMBER(NULL, "user") == NULL &&
        DCC_CTX_OPT_ROLE(NULL, "role") == NULL &&
        DCC_CTX_OPT_CHANNEL(NULL, "channel") == NULL &&
        DCC_CTX_OPT_MESSAGE(NULL, "message") == NULL &&
        DCC_CTX_OPT_ATTACHMENT(NULL, "attachment") == NULL &&
        DCC_CTX_ARG(NULL, "name") == NULL &&
        DCC_CTX_ARG_PRESENT(NULL, "name") == 0U &&
        DCC_CTX_ARG_STRING(NULL, "name") == NULL &&
        strcmp(DCC_CTX_ARG_STRING_OR(NULL, "name", "fallback"), "fallback") == 0 &&
        DCC_CTX_ARG_INT(NULL, "limit") == 0 &&
        DCC_CTX_ARG_INT_OR(NULL, "limit", 25) == 25 &&
        DCC_CTX_ARG_NUMBER(NULL, "score") == 0.0 &&
        DCC_CTX_ARG_NUMBER_OR(NULL, "score", 1.5) > 1.49 &&
        DCC_CTX_ARG_NUMBER_OR(NULL, "score", 1.5) < 1.51 &&
        DCC_CTX_ARG_BOOL(NULL, "visible") == 0U &&
        DCC_CTX_ARG_BOOL_OR(NULL, "visible", 1U) == 1U &&
        DCC_CTX_ARG_SNOWFLAKE(NULL, "id") == 0U &&
        DCC_CTX_ARG_SNOWFLAKE_OR(NULL, "id", 42U) == 42U &&
        DCC_CTX_ARG_USER(NULL, "user") == NULL &&
        DCC_CTX_ARG_MEMBER(NULL, "user") == NULL &&
        DCC_CTX_ARG_ROLE(NULL, "role") == NULL &&
        DCC_CTX_ARG_CHANNEL(NULL, "channel") == NULL &&
        DCC_CTX_ARG_MESSAGE(NULL, "message") == NULL &&
        DCC_CTX_ARG_ATTACHMENT(NULL, "attachment") == NULL &&
        strcmp(DCC_CTX_ARG_USER_MENTION(NULL, "user"), "") == 0 &&
        DCC_PARAM_STRING(NULL, "name") == NULL &&
        strcmp(DCC_PARAM_STRING_OR(NULL, "name", "fallback"), "fallback") == 0 &&
        DCC_PARAM_INT(NULL, "limit") == 0 &&
        DCC_PARAM_INT_OR(NULL, "limit", 25) == 25 &&
        DCC_PARAM_BOOL(NULL, "visible") == 0U &&
        DCC_PARAM_BOOL_OR(NULL, "visible", 1U) == 1U &&
        DCC_OPTION_STRING(NULL, "name") == NULL &&
        strcmp(DCC_OPTION_STRING_OR(NULL, "name", "fallback"), "fallback") == 0 &&
        DCC_OPTION_INTEGER(NULL, "limit") == 0 &&
        DCC_OPTION_INTEGER_OR(NULL, "limit", 25) == 25 &&
        DCC_OPTION_BOOLEAN(NULL, "visible") == 0U &&
        DCC_OPTION_BOOLEAN_OR(NULL, "visible", 1U) == 1U &&
        DCC_OPTION_NUMBER(NULL, "score") == 0.0 &&
        DCC_OPTION_NUMBER_OR(NULL, "score", 1.5) > 1.49 &&
        DCC_OPTION_NUMBER_OR(NULL, "score", 1.5) < 1.51 &&
        DCC_OPTION_SNOWFLAKE(NULL, "id") == 0U &&
        DCC_OPTION_SNOWFLAKE_OR(NULL, "id", 42U) == 42U &&
        DCC_PARAM_USER(NULL, "user") == NULL &&
        DCC_PARAM_MEMBER(NULL, "user") == NULL &&
        DCC_PARAM_ROLE(NULL, "role") == NULL &&
        DCC_PARAM_CHANNEL(NULL, "channel") == NULL &&
        DCC_OPTION_USER(NULL, "user") == NULL &&
        DCC_OPTION_MEMBER(NULL, "user") == NULL &&
        DCC_OPTION_ROLE(NULL, "role") == NULL &&
        DCC_OPTION_CHANNEL(NULL, "channel") == NULL &&
        DCC_OPTION_MESSAGE(NULL, "message") == NULL &&
        DCC_OPTION_ATTACHMENT(NULL, "attachment") == NULL &&
        DCC_CTX_OPTION(NULL, "name") == NULL &&
        DCC_CTX_OPTION_PRESENT(NULL, "name") == 0U &&
        DCC_CTX_OPTION_TEXT(NULL, "name") == NULL &&
        strcmp(DCC_CTX_OPTION_TEXT_OR(NULL, "name", "fallback"), "fallback") == 0 &&
        DCC_CTX_OPTION_INTEGER(NULL, "limit") == 0 &&
        DCC_CTX_OPTION_INTEGER_OR(NULL, "limit", 25) == 25 &&
        DCC_CTX_OPTION_BOOLEAN(NULL, "visible") == 0U &&
        DCC_CTX_OPTION_BOOLEAN_OR(NULL, "visible", 1U) == 1U &&
        DCC_CTX_OPTION_USER(NULL, "user") == NULL &&
        DCC_CTX_OPTION_MEMBER(NULL, "user") == NULL &&
        DCC_CTX_OPTION_ROLE(NULL, "role") == NULL &&
        DCC_CTX_OPTION_CHANNEL(NULL, "channel") == NULL &&
        DCC_CTX_OPTION_MESSAGE(NULL, "message") == NULL &&
        DCC_CTX_OPTION_ATTACHMENT(NULL, "attachment") == NULL &&
        strcmp(DCC_CTX_OPTION_USER_MENTION(NULL, "user"), "") == 0 &&
        DCC_CTX_FORM_PRESENT(NULL, "name") == 0U &&
        strcmp(DCC_CTX_FORM_STRING_OR(NULL, "name", "fallback"), "fallback") == 0 &&
        strcmp(DCC_CTX_FORM_TEXT(NULL, "name", "fallback"), "fallback") == 0 &&
        strcmp(DCC_CTX_FORM_TEXT_OR(NULL, "name", "fallback"), "fallback") == 0 &&
        DCC_CTX_FORM_TEXT_VALUE(NULL, "name") == NULL &&
        DCC_CTX_FORM_BOOL_OR(NULL, "visible", 1U) == 1U &&
        DCC_CTX_FORM_CHECKED(NULL, "visible", 1U) == 1U &&
        DCC_CTX_FORM_CHECKED_OR(NULL, "visible", 1U) == 1U &&
        DCC_CTX_FORM_IS_CHECKED(NULL, "visible") == 0U &&
        DCC_CTX_FIELD_PRESENT(NULL, "name") == 0U &&
        DCC_CTX_FIELD_TEXT(NULL, "name") == NULL &&
        strcmp(DCC_CTX_FIELD_TEXT_OR(NULL, "name", "fallback"), "fallback") == 0 &&
        DCC_CTX_FIELD_BOOL(NULL, "visible") == 0U &&
        DCC_CTX_FIELD_BOOL_OR(NULL, "visible", 1U) == 1U &&
        DCC_CTX_FIELD_CHECKED(NULL, "visible") == 0U &&
        DCC_CTX_FIELD_CHECKED_OR(NULL, "visible", 1U) == 1U &&
        DCC_FIELD_TEXT(NULL, "name") == NULL &&
        strcmp(DCC_FIELD_TEXT_OR(NULL, "name", "fallback"), "fallback") == 0 &&
        DCC_FIELD_CHECKED(NULL, "visible") == 0U &&
        DCC_FIELD_CHECKED_OR(NULL, "visible", 1U) == 1U &&
        DCC_FIELD_VALUE_COUNT(NULL, "choices") == 0U &&
        DCC_FIELD_VALUE_AT(NULL, "choices", 0U) == NULL &&
        DCC_FIELD_FIRST_VALUE(NULL, "choices") == NULL &&
        strcmp(DCC_FIELD_FIRST_VALUE_OR(NULL, "choices", "fallback"), "fallback") == 0 &&
        DCC_CTX_FIELD_VALUE_COUNT(NULL, "choices") == 0U &&
        DCC_CTX_FIELD_VALUE_AT(NULL, "choices", 0U) == NULL &&
        DCC_CTX_FIELD_FIRST_VALUE(NULL, "choices") == NULL &&
        strcmp(DCC_CTX_FIELD_FIRST_VALUE_OR(NULL, "choices", "fallback"), "fallback") == 0 &&
        DCC_CTX_SELECTED_COUNT(NULL) == 0U &&
        DCC_CTX_SELECTED_VALUE(NULL, 0U) == NULL &&
        DCC_CTX_SELECTED_VALUE_AT(NULL, 0U) == NULL &&
        DCC_CTX_SELECTED_FIRST(NULL) == NULL &&
        strcmp(DCC_CTX_SELECTED_FIRST_OR(NULL, "fallback"), "fallback") == 0 &&
        DCC_CTX_SELECT_COUNT(NULL) == 0U &&
        DCC_CTX_SELECT_VALUE(NULL, 0U) == NULL &&
        DCC_CTX_SELECT_VALUE_AT(NULL, 0U) == NULL &&
        DCC_CTX_SELECT_FIRST(NULL) == NULL &&
        strcmp(DCC_CTX_SELECT_FIRST_OR(NULL, "fallback"), "fallback") == 0 &&
        DCC_SELECTED_COUNT(NULL) == 0U &&
        DCC_SELECTED_VALUE(NULL, 0U) == NULL &&
        DCC_SELECTED_FIRST(NULL) == NULL &&
        strcmp(DCC_SELECTED_FIRST_OR(NULL, "fallback"), "fallback") == 0 &&
        DCC_SELECT_COUNT(NULL) == 0U &&
        DCC_SELECT_VALUE_AT(NULL, 0U) == NULL &&
        DCC_SELECT_FIRST(NULL) == NULL &&
        strcmp(DCC_SELECT_FIRST_OR(NULL, "fallback"), "fallback") == 0 &&
        DCC_APP_ON_READY(NULL, dcc_package_consumer_app_ready_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_EVENT(NULL, DCC_EVENT_READY, dcc_package_consumer_app_event_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE(NULL, dcc_package_consumer_app_message_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_UPDATE(NULL, dcc_package_consumer_app_message_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_DELETE(NULL, dcc_package_consumer_app_message_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_COMMAND(NULL, "!", "ping", dcc_package_consumer_app_message_command_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_PREFIX_COMMAND(NULL, "!", "ping", dcc_package_consumer_app_message_command_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_TEXT_COMMAND(NULL, "!", "ping", dcc_package_consumer_app_message_command_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_PREFIX_COMMAND_FN(NULL, "!", dcc_package_consumer_prefix_command_alias) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_TEXT_COMMAND_FN(NULL, "!", dcc_package_consumer_prefix_command_alias) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SLASH(NULL, "ping", "Ping", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_COMMAND(NULL, "ping", "Ping", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_USER_CONTEXT_MENU(NULL, "Package user", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MESSAGE_CONTEXT_MENU(NULL, "Package message", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_route_apply_policy(NULL, DCC_APP_ROUTE_INVALID, &route_policy) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SLASH_POLICY(NULL, "ping", "Ping", dcc_package_consumer_app_handler, route_policy) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_COMMAND_GUARDED(
            NULL,
            "ping",
            "Ping",
            dcc_package_consumer_app_handler,
            DCC_ONLY_GUILD()
        ) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SUBCOMMAND(NULL, "admin", "reload", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SUBCOMMAND_GUARDED(
            NULL,
            "admin",
            "reload",
            dcc_package_consumer_app_handler,
            DCC_ONLY_GUILD()
        ) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_AUTOCOMPLETE(NULL, "search", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_AUTOCOMPLETE_GUARDED(
            NULL,
            "search",
            dcc_package_consumer_app_handler,
            DCC_RATE_LIMIT_GLOBAL(1U, 1000U)
        ) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON(NULL, "button", dcc_package_consumer_app_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_NS(NULL, "button", "ns", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_POLICY(NULL, "button", dcc_package_consumer_app_handler, route_policy) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_NS_DATA_POLICY(
            NULL,
            "button",
            "ns",
            dcc_package_consumer_app_handler,
            &app_state,
            route_policy
        ) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_GUARDED(
            NULL,
            "button",
            dcc_package_consumer_app_handler,
            DCC_HAS_ROLE(789U)
        ) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_PREFIX(NULL, "button:", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT(NULL, "select", dcc_package_consumer_app_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_NS(NULL, "select", "ns", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_GUARDED(
            NULL,
            "select",
            dcc_package_consumer_app_handler,
            DCC_ONLY_DM()
        ) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_PREFIX(NULL, "select:", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_SELECT_NS_GUARDED(
            NULL,
            "select",
            "ns",
            dcc_package_consumer_app_handler,
            DCC_ONLY_DM()
        ) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL(NULL, "modal", dcc_package_consumer_app_handler) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_NS_PREFIX(NULL, "modal", "ns:", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_PREFIX_DATA_POLICY(
            NULL,
            "modal:",
            dcc_package_consumer_app_handler,
            &app_state,
            route_policy
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_PREFIX_DATA_GUARDED(
            NULL,
            "modal:",
            dcc_package_consumer_app_handler,
            &app_state,
            DCC_ONLY_NSFW()
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_NS_PREFIX_DATA_GUARDED(
            NULL,
            "modal",
            "ns:",
            dcc_package_consumer_app_handler,
            &app_state,
            DCC_ONLY_NSFW()
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_MODAL_PREFIX(NULL, "modal:", dcc_package_consumer_app_handler) ==
            DCC_ERR_INVALID_ARG &&
        DCC_GROUP_BUTTON(NULL, "button", dcc_package_consumer_app_handler) == DCC_ERR_INVALID_ARG &&
        DCC_GROUP_POLICY(NULL, route_policy) == DCC_ERR_INVALID_ARG &&
        dcc_app_wait_for_button(NULL, 1U, &zero_wait_result) == DCC_ERR_INVALID_ARG &&
        zero_wait_result.size == sizeof(zero_wait_result) &&
        dcc_app_wait_for_button(NULL, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        dcc_app_wait_for_component(NULL, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        dcc_app_wait_for_select(NULL, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        dcc_app_wait_for_modal(NULL, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        dcc_app_wait_for_message(NULL, DCC_EVENT_MESSAGE_CREATE, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        DCC_APP_WAIT_FOR_BUTTON(NULL, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        DCC_APP_WAIT_FOR_COMPONENT(NULL, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        DCC_APP_WAIT_FOR_SELECT(NULL, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        DCC_APP_WAIT_FOR_MODAL(NULL, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        DCC_APP_WAIT_FOR_MESSAGE(NULL, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        DCC_APP_WAIT_FOR_MESSAGE_UPDATE(NULL, 1U, &wait_result) == DCC_ERR_INVALID_ARG &&
        DCC_APP_WAIT_FOR_COMPONENT_FILTERED(NULL, &wait_filter, 1U, &wait_result) ==
            DCC_ERR_INVALID_ARG &&
        dcc_app_wait_result_type(NULL) == 0 &&
        dcc_app_wait_result_interaction(NULL) == NULL &&
        dcc_app_wait_result_message(NULL) == NULL &&
        dcc_app_wait_result_id(NULL) == 0U &&
        dcc_app_wait_result_custom_id(NULL) == NULL &&
        dcc_app_wait_result_value_count(NULL) == 0U &&
        dcc_app_wait_result_value(NULL, 0U) == NULL &&
        strcmp(dcc_app_wait_result_message_content(NULL, "fallback"), "fallback") == 0 &&
        dcc_app_wait_result_type(&package_wait_interaction_result) == DCC_EVENT_BUTTON_CLICK &&
        dcc_app_wait_result_interaction(&package_wait_interaction_result) ==
            &package_wait_interaction &&
        dcc_app_wait_result_id(&package_wait_interaction_result) == 111ULL &&
        dcc_app_wait_result_user_id(&package_wait_interaction_result) == 444ULL &&
        dcc_app_wait_result_channel_id(&package_wait_interaction_result) == 333ULL &&
        dcc_app_wait_result_guild_id(&package_wait_interaction_result) == 222ULL &&
        strcmp(dcc_app_wait_result_custom_id(&package_wait_interaction_result),
            "package.button") == 0 &&
        dcc_app_wait_result_value_count(&package_wait_interaction_result) == 2U &&
        strcmp(dcc_app_wait_result_value(&package_wait_interaction_result, 1U), "b") == 0 &&
        dcc_app_wait_result_form_field(&package_wait_interaction_result, "choices") ==
            &package_wait_fields[2] &&
        strcmp(dcc_app_wait_result_form_string(&package_wait_interaction_result, "name", ""),
            "package") == 0 &&
        dcc_app_wait_result_form_bool(&package_wait_interaction_result, "enabled", 0U) == 1U &&
        dcc_app_wait_result_form_value_count(&package_wait_interaction_result, "choices") == 2U &&
        strcmp(dcc_app_wait_result_form_value(&package_wait_interaction_result, "choices", 1U),
            "y") == 0 &&
        DCC_WAIT_ID(&package_wait_message_result) == 555ULL &&
        DCC_WAIT_USER_ID(&package_wait_message_result) == 444ULL &&
        strcmp(DCC_WAIT_MESSAGE_CONTENT(&package_wait_message_result, ""), "package message") == 0 &&
        sync_options.application_id == 123456789ULL &&
        sync_options.command_registry.dry_run == 1U &&
        auto_sync_options.application_id == 0U &&
        auto_sync_options.command_registry.size == 0U &&
        auto_sync_options.infer_application_id == 1U &&
        auto_sync_options.apply == 1U &&
        dcc_app_sync_commands_on_ready(app, &sync_options) == DCC_OK &&
        dcc_app_disable_command_sync_on_ready(app) == DCC_OK &&
        dcc_app_disable_command_sync_on_ready(NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_options_from_env(NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_create_from_env(NULL, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_component_session_options_set_secret(
            &session_options,
            secret,
            sizeof(secret) - 1U
        ) == DCC_OK &&
        dcc_component_session_options_set_ids(&session_options, "pkgapp", "nonce") == DCC_OK &&
        dcc_component_session_options_set_ttl(&session_options, 1000U, 10000U) == DCC_OK &&
        dcc_component_session_create(&session_options, &session) == DCC_OK &&
        dcc_component_session_store_add(&session_store, &session) == DCC_OK &&
        dcc_component_session_store_export_json(
            &session_store,
            &session_store_json,
            &session_store_json_len
        ) == DCC_OK &&
        session_store_json != NULL &&
        session_store_json_len != 0U &&
        dcc_component_session_store_import_json(
            &restored_session_store,
            session_store_json,
            session_store_json_len
        ) == DCC_OK &&
        dcc_component_session_store_count(&restored_session_store) == 1U &&
        dcc_app_component_session(app, &session, NULL, dcc_package_consumer_app_handler, NULL) ==
            DCC_OK;

    dcc_app_wait_result_deinit(&wait_result);
    dcc_app_wait_result_deinit(&zero_wait_result);
    dcc_component_session_store_json_free(session_store_json);
    dcc_component_session_store_deinit(&restored_session_store);
    dcc_component_session_store_deinit(&session_store);
    dcc_component_session_store_deinit(&definition_session_store);
    dcc_component_session_deinit(&definition_session);
    dcc_component_session_deinit(&session);
    dcc_app_destroy(created_defined_app);
    dcc_app_destroy(app);
    return ok && app_state.cleanup_count == 1;
}
