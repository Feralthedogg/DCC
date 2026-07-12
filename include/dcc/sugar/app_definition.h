#ifndef DCC_SUGAR_APP_DEFINITION_H
#define DCC_SUGAR_APP_DEFINITION_H

#include <dcc/app.h>
#include <dcc/sugar/route_policy.h>

#define DCC_APP(name_, ...) \
    ((dcc_app_definition_t){ \
        .size = sizeof(dcc_app_definition_t), \
        .name = (name_), \
        __VA_ARGS__ \
    })

#define DCC_APP_MODULES(...) \
    .modules = (dcc_app_module_t[]){ __VA_ARGS__ }, \
    .module_count = sizeof((dcc_app_module_t[]){ __VA_ARGS__ }) / sizeof(dcc_app_module_t)

#define DCC_APP_FEATURES(...) \
    DCC_APP_MODULES(__VA_ARGS__)

#define DCC_APP_USE(...) \
    DCC_APP_FEATURES(__VA_ARGS__)

#define DCC_APP_EXTENSIONS(...) \
    .extensions = (dcc_app_extension_t[]){ __VA_ARGS__ }, \
    .extension_count = sizeof((dcc_app_extension_t[]){ __VA_ARGS__ }) / sizeof(dcc_app_extension_t)

#define DCC_APP_TYPED_SLASHES(...) \
    .typed_slash_commands = (dcc_app_typed_slash_command_t[]){ __VA_ARGS__ }, \
    .typed_slash_command_count = sizeof((dcc_app_typed_slash_command_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_slash_command_t)

#define DCC_APP_TYPED_COMMANDS(...) \
    DCC_APP_TYPED_SLASHES(__VA_ARGS__)
#define DCC_APP_COMMAND_ROUTES(...) \
    DCC_APP_TYPED_COMMANDS(__VA_ARGS__)

#define DCC_APP_TYPED_SUBCOMMANDS(...) \
    .typed_subcommands = (dcc_app_typed_subcommand_t[]){ __VA_ARGS__ }, \
    .typed_subcommand_count = sizeof((dcc_app_typed_subcommand_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_subcommand_t)
#define DCC_APP_SUBCOMMAND_ROUTES(...) \
    DCC_APP_TYPED_SUBCOMMANDS(__VA_ARGS__)

#define DCC_APP_TYPED_AUTOCOMPLETES(...) \
    .typed_autocompletes = (dcc_app_typed_autocomplete_t[]){ __VA_ARGS__ }, \
    .typed_autocomplete_count = sizeof((dcc_app_typed_autocomplete_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_autocomplete_t)
#define DCC_APP_AUTOCOMPLETE_ROUTES(...) \
    DCC_APP_TYPED_AUTOCOMPLETES(__VA_ARGS__)

#define DCC_APP_TYPED_MODALS(...) \
    .typed_modals = (dcc_app_typed_modal_t[]){ __VA_ARGS__ }, \
    .typed_modal_count = sizeof((dcc_app_typed_modal_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_modal_t)
#define DCC_APP_MODAL_ROUTES(...) \
    DCC_APP_TYPED_MODALS(__VA_ARGS__)

#define DCC_APP_TYPED_BUTTONS(...) \
    .typed_buttons = (dcc_app_typed_component_t[]){ __VA_ARGS__ }, \
    .typed_button_count = sizeof((dcc_app_typed_component_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_component_t)
#define DCC_APP_BUTTON_ROUTES(...) \
    DCC_APP_TYPED_BUTTONS(__VA_ARGS__)

#define DCC_APP_TYPED_SELECTS(...) \
    .typed_selects = (dcc_app_typed_component_t[]){ __VA_ARGS__ }, \
    .typed_select_count = sizeof((dcc_app_typed_component_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_typed_component_t)
#define DCC_APP_SELECT_ROUTES(...) \
    DCC_APP_TYPED_SELECTS(__VA_ARGS__)

#define DCC_APP_MESSAGE_COMMANDS(...) \
    .message_commands = (dcc_app_extension_message_command_t[]){ __VA_ARGS__ }, \
    .message_command_count = sizeof((dcc_app_extension_message_command_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_message_command_t)

#define DCC_APP_EVENTS(...) \
    .events = (dcc_app_extension_event_t[]){ __VA_ARGS__ }, \
    .event_count = sizeof((dcc_app_extension_event_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_event_t)

#define DCC_APP_TASKS(...) \
    .tasks = (dcc_app_extension_task_t[]){ __VA_ARGS__ }, \
    .task_count = sizeof((dcc_app_extension_task_t[]){ __VA_ARGS__ }) / sizeof(dcc_app_extension_task_t)

#define DCC_APP_LISTENERS(...) \
    .listeners = (dcc_app_listener_t[]){ __VA_ARGS__ }, \
    .listener_count = sizeof((dcc_app_listener_t[]){ __VA_ARGS__ }) / sizeof(dcc_app_listener_t)

#define DCC_APP_ROUTES(...) \
    DCC_APP_LISTENERS(__VA_ARGS__)

#define DCC_APP_ROUTE_TABLE(...) \
    DCC_APP_ROUTES(__VA_ARGS__)

#define DCC_APP_VIEWS(...) \
    .views = (dcc_app_view_t[]){ __VA_ARGS__ }, \
    .view_count = sizeof((dcc_app_view_t[]){ __VA_ARGS__ }) / sizeof(dcc_app_view_t)

#define DCC_APP_PERSISTENT_VIEWS(...) \
    DCC_APP_VIEWS(__VA_ARGS__)

#define DCC_COMPONENT_SESSION_ROUTE_OPTIONS(session_, options_, handler_, user_data_) \
    ((dcc_app_component_session_route_t){ \
        .size = sizeof(dcc_app_component_session_route_t), \
        .session = (session_), \
        .options = (options_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_COMPONENT_SESSION_ROUTE(session_, handler_, user_data_) \
    DCC_COMPONENT_SESSION_ROUTE_OPTIONS((session_), NULL, (handler_), (user_data_))

#define DCC_COMPONENT_SESSION_STORE_ROUTE_OPTIONS(store_, options_, handler_, user_data_) \
    ((dcc_app_component_session_route_t){ \
        .size = sizeof(dcc_app_component_session_route_t), \
        .store = (store_), \
        .options = (options_), \
        .handler = (handler_), \
        .user_data = (user_data_) \
    })

#define DCC_COMPONENT_SESSION_STORE_ROUTE(store_, handler_, user_data_) \
    DCC_COMPONENT_SESSION_STORE_ROUTE_OPTIONS((store_), NULL, (handler_), (user_data_))

#define DCC_APP_COMPONENT_SESSIONS(...) \
    .component_sessions = (dcc_app_component_session_route_t[]){ __VA_ARGS__ }, \
    .component_session_count = sizeof((dcc_app_component_session_route_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_component_session_route_t)

#define DCC_ENV(name_) \
    ((dcc_app_env_requirement_t){ \
        .size = sizeof(dcc_app_env_requirement_t), \
        .name = (name_) \
    })

#define DCC_ENV_DESCRIBED(name_, description_) \
    ((dcc_app_env_requirement_t){ \
        .size = sizeof(dcc_app_env_requirement_t), \
        .name = (name_), \
        .description = (description_) \
    })

#define DCC_REQUIRE_ENV(...) \
    .required_env = (dcc_app_env_requirement_t[]){ __VA_ARGS__ }, \
    .required_env_count = sizeof((dcc_app_env_requirement_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_env_requirement_t)

#define DCC_APP_REQUIRE_ENV(...) \
    DCC_REQUIRE_ENV(__VA_ARGS__)

#define DCC_APP_COMMAND_SYNC_ON_READY(options_) \
    .command_sync = &((dcc_app_command_sync_options_t[]){ (options_) })[0]

#define DCC_APP_COMMAND_SYNC_AUTO() \
    ((dcc_app_command_sync_options_t){ \
        .size = sizeof(dcc_app_command_sync_options_t), \
        .apply = 1U, \
        .once = 1U, \
        .infer_application_id = 1U \
    })

#define DCC_APP_COMMAND_SYNC_AUTO_PLAN() \
    ((dcc_app_command_sync_options_t){ \
        .size = sizeof(dcc_app_command_sync_options_t), \
        .once = 1U, \
        .infer_application_id = 1U \
    })

#define DCC_APP_COMMAND_SYNC_GLOBAL_AUTO() \
    ((dcc_app_command_sync_options_t){ \
        .size = sizeof(dcc_app_command_sync_options_t), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL(), \
        .apply = 1U, \
        .once = 1U, \
        .infer_application_id = 1U \
    })

#define DCC_APP_COMMAND_SYNC_GLOBAL_AUTO_PLAN() \
    ((dcc_app_command_sync_options_t){ \
        .size = sizeof(dcc_app_command_sync_options_t), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DRY_RUN(), \
        .once = 1U, \
        .infer_application_id = 1U \
    })

#define DCC_APP_COMMAND_SYNC_GUILD_AUTO(guild_id_) \
    ((dcc_app_command_sync_options_t){ \
        .size = sizeof(dcc_app_command_sync_options_t), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD((guild_id_)), \
        .apply = 1U, \
        .once = 1U, \
        .infer_application_id = 1U \
    })

#define DCC_APP_COMMAND_SYNC_GUILD_AUTO_PLAN(guild_id_) \
    ((dcc_app_command_sync_options_t){ \
        .size = sizeof(dcc_app_command_sync_options_t), \
        .command_registry = DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DRY_RUN((guild_id_)), \
        .once = 1U, \
        .infer_application_id = 1U \
    })

#define DCC_APP_AUTO_SYNC() \
    DCC_APP_COMMAND_SYNC_ON_READY(DCC_APP_COMMAND_SYNC_AUTO())

#define DCC_APP_AUTO_SYNC_PLAN() \
    DCC_APP_COMMAND_SYNC_ON_READY(DCC_APP_COMMAND_SYNC_AUTO_PLAN())

#define DCC_APP_AUTO_SYNC_GLOBAL() \
    DCC_APP_COMMAND_SYNC_ON_READY(DCC_APP_COMMAND_SYNC_GLOBAL_AUTO())

#define DCC_APP_AUTO_SYNC_GLOBAL_PLAN() \
    DCC_APP_COMMAND_SYNC_ON_READY(DCC_APP_COMMAND_SYNC_GLOBAL_AUTO_PLAN())

#define DCC_APP_AUTO_SYNC_GUILD(guild_id_) \
    DCC_APP_COMMAND_SYNC_ON_READY(DCC_APP_COMMAND_SYNC_GUILD_AUTO(guild_id_))

#define DCC_APP_AUTO_SYNC_GUILD_PLAN(guild_id_) \
    DCC_APP_COMMAND_SYNC_ON_READY(DCC_APP_COMMAND_SYNC_GUILD_AUTO_PLAN(guild_id_))

#define DCC_APP_MIDDLEWARE(...) \
    .middlewares = (dcc_app_extension_middleware_t[]){ __VA_ARGS__ }, \
    .middleware_count = sizeof((dcc_app_extension_middleware_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_middleware_t)

#define DCC_APP_GUILD_ONLY() \
    .guild_only = 1U

#define DCC_APP_DM_ONLY() \
    .dm_only = 1U

#define DCC_APP_NSFW_ONLY() \
    .nsfw_only = 1U

#define DCC_APP_CHANNEL_TYPES(...) \
    .channel_types = (uint32_t[]){ __VA_ARGS__ }, \
    .channel_type_count = sizeof((uint32_t[]){ __VA_ARGS__ }) / sizeof(uint32_t)

#define DCC_APP_OWNER(user_id_) \
    .owner_user_ids = (dcc_snowflake_t[]){ (user_id_) }, \
    .owner_user_id_count = 1U

#define DCC_APP_OWNERS(...) \
    .owner_user_ids = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
    .owner_user_id_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t)

#define DCC_APP_REQUIRED_PERMISSIONS(permissions_) \
    .required_permissions = (permissions_)

#define DCC_APP_REQUIRE_ROLE(role_id_) \
    .required_role_ids = (dcc_snowflake_t[]){ (role_id_) }, \
    .required_role_id_count = 1U

#define DCC_APP_REQUIRE_ROLES(...) \
    .required_role_ids = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
    .required_role_id_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t)

#define DCC_APP_REQUIRE_ANY_ROLE(...) \
    .any_role_ids = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
    .any_role_id_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t)

#define DCC_APP_CHECKS(...) \
    .checks = (dcc_app_check_t[]){ __VA_ARGS__ }, \
    .check_count = sizeof((dcc_app_check_t[]){ __VA_ARGS__ }) / sizeof(dcc_app_check_t)

#define DCC_APP_CHECK(check_, user_data_) \
    DCC_APP_CHECKS(DCC_CHECK((check_), (user_data_)))

#define DCC_APP_CHECK_MESSAGE(check_, user_data_, title_, description_) \
    DCC_APP_CHECKS(DCC_CHECK_MESSAGE((check_), (user_data_), (title_), (description_)))

#define DCC_APP_COOLDOWN(options_) \
    .cooldown = (options_)

#define DCC_APP_COOLDOWN_GLOBAL(limit_, window_ms_) \
    DCC_APP_COOLDOWN(DCC_COOLDOWN_GLOBAL((limit_), (window_ms_)))

#define DCC_APP_COOLDOWN_USER(limit_, window_ms_) \
    DCC_APP_COOLDOWN(DCC_COOLDOWN_USER((limit_), (window_ms_)))

#define DCC_APP_COOLDOWN_CHANNEL(limit_, window_ms_) \
    DCC_APP_COOLDOWN(DCC_COOLDOWN_CHANNEL((limit_), (window_ms_)))

#define DCC_APP_COOLDOWN_GUILD(limit_, window_ms_) \
    DCC_APP_COOLDOWN(DCC_COOLDOWN_GUILD((limit_), (window_ms_)))

#define DCC_APP_AUTO_DEFER_MS(after_ms_) \
    .auto_defer_after_ms = (after_ms_)

#define DCC_APP_AUTO_DEFER(after_ms_) \
    DCC_APP_AUTO_DEFER_MS((after_ms_))

#define DCC_APP_AUTO_DEFER_DEFAULT() \
    DCC_APP_AUTO_DEFER_MS(1500U)

#define DCC_APP_AUTO_DEFER_EPHEMERAL_MS(after_ms_) \
    .auto_defer_after_ms = (after_ms_), \
    .auto_defer_ephemeral = 1U

#define DCC_APP_AUTO_DEFER_EPHEMERAL(after_ms_) \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS((after_ms_))

#define DCC_APP_AUTO_DEFER_PRIVATE(after_ms_) \
    DCC_APP_AUTO_DEFER_EPHEMERAL((after_ms_))

#define DCC_APP_AUTO_DEFER_EPHEMERAL_DEFAULT() \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS(1500U)

#define DCC_APP_AUTO_DEFER_PRIVATE_DEFAULT() \
    DCC_APP_AUTO_DEFER_EPHEMERAL_DEFAULT()

#define DCC_APP_DEFAULT_ERRORS() \
    .use_default_error_responses = 1U

#define DCC_APP_ERROR_HANDLER(handler_, user_data_) \
    .error_handler = (handler_), \
    .error_user_data = (user_data_)

#define DCC_APP_ERRORS(handler_, user_data_) \
    DCC_APP_ERROR_HANDLER((handler_), (user_data_))

#define DCC_APP_DEV_MODE() \
    DCC_APP_AUTO_SYNC(), \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS(1500U), \
    DCC_APP_DEFAULT_ERRORS()

#define DCC_APP_DEV_MODE_MS(after_ms_) \
    DCC_APP_AUTO_SYNC(), \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS((after_ms_)), \
    DCC_APP_DEFAULT_ERRORS()

#define DCC_APP_DEV_MODE_PLAN() \
    DCC_APP_AUTO_SYNC_PLAN(), \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS(1500U), \
    DCC_APP_DEFAULT_ERRORS()

#define DCC_APP_DEV_MODE_PLAN_MS(after_ms_) \
    DCC_APP_AUTO_SYNC_PLAN(), \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS((after_ms_)), \
    DCC_APP_DEFAULT_ERRORS()

#define DCC_APP_PRODUCTION_MODE() \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS(1500U), \
    DCC_APP_DEFAULT_ERRORS()

#define DCC_APP_PRODUCTION_MODE_MS(after_ms_) \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS((after_ms_)), \
    DCC_APP_DEFAULT_ERRORS()

#define DCC_APP_PRESET_BOT() \
    DCC_APP_PRODUCTION_MODE()

#define DCC_APP_PRESET_BOT_MS(after_ms_) \
    DCC_APP_PRODUCTION_MODE_MS((after_ms_))

#define DCC_APP_PRESET_DEV_GUILD(guild_id_) \
    DCC_APP_AUTO_SYNC_GUILD((guild_id_)), \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS(1500U), \
    DCC_APP_DEFAULT_ERRORS()

#define DCC_APP_PRESET_DEV_GUILD_MS(guild_id_, after_ms_) \
    DCC_APP_AUTO_SYNC_GUILD((guild_id_)), \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS((after_ms_)), \
    DCC_APP_DEFAULT_ERRORS()

#define DCC_APP_PRESET_DEV_GUILD_PLAN(guild_id_) \
    DCC_APP_AUTO_SYNC_GUILD_PLAN((guild_id_)), \
    DCC_APP_AUTO_DEFER_EPHEMERAL_MS(1500U), \
    DCC_APP_DEFAULT_ERRORS()

#define DCC_DEV_APP(name_, ...) \
    DCC_APP((name_), DCC_APP_DEV_MODE(), __VA_ARGS__)

#define DCC_DEV_APP_MS(name_, after_ms_, ...) \
    DCC_APP((name_), DCC_APP_DEV_MODE_MS((after_ms_)), __VA_ARGS__)

#define DCC_PROD_APP(name_, ...) \
    DCC_APP((name_), DCC_APP_PRODUCTION_MODE(), __VA_ARGS__)

#define DCC_PROD_APP_MS(name_, after_ms_, ...) \
    DCC_APP((name_), DCC_APP_PRODUCTION_MODE_MS((after_ms_)), __VA_ARGS__)

#define DCC_DEV_PLAN_APP(name_, ...) \
    DCC_APP((name_), DCC_APP_DEV_MODE_PLAN(), __VA_ARGS__)

#define DCC_DEV_PLAN_APP_MS(name_, after_ms_, ...) \
    DCC_APP((name_), DCC_APP_DEV_MODE_PLAN_MS((after_ms_)), __VA_ARGS__)

#define DCC_DEV_GUILD_APP(name_, guild_id_, ...) \
    DCC_APP((name_), DCC_APP_PRESET_DEV_GUILD((guild_id_)), __VA_ARGS__)

#define DCC_DEV_GUILD_APP_MS(name_, guild_id_, after_ms_, ...) \
    DCC_APP((name_), DCC_APP_PRESET_DEV_GUILD_MS((guild_id_), (after_ms_)), __VA_ARGS__)

#define DCC_DEV_GUILD_PLAN_APP(name_, guild_id_, ...) \
    DCC_APP((name_), DCC_APP_PRESET_DEV_GUILD_PLAN((guild_id_)), __VA_ARGS__)

#define DCC_BOT(name_, ...) \
    DCC_PROD_APP((name_), __VA_ARGS__)

#define DCC_BOT_APP(name_, ...) \
    DCC_PROD_APP((name_), __VA_ARGS__)

#define DCC_DEV_BOT(name_, ...) \
    DCC_DEV_APP((name_), __VA_ARGS__)

#define DCC_DEV_BOT_APP(name_, ...) \
    DCC_DEV_APP((name_), __VA_ARGS__)

#define DCC_GUILD_BOT(name_, guild_id_, ...) \
    DCC_PROD_APP((name_), __VA_ARGS__)

#define DCC_DEV_GUILD_BOT(name_, guild_id_, ...) \
    DCC_DEV_GUILD_APP((name_), (guild_id_), __VA_ARGS__)

#define DCC_DEV_GUILD_BOT_APP(name_, guild_id_, ...) \
    DCC_DEV_GUILD_APP((name_), (guild_id_), __VA_ARGS__)

#define DCC_BOT_GUILD(name_, guild_id_, ...) \
    DCC_GUILD_BOT((name_), (guild_id_), __VA_ARGS__)

#define DCC_BOT_FEATURES(name_, ...) \
    DCC_BOT((name_), DCC_APP_FEATURES(__VA_ARGS__))

#define DCC_BOT_USE(name_, ...) \
    DCC_BOT_FEATURES((name_), __VA_ARGS__)

#define DCC_GUILD_BOT_FEATURES(name_, guild_id_, ...) \
    DCC_GUILD_BOT((name_), (guild_id_), DCC_APP_FEATURES(__VA_ARGS__))

#define DCC_GUILD_BOT_USE(name_, guild_id_, ...) \
    DCC_GUILD_BOT_FEATURES((name_), (guild_id_), __VA_ARGS__)

#define DCC_SIMPLE_BOT(name_, ...) \
    DCC_BOT((name_), DCC_APP_LISTENERS(__VA_ARGS__))

#define DCC_BOT_LISTENERS(name_, ...) \
    DCC_SIMPLE_BOT((name_), __VA_ARGS__)

#define DCC_BOT_ROUTES(name_, ...) \
    DCC_BOT((name_), DCC_APP_ROUTES(__VA_ARGS__))

#define DCC_APP_ROUTES_BOT(name_, ...) \
    DCC_BOT_ROUTES((name_), __VA_ARGS__)

#define DCC_SIMPLE_GUILD_BOT(name_, guild_id_, ...) \
    DCC_GUILD_BOT((name_), (guild_id_), DCC_APP_LISTENERS(__VA_ARGS__))

#define DCC_GUILD_BOT_LISTENERS(name_, guild_id_, ...) \
    DCC_SIMPLE_GUILD_BOT((name_), (guild_id_), __VA_ARGS__)

#define DCC_GUILD_BOT_ROUTES(name_, guild_id_, ...) \
    DCC_GUILD_BOT((name_), (guild_id_), DCC_APP_ROUTES(__VA_ARGS__))

#define DCC_GUILD_APP_ROUTES_BOT(name_, guild_id_, ...) \
    DCC_GUILD_BOT_ROUTES((name_), (guild_id_), __VA_ARGS__)

#define DCC_RUN_APP(definition_) \
    dcc_app_run_dotenv_defined_with_signals(NULL, &((dcc_app_definition_t[]){ (definition_) })[0])

#define DCC_RUN_APP_TOKEN(token_env_, definition_) \
    dcc_app_run_dotenv_defined_with_signals((token_env_), &((dcc_app_definition_t[]){ (definition_) })[0])

#define DCC_RUN_APP_ENV(token_env_, definition_) \
    dcc_app_run_from_env_defined_with_signals((token_env_), &((dcc_app_definition_t[]){ (definition_) })[0])

#define DCC_RUN_APP_OPTIONS(options_, definition_) \
    dcc_app_run_defined_with_signals( \
        &((dcc_app_options_t[]){ (options_) })[0], \
        &((dcc_app_definition_t[]){ (definition_) })[0] \
    )

#define DCC_STATUS_EXIT_CODE(status_) \
    ((status_) == DCC_OK ? 0 : 1)

#define DCC_MAIN(definition_) \
    DCC_APP_MAIN(definition_)

#define DCC_APP_MAIN(definition_) \
    int main(void) { \
        dcc_status_t dcc_main_status = DCC_RUN_APP(definition_); \
        return DCC_STATUS_EXIT_CODE(dcc_main_status); \
    }

#define DCC_APP_MAIN_TOKEN(token_env_, definition_) \
    int main(void) { \
        dcc_status_t dcc_main_status = DCC_RUN_APP_TOKEN((token_env_), (definition_)); \
        return DCC_STATUS_EXIT_CODE(dcc_main_status); \
    }

#define DCC_APP_MAIN_ENV(token_env_, definition_) \
    int main(void) { \
        dcc_status_t dcc_main_status = DCC_RUN_APP_ENV((token_env_), (definition_)); \
        return DCC_STATUS_EXIT_CODE(dcc_main_status); \
    }

#define DCC_APP_MAIN_OPTIONS(options_, definition_) \
    int main(void) { \
        dcc_status_t dcc_main_status = DCC_RUN_APP_OPTIONS((options_), (definition_)); \
        return DCC_STATUS_EXIT_CODE(dcc_main_status); \
    }

#define DCC_RUN_DEV_APP(name_, ...) \
    DCC_RUN_APP(DCC_DEV_APP((name_), __VA_ARGS__))

#define DCC_RUN_DEV_BOT(name_, ...) \
    DCC_RUN_APP(DCC_DEV_BOT((name_), __VA_ARGS__))

#define DCC_RUN_DEV_BOT_TOKEN(token_env_, name_, ...) \
    DCC_RUN_APP_TOKEN((token_env_), DCC_DEV_BOT((name_), __VA_ARGS__))

#define DCC_RUN_DEV_BOT_ENV(token_env_, name_, ...) \
    DCC_RUN_APP_ENV((token_env_), DCC_DEV_BOT((name_), __VA_ARGS__))

#define DCC_RUN_DEV_GUILD_BOT(name_, guild_id_, ...) \
    DCC_RUN_APP(DCC_DEV_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_DEV_GUILD_BOT_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_RUN_APP_TOKEN((token_env_), DCC_DEV_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_DEV_GUILD_BOT_ENV(token_env_, name_, guild_id_, ...) \
    DCC_RUN_APP_ENV((token_env_), DCC_DEV_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_BOT(name_, ...) \
    DCC_RUN_APP(DCC_BOT((name_), __VA_ARGS__))

#define DCC_RUN_BOT_TOKEN(token_env_, name_, ...) \
    DCC_RUN_APP_TOKEN((token_env_), DCC_BOT((name_), __VA_ARGS__))

#define DCC_RUN_BOT_ENV(token_env_, name_, ...) \
    DCC_RUN_APP_ENV((token_env_), DCC_BOT((name_), __VA_ARGS__))

#define DCC_RUN_BOT_FEATURES(name_, ...) \
    DCC_RUN_APP(DCC_BOT_FEATURES((name_), __VA_ARGS__))

#define DCC_RUN_BOT_FEATURES_TOKEN(token_env_, name_, ...) \
    DCC_RUN_APP_TOKEN((token_env_), DCC_BOT_FEATURES((name_), __VA_ARGS__))

#define DCC_RUN_BOT_FEATURES_ENV(token_env_, name_, ...) \
    DCC_RUN_APP_ENV((token_env_), DCC_BOT_FEATURES((name_), __VA_ARGS__))

#define DCC_RUN_GUILD_BOT(name_, guild_id_, ...) \
    DCC_RUN_APP(DCC_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_GUILD_BOT_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_RUN_APP_TOKEN((token_env_), DCC_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_GUILD_BOT_ENV(token_env_, name_, guild_id_, ...) \
    DCC_RUN_APP_ENV((token_env_), DCC_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_GUILD_BOT_FEATURES(name_, guild_id_, ...) \
    DCC_RUN_APP(DCC_GUILD_BOT_FEATURES((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_GUILD_BOT_FEATURES_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_RUN_APP_TOKEN((token_env_), DCC_GUILD_BOT_FEATURES((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_GUILD_BOT_FEATURES_ENV(token_env_, name_, guild_id_, ...) \
    DCC_RUN_APP_ENV((token_env_), DCC_GUILD_BOT_FEATURES((name_), (guild_id_), __VA_ARGS__))

#define DCC_DEV_APP_MAIN(name_, ...) \
    DCC_APP_MAIN(DCC_DEV_APP((name_), __VA_ARGS__))

#define DCC_DEV_BOT_MAIN(name_, ...) \
    DCC_APP_MAIN(DCC_DEV_BOT((name_), __VA_ARGS__))

#define DCC_DEV_BOT_MAIN_TOKEN(token_env_, name_, ...) \
    DCC_APP_MAIN_TOKEN((token_env_), DCC_DEV_BOT((name_), __VA_ARGS__))

#define DCC_DEV_BOT_MAIN_ENV(token_env_, name_, ...) \
    DCC_APP_MAIN_ENV((token_env_), DCC_DEV_BOT((name_), __VA_ARGS__))

#define DCC_DEV_GUILD_BOT_MAIN(name_, guild_id_, ...) \
    DCC_APP_MAIN(DCC_DEV_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_DEV_GUILD_BOT_MAIN_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_APP_MAIN_TOKEN((token_env_), DCC_DEV_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_DEV_GUILD_BOT_MAIN_ENV(token_env_, name_, guild_id_, ...) \
    DCC_APP_MAIN_ENV((token_env_), DCC_DEV_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_BOT_MAIN(name_, ...) \
    DCC_APP_MAIN(DCC_BOT((name_), __VA_ARGS__))

#define DCC_BOT_MAIN_TOKEN(token_env_, name_, ...) \
    DCC_APP_MAIN_TOKEN((token_env_), DCC_BOT((name_), __VA_ARGS__))

#define DCC_BOT_MAIN_ENV(token_env_, name_, ...) \
    DCC_APP_MAIN_ENV((token_env_), DCC_BOT((name_), __VA_ARGS__))

#define DCC_BOT_FEATURES_MAIN(name_, ...) \
    DCC_APP_MAIN(DCC_BOT_FEATURES((name_), __VA_ARGS__))

#define DCC_BOT_FEATURES_MAIN_TOKEN(token_env_, name_, ...) \
    DCC_APP_MAIN_TOKEN((token_env_), DCC_BOT_FEATURES((name_), __VA_ARGS__))

#define DCC_BOT_FEATURES_MAIN_ENV(token_env_, name_, ...) \
    DCC_APP_MAIN_ENV((token_env_), DCC_BOT_FEATURES((name_), __VA_ARGS__))

#define DCC_GUILD_BOT_MAIN(name_, guild_id_, ...) \
    DCC_APP_MAIN(DCC_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_GUILD_BOT_MAIN_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_APP_MAIN_TOKEN((token_env_), DCC_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_GUILD_BOT_MAIN_ENV(token_env_, name_, guild_id_, ...) \
    DCC_APP_MAIN_ENV((token_env_), DCC_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_GUILD_BOT_FEATURES_MAIN(name_, guild_id_, ...) \
    DCC_APP_MAIN(DCC_GUILD_BOT_FEATURES((name_), (guild_id_), __VA_ARGS__))

#define DCC_GUILD_BOT_FEATURES_MAIN_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_APP_MAIN_TOKEN((token_env_), DCC_GUILD_BOT_FEATURES((name_), (guild_id_), __VA_ARGS__))

#define DCC_GUILD_BOT_FEATURES_MAIN_ENV(token_env_, name_, guild_id_, ...) \
    DCC_APP_MAIN_ENV((token_env_), DCC_GUILD_BOT_FEATURES((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_SIMPLE_BOT(name_, ...) \
    DCC_RUN_APP(DCC_SIMPLE_BOT((name_), __VA_ARGS__))

#define DCC_RUN_SIMPLE_BOT_TOKEN(token_env_, name_, ...) \
    DCC_RUN_APP_TOKEN((token_env_), DCC_SIMPLE_BOT((name_), __VA_ARGS__))

#define DCC_RUN_SIMPLE_BOT_ENV(token_env_, name_, ...) \
    DCC_RUN_APP_ENV((token_env_), DCC_SIMPLE_BOT((name_), __VA_ARGS__))

#define DCC_RUN_BOT_LISTENERS(name_, ...) \
    DCC_RUN_SIMPLE_BOT((name_), __VA_ARGS__)

#define DCC_RUN_BOT_ROUTES(name_, ...) \
    DCC_RUN_APP(DCC_BOT_ROUTES((name_), __VA_ARGS__))

#define DCC_RUN_BOT_LISTENERS_TOKEN(token_env_, name_, ...) \
    DCC_RUN_SIMPLE_BOT_TOKEN((token_env_), (name_), __VA_ARGS__)

#define DCC_RUN_BOT_ROUTES_TOKEN(token_env_, name_, ...) \
    DCC_RUN_APP_TOKEN((token_env_), DCC_BOT_ROUTES((name_), __VA_ARGS__))

#define DCC_RUN_BOT_LISTENERS_ENV(token_env_, name_, ...) \
    DCC_RUN_SIMPLE_BOT_ENV((token_env_), (name_), __VA_ARGS__)

#define DCC_RUN_BOT_ROUTES_ENV(token_env_, name_, ...) \
    DCC_RUN_APP_ENV((token_env_), DCC_BOT_ROUTES((name_), __VA_ARGS__))

#define DCC_SIMPLE_BOT_MAIN(name_, ...) \
    DCC_APP_MAIN(DCC_SIMPLE_BOT((name_), __VA_ARGS__))

#define DCC_SIMPLE_BOT_MAIN_TOKEN(token_env_, name_, ...) \
    DCC_APP_MAIN_TOKEN((token_env_), DCC_SIMPLE_BOT((name_), __VA_ARGS__))

#define DCC_SIMPLE_BOT_MAIN_ENV(token_env_, name_, ...) \
    DCC_APP_MAIN_ENV((token_env_), DCC_SIMPLE_BOT((name_), __VA_ARGS__))

#define DCC_BOT_LISTENERS_MAIN(name_, ...) \
    DCC_SIMPLE_BOT_MAIN((name_), __VA_ARGS__)

#define DCC_BOT_ROUTES_MAIN(name_, ...) \
    DCC_APP_MAIN(DCC_BOT_ROUTES((name_), __VA_ARGS__))

#define DCC_BOT_LISTENERS_MAIN_TOKEN(token_env_, name_, ...) \
    DCC_SIMPLE_BOT_MAIN_TOKEN((token_env_), (name_), __VA_ARGS__)

#define DCC_BOT_ROUTES_MAIN_TOKEN(token_env_, name_, ...) \
    DCC_APP_MAIN_TOKEN((token_env_), DCC_BOT_ROUTES((name_), __VA_ARGS__))

#define DCC_BOT_LISTENERS_MAIN_ENV(token_env_, name_, ...) \
    DCC_SIMPLE_BOT_MAIN_ENV((token_env_), (name_), __VA_ARGS__)

#define DCC_BOT_ROUTES_MAIN_ENV(token_env_, name_, ...) \
    DCC_APP_MAIN_ENV((token_env_), DCC_BOT_ROUTES((name_), __VA_ARGS__))

#define DCC_RUN_SIMPLE_GUILD_BOT(name_, guild_id_, ...) \
    DCC_RUN_APP(DCC_SIMPLE_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_SIMPLE_GUILD_BOT_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_RUN_APP_TOKEN((token_env_), DCC_SIMPLE_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_SIMPLE_GUILD_BOT_ENV(token_env_, name_, guild_id_, ...) \
    DCC_RUN_APP_ENV((token_env_), DCC_SIMPLE_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_GUILD_BOT_LISTENERS(name_, guild_id_, ...) \
    DCC_RUN_SIMPLE_GUILD_BOT((name_), (guild_id_), __VA_ARGS__)

#define DCC_RUN_GUILD_BOT_ROUTES(name_, guild_id_, ...) \
    DCC_RUN_APP(DCC_GUILD_BOT_ROUTES((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_GUILD_BOT_LISTENERS_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_RUN_SIMPLE_GUILD_BOT_TOKEN((token_env_), (name_), (guild_id_), __VA_ARGS__)

#define DCC_RUN_GUILD_BOT_ROUTES_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_RUN_APP_TOKEN((token_env_), DCC_GUILD_BOT_ROUTES((name_), (guild_id_), __VA_ARGS__))

#define DCC_RUN_GUILD_BOT_LISTENERS_ENV(token_env_, name_, guild_id_, ...) \
    DCC_RUN_SIMPLE_GUILD_BOT_ENV((token_env_), (name_), (guild_id_), __VA_ARGS__)

#define DCC_RUN_GUILD_BOT_ROUTES_ENV(token_env_, name_, guild_id_, ...) \
    DCC_RUN_APP_ENV((token_env_), DCC_GUILD_BOT_ROUTES((name_), (guild_id_), __VA_ARGS__))

#define DCC_SIMPLE_GUILD_BOT_MAIN(name_, guild_id_, ...) \
    DCC_APP_MAIN(DCC_SIMPLE_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_SIMPLE_GUILD_BOT_MAIN_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_APP_MAIN_TOKEN((token_env_), DCC_SIMPLE_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_SIMPLE_GUILD_BOT_MAIN_ENV(token_env_, name_, guild_id_, ...) \
    DCC_APP_MAIN_ENV((token_env_), DCC_SIMPLE_GUILD_BOT((name_), (guild_id_), __VA_ARGS__))

#define DCC_GUILD_BOT_LISTENERS_MAIN(name_, guild_id_, ...) \
    DCC_SIMPLE_GUILD_BOT_MAIN((name_), (guild_id_), __VA_ARGS__)

#define DCC_GUILD_BOT_ROUTES_MAIN(name_, guild_id_, ...) \
    DCC_APP_MAIN(DCC_GUILD_BOT_ROUTES((name_), (guild_id_), __VA_ARGS__))

#define DCC_GUILD_BOT_LISTENERS_MAIN_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_SIMPLE_GUILD_BOT_MAIN_TOKEN((token_env_), (name_), (guild_id_), __VA_ARGS__)

#define DCC_GUILD_BOT_ROUTES_MAIN_TOKEN(token_env_, name_, guild_id_, ...) \
    DCC_APP_MAIN_TOKEN((token_env_), DCC_GUILD_BOT_ROUTES((name_), (guild_id_), __VA_ARGS__))

#define DCC_GUILD_BOT_LISTENERS_MAIN_ENV(token_env_, name_, guild_id_, ...) \
    DCC_SIMPLE_GUILD_BOT_MAIN_ENV((token_env_), (name_), (guild_id_), __VA_ARGS__)

#define DCC_GUILD_BOT_ROUTES_MAIN_ENV(token_env_, name_, guild_id_, ...) \
    DCC_APP_MAIN_ENV((token_env_), DCC_GUILD_BOT_ROUTES((name_), (guild_id_), __VA_ARGS__))

#endif
