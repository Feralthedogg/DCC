#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

static int listener_state;

typedef struct listener_echo_args {
    const char *text;
} listener_echo_args_t;

typedef struct listener_modal_args {
    const char *name;
    uint8_t visible;
} listener_modal_args_t;

typedef struct listener_component_args {
    const char *action;
    const char *value;
} listener_component_args_t;

#define LISTENER_ECHO_PARAMS(X, T) \
    X(T, REQUIRED_STRING, text, "text", "Text to echo")

#define LISTENER_MODAL_PARAMS(X, T) \
    X(T, REQUIRED_STRING, name, "name") \
    X(T, BOOL, visible, "visible", 1U)

#define LISTENER_COMPONENT_PARAMS(X, T) \
    X(T, REQUIRED_CUSTOM_ID_SUFFIX, action, "listener.") \
    X(T, FIRST_VALUE, value, "")

static void listener_slash(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
}

static void listener_button(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
}

static void listener_select(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    (void)user_data;
}

static void listener_ready(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data) {
    (void)app;
    (void)ready;
    (void)user_data;
}

static void listener_message_command(
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

static void listener_task(dcc_app_t *app, void *user_data) {
    (void)app;
    (void)user_data;
}

static void listener_event(dcc_app_t *app, const dcc_event_t *event, void *user_data) {
    (void)app;
    (void)event;
    (void)user_data;
}

DCC_MEMBER_JOIN_FN(listener_member_join) {
    (void)app;
    (void)member;
    (void)event;
    (void)user_data;
}

DCC_DECORATE_SLASH(listener_decorated_ping, "Decorated pong") {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_BUTTON_NS(listener_decorated_refresh, "status", "decorated-refresh") {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_READY_ONCE_DATA(listener_decorated_ready, &listener_state) {
    (void)app;
    (void)ready;
    (void)user_data;
}

DCC_DECORATE_MESSAGE_COMMAND_DATA(listener_decorated_text, "!", "decorated", &listener_state) {
    (void)app;
    (void)message;
    (void)args;
    (void)event;
    (void)user_data;
}

DCC_DECORATE_TASK_DAILY_KST_DATA(listener_decorated_task, "10:15", &listener_state) {
    (void)app;
    (void)user_data;
}

DCC_DECORATE_SLASH_PARAMS_DATA_GUARDED(
    listener_echo,
    "Echo text",
    listener_echo_args_t,
    args,
    &listener_state,
    LISTENER_ECHO_PARAMS,
    DCC_ROUTE_GUILD_ONLY()
) {
    (void)ctx;
    (void)args;
    (void)user_data;
}

DCC_TYPED_HANDLER(listener_typed_component) {
    (void)ctx;
    (void)args;
    (void)user_data;
}

DCC_DEFINE_LISTENERS(
    listener_feature,
    DCC_LISTEN_SLASH_DATA("ping", "Reply with pong", listener_slash, &listener_state),
    DCC_LISTEN_BUTTON_NS_DATA("status", "refresh", listener_button, &listener_state),
    DCC_LISTEN_SELECT_NS("roles", "choose", listener_select),
    DCC_LISTEN_READY_ONCE_DATA(listener_ready, &listener_state),
    DCC_LISTEN_PREFIX_COMMAND_DATA("!", "ping", listener_message_command, &listener_state),
    DCC_LISTEN_TASK_DAILY_KST_DATA("09:00", listener_task, &listener_state),
    DCC_LISTEN_TYPED_SLASH(DCC_TYPED_ROUTE(listener_echo))
)

DCC_DECLARE_FEATURE(feature_alias);
DCC_DEFINE_PUBLIC_FEATURE(
    feature_alias,
    "feature_alias",
    DCC_FEATURE_COMMANDS(
        DCC_ON_SLASH_DATA("feature", "Feature alias command", listener_slash, &listener_state)
    )
)

static int check_listener_descriptors(void) {
    dcc_sugar_listener_list_t listeners =
        DCC_LISTENERS(
            DCC_LISTEN_SLASH_DATA("ping", "Reply with pong", listener_slash, &listener_state),
            DCC_LISTEN_BUTTON_NS_DATA("status", "refresh", listener_button, &listener_state),
            DCC_LISTEN_READY_ONCE_DATA(listener_ready, &listener_state),
            DCC_LISTEN_MEMBER_JOIN_DATA(listener_member_join, &listener_state),
            DCC_LISTEN_PREFIX_COMMAND_DATA("!", "ping", listener_message_command, &listener_state),
            DCC_LISTEN_TASK_DAILY_KST_DATA("09:00", listener_task, &listener_state),
            DCC_LISTEN_TYPED_SLASH(DCC_TYPED_ROUTE(listener_echo))
        );

    if (listeners.count != 7U || listeners.items == NULL) {
        fprintf(stderr, "DCC_LISTENERS count mismatch\n");
        return 1;
    }
    if (listeners.items[0].kind != DCC_SUGAR_LISTENER_SLASH ||
        strcmp(listeners.items[0].slash.name, "ping") != 0 ||
        strcmp(listeners.items[0].slash.description, "Reply with pong") != 0 ||
        listeners.items[0].slash.handler != listener_slash ||
        listeners.items[0].slash.user_data != &listener_state) {
        fprintf(stderr, "DCC_LISTEN_SLASH_DATA descriptor mismatch\n");
        return 1;
    }
    if (listeners.items[1].kind != DCC_SUGAR_LISTENER_COMPONENT ||
        listeners.items[1].component.type != DCC_APP_EXTENSION_BUTTON ||
        strcmp(listeners.items[1].component.custom_id, "status.refresh") != 0 ||
        listeners.items[1].component.handler != listener_button) {
        fprintf(stderr, "DCC_LISTEN_BUTTON_NS_DATA descriptor mismatch\n");
        return 1;
    }
    if (listeners.items[2].kind != DCC_SUGAR_LISTENER_EVENT ||
        listeners.items[2].event.type != DCC_APP_EXTENSION_READY ||
        listeners.items[2].event.once != 1U ||
        listeners.items[2].event.ready_handler != listener_ready ||
        listeners.items[2].event.user_data != &listener_state) {
        fprintf(stderr, "DCC_LISTEN_READY_ONCE_DATA descriptor mismatch\n");
        return 1;
    }
    if (listeners.items[3].kind != DCC_SUGAR_LISTENER_EVENT ||
        listeners.items[3].event.type != DCC_APP_EXTENSION_EVENT ||
        listeners.items[3].event.event_type != DCC_EVENT_GUILD_MEMBER_ADD ||
        listeners.items[3].event.event_handler == NULL ||
        listeners.items[3].event.user_data != &listener_state) {
        fprintf(stderr, "DCC_LISTEN_MEMBER_JOIN_DATA descriptor mismatch\n");
        return 1;
    }
    if (listeners.items[4].kind != DCC_SUGAR_LISTENER_MESSAGE_COMMAND ||
        strcmp(listeners.items[4].message_command.prefix, "!") != 0 ||
        strcmp(listeners.items[4].message_command.name, "ping") != 0 ||
        listeners.items[4].message_command.handler != listener_message_command) {
        fprintf(stderr, "DCC_LISTEN_PREFIX_COMMAND_DATA descriptor mismatch\n");
        return 1;
    }
    if (listeners.items[5].kind != DCC_SUGAR_LISTENER_TASK ||
        listeners.items[5].task.type != DCC_APP_EXTENSION_TASK_EVERY_KST ||
        strcmp(listeners.items[5].task.hhmm, "09:00") != 0 ||
        listeners.items[5].task.handler != listener_task) {
        fprintf(stderr, "DCC_LISTEN_TASK_DAILY_KST_DATA descriptor mismatch\n");
        return 1;
    }
    if (listeners.items[6].kind != DCC_SUGAR_LISTENER_TYPED_SLASH ||
        listeners.items[6].typed_slash.handler != listener_echo ||
        listeners.items[6].typed_slash.user_data != &listener_state ||
        listeners.items[6].typed_slash.policy.guild_only != 1U) {
        fprintf(stderr, "DCC_LISTEN_TYPED_SLASH descriptor mismatch\n");
        return 1;
    }
    return 0;
}

static int check_decorated_listener_descriptors(void) {
    dcc_sugar_listener_list_t listeners =
        DCC_LISTENERS(
            DCC_LISTEN_DECORATED_SLASH(listener_decorated_ping),
            DCC_LISTEN_BUTTON_DECORATED(listener_decorated_refresh),
            DCC_LISTEN_READY_DECORATED(listener_decorated_ready),
            DCC_LISTEN_PREFIX_COMMAND_DECORATED(listener_decorated_text),
            DCC_LISTEN_TASK_DECORATED(listener_decorated_task),
            DCC_LISTEN_TYPED_SLASH_DECORATED(listener_echo)
        );

    if (listeners.count != 6U || listeners.items == NULL) {
        fprintf(stderr, "decorated listener count mismatch\n");
        return 1;
    }
    if (listeners.items[0].kind != DCC_SUGAR_LISTENER_SLASH ||
        strcmp(listeners.items[0].slash.name, "listener_decorated_ping") != 0 ||
        listeners.items[0].slash.handler != listener_decorated_ping) {
        fprintf(stderr, "DCC_LISTEN_DECORATED_SLASH mismatch\n");
        return 1;
    }
    if (listeners.items[1].kind != DCC_SUGAR_LISTENER_COMPONENT ||
        strcmp(listeners.items[1].component.custom_id, "status.decorated-refresh") != 0 ||
        listeners.items[1].component.handler != listener_decorated_refresh) {
        fprintf(stderr, "DCC_LISTEN_BUTTON_DECORATED mismatch\n");
        return 1;
    }
    if (listeners.items[2].kind != DCC_SUGAR_LISTENER_EVENT ||
        listeners.items[2].event.type != DCC_APP_EXTENSION_READY ||
        listeners.items[2].event.ready_handler != listener_decorated_ready ||
        listeners.items[2].event.once != 1U ||
        listeners.items[2].event.user_data != &listener_state) {
        fprintf(stderr, "DCC_LISTEN_READY_DECORATED mismatch\n");
        return 1;
    }
    if (listeners.items[3].kind != DCC_SUGAR_LISTENER_MESSAGE_COMMAND ||
        strcmp(listeners.items[3].message_command.prefix, "!") != 0 ||
        strcmp(listeners.items[3].message_command.name, "decorated") != 0 ||
        listeners.items[3].message_command.handler != listener_decorated_text ||
        listeners.items[3].message_command.user_data != &listener_state) {
        fprintf(stderr, "DCC_LISTEN_PREFIX_COMMAND_DECORATED mismatch\n");
        return 1;
    }
    if (listeners.items[4].kind != DCC_SUGAR_LISTENER_TASK ||
        listeners.items[4].task.type != DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST ||
        strcmp(listeners.items[4].task.hhmm, "10:15") != 0 ||
        listeners.items[4].task.handler != listener_decorated_task ||
        listeners.items[4].task.user_data != &listener_state) {
        fprintf(stderr, "DCC_LISTEN_TASK_DECORATED mismatch\n");
        return 1;
    }
    if (listeners.items[5].kind != DCC_SUGAR_LISTENER_TYPED_SLASH ||
        listeners.items[5].typed_slash.handler != listener_echo ||
        listeners.items[5].typed_slash.policy.guild_only != 1U) {
        fprintf(stderr, "DCC_LISTEN_TYPED_SLASH_DECORATED mismatch\n");
        return 1;
    }
    return 0;
}

static int check_listener_runtime_registration(void) {
    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK) {
        return 1;
    }

    int ok =
        dcc_sugar_listeners_register(NULL, NULL) == DCC_ERR_INVALID_ARG &&
        DCC_LISTENERS_REGISTER(
            app,
            DCC_LISTEN_SLASH_DATA("runtime-ping", "Reply with pong", listener_slash, &listener_state),
            DCC_LISTEN_BUTTON_NS_DATA("runtime", "refresh", listener_button, &listener_state),
            DCC_LISTEN_READY_ONCE_DATA(listener_ready, &listener_state),
            DCC_LISTEN_MEMBER_JOIN_DATA(listener_member_join, &listener_state),
            DCC_LISTEN_PREFIX_COMMAND_DATA("!", "runtime", listener_message_command, &listener_state)
        ) == DCC_OK &&
        DCC_LOAD_LISTENERS(app, listener_feature) == DCC_OK;

    dcc_app_destroy(app);
    return ok ? 0 : 1;
}

static int check_simple_bot_definition(void) {
    dcc_app_definition_t bot =
        DCC_SIMPLE_BOT(
            "listener.simple",
            DCC_LISTEN_SLASH_DATA("simple-ping", "Reply with pong", listener_slash, &listener_state),
            DCC_LISTEN_BUTTON_NS_DATA("simple", "refresh", listener_button, &listener_state),
            DCC_LISTEN_MEMBER_JOIN_DATA(listener_member_join, &listener_state),
            DCC_LISTEN_TYPED_SLASH(DCC_TYPED_ROUTE(listener_echo))
        );

    if (strcmp(bot.name, "listener.simple") != 0 ||
        bot.listener_count != 4U ||
        bot.listeners == NULL ||
        bot.listeners[0].kind != DCC_APP_LISTENER_SLASH ||
        strcmp(bot.listeners[0].slash.name, "simple-ping") != 0 ||
        bot.listeners[1].kind != DCC_APP_LISTENER_COMPONENT ||
        strcmp(bot.listeners[1].component.custom_id, "simple.refresh") != 0 ||
        bot.listeners[2].kind != DCC_APP_LISTENER_EVENT ||
        bot.listeners[2].event.event_type != DCC_EVENT_GUILD_MEMBER_ADD ||
        bot.listeners[3].kind != DCC_APP_LISTENER_TYPED_SLASH ||
        bot.auto_defer_after_ms != 1500U ||
        bot.auto_defer_ephemeral != 1U ||
        bot.use_default_error_responses != 1U ||
        bot.command_sync == NULL) {
        fprintf(stderr, "DCC_SIMPLE_BOT definition mismatch\n");
        return 1;
    }

    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK) {
        return 1;
    }

    int ok = dcc_app_apply(app, &bot) == DCC_OK;
    dcc_app_destroy(app);
    return ok ? 0 : 1;
}

static int check_route_table_aliases(void) {
    dcc_sugar_listener_list_t routes =
        DCC_ROUTES(
            DCC_ROUTE_COMMAND_DATA("route-ping", "Reply with pong", listener_slash, &listener_state),
            DCC_ROUTE_BUTTON_NS_DATA("route", "refresh", listener_button, &listener_state),
            DCC_ROUTE_SELECT_NS_PREFIX_DATA("route", "pick.", listener_select, &listener_state),
            DCC_ROUTE_MODAL_NS_PREFIX_DATA("route", "edit.", listener_button, &listener_state),
            DCC_ROUTE_READY_ONCE_DATA(listener_ready, &listener_state),
            DCC_ROUTE_MEMBER_JOIN_DATA(listener_member_join, &listener_state),
            DCC_ROUTE_PREFIX_COMMAND_DATA("!", "route", listener_message_command, &listener_state),
            DCC_ROUTE_TASK_DAILY_KST_DATA("11:30", listener_task, &listener_state),
            DCC_ROUTE_COMMAND_PARAMS_DATA_GUARDED(
                "route-typed",
                "Typed route alias",
                listener_echo_args_t,
                listener_echo,
                &listener_state,
                LISTENER_ECHO_PARAMS,
                DCC_ONLY_GUILD()
            )
        );

    if (routes.count != 9U ||
        routes.items == NULL ||
        routes.items[0].kind != DCC_SUGAR_LISTENER_SLASH ||
        strcmp(routes.items[0].slash.name, "route-ping") != 0 ||
        routes.items[0].slash.user_data != &listener_state ||
        routes.items[1].kind != DCC_SUGAR_LISTENER_COMPONENT ||
        strcmp(routes.items[1].component.custom_id, "route.refresh") != 0 ||
        routes.items[2].kind != DCC_SUGAR_LISTENER_COMPONENT ||
        strcmp(routes.items[2].component.custom_id, "route.pick.") != 0 ||
        routes.items[3].kind != DCC_SUGAR_LISTENER_COMPONENT ||
        strcmp(routes.items[3].component.custom_id, "route.edit.") != 0 ||
        routes.items[4].kind != DCC_SUGAR_LISTENER_EVENT ||
        routes.items[4].event.type != DCC_APP_EXTENSION_READY ||
        routes.items[4].event.once != 1U ||
        routes.items[5].kind != DCC_SUGAR_LISTENER_EVENT ||
        routes.items[5].event.event_type != DCC_EVENT_GUILD_MEMBER_ADD ||
        routes.items[6].kind != DCC_SUGAR_LISTENER_MESSAGE_COMMAND ||
        strcmp(routes.items[6].message_command.name, "route") != 0 ||
        routes.items[7].kind != DCC_SUGAR_LISTENER_TASK ||
        routes.items[7].task.type != DCC_APP_EXTENSION_TASK_EVERY_KST ||
        strcmp(routes.items[7].task.hhmm, "11:30") != 0 ||
        routes.items[8].kind != DCC_SUGAR_LISTENER_TYPED_SLASH ||
        routes.items[8].typed_slash.handler != listener_echo ||
        routes.items[8].typed_slash.user_data != &listener_state ||
        routes.items[8].typed_slash.policy.guild_only != 1U) {
        fprintf(stderr, "DCC_ROUTES descriptor mismatch\n");
        return 1;
    }
    if (routes.items[2].component.type != DCC_APP_EXTENSION_SELECT_PREFIX ||
        routes.items[3].component.type != DCC_APP_EXTENSION_MODAL_PREFIX) {
        fprintf(stderr, "DCC_ROUTE component prefix type mismatch\n");
        return 1;
    }

    dcc_app_definition_t bot =
        DCC_BOT_ROUTES(
            "route.bot",
            DCC_ROUTE_COMMAND("route-simple", "Reply with pong", listener_slash),
            DCC_ROUTE_READY(listener_ready)
        );
    dcc_app_definition_t guild_bot =
        DCC_GUILD_BOT_ROUTES(
            "route.guild",
            123ULL,
            DCC_ROUTE_COMMAND("route-guild", "Reply with pong", listener_slash)
        );

    if (strcmp(bot.name, "route.bot") != 0 ||
        bot.listener_count != 2U ||
        bot.listeners == NULL ||
        strcmp(bot.listeners[0].slash.name, "route-simple") != 0 ||
        bot.listeners[1].kind != DCC_APP_LISTENER_EVENT ||
        bot.auto_defer_after_ms != 1500U ||
        bot.auto_defer_ephemeral != 1U ||
        bot.use_default_error_responses != 1U ||
        guild_bot.command_sync == NULL ||
        guild_bot.command_sync->command_registry.guild_id != 123ULL ||
        guild_bot.listener_count != 1U) {
        fprintf(stderr, "DCC_BOT_ROUTES definition mismatch\n");
        return 1;
    }

    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        return 1;
    }

    int ok =
        DCC_ROUTES_REGISTER(
            app,
            DCC_ROUTE_COMMAND("registered-route", "Registered route", listener_slash)
        ) == DCC_OK &&
        dcc_command_registry_count(dcc_app_command_registry(app)) == 1U;
    dcc_app_destroy(app);
    return ok ? 0 : 1;
}

static int check_typed_component_listener_descriptors(void) {
    dcc_sugar_listener_t modal =
        DCC_LISTEN_MODAL_PARAMS_DATA_VALIDATED(
            "listener.modal",
            listener_modal_args_t,
            listener_typed_component,
            &listener_state,
            LISTENER_MODAL_PARAMS,
            DCC_VALIDATORS(
                DCC_VALIDATE_STRING_REQUIRED(listener_modal_args_t, name, "name")
            )
        );
    if (modal.kind != DCC_SUGAR_LISTENER_TYPED_MODAL ||
        strcmp(modal.typed_modal.custom_id, "listener.modal") != 0 ||
        modal.typed_modal.bindings.count != 2U ||
        modal.typed_modal.validators.count != 1U ||
        modal.typed_modal.user_data != &listener_state) {
        fprintf(stderr, "DCC_LISTEN_MODAL_PARAMS_DATA_VALIDATED descriptor mismatch\n");
        return 1;
    }

    dcc_sugar_listener_t button =
        DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED(
            "listener",
            "button.",
            listener_component_args_t,
            listener_typed_component,
            &listener_state,
            LISTENER_COMPONENT_PARAMS,
            DCC_ONLY_GUILD()
        );
    if (button.kind != DCC_SUGAR_LISTENER_TYPED_BUTTON ||
        strcmp(button.typed_component.custom_id, "listener.button.") != 0 ||
        button.typed_component.prefix != 1U ||
        button.typed_component.bindings.count != 2U ||
        button.typed_component.user_data != &listener_state ||
        button.typed_component.policy.guild_only != 1U) {
        fprintf(stderr, "DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED descriptor mismatch\n");
        return 1;
    }

    dcc_sugar_listener_t select =
        DCC_LISTEN_SELECT_NS_PARAMS_FN_DATA_GUARDED(
            "listener",
            listener_typed_component,
            listener_component_args_t,
            &listener_state,
            LISTENER_COMPONENT_PARAMS,
            DCC_ONLY_GUILD()
        );
    if (select.kind != DCC_SUGAR_LISTENER_TYPED_SELECT ||
        strcmp(select.typed_component.custom_id, "listener.listener_typed_component") != 0 ||
        select.typed_component.bindings.count != 2U ||
        select.typed_component.user_data != &listener_state ||
        select.typed_component.policy.guild_only != 1U) {
        fprintf(stderr, "DCC_LISTEN_SELECT_NS_PARAMS_FN_DATA_GUARDED descriptor mismatch\n");
        return 1;
    }

    return 0;
}

static int check_command_schema_listener_apply(void) {
    dcc_app_definition_t bot =
        DCC_SIMPLE_BOT(
            "listener.schema",
            DCC_LISTEN_COMMAND_SCHEMA(DCC_CMD("registered", "Registered command"))
        );

    if (bot.listener_count != 1U ||
        bot.listeners == NULL ||
        bot.listeners[0].kind != DCC_APP_LISTENER_COMMAND_SCHEMA ||
        strcmp(bot.listeners[0].command_schema.name, "registered") != 0) {
        fprintf(stderr, "DCC_LISTEN_COMMAND_SCHEMA descriptor mismatch\n");
        return 1;
    }

    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        return 1;
    }

    int ok =
        dcc_app_apply(app, &bot) == DCC_OK &&
        dcc_command_registry_count(dcc_app_command_registry(app)) == 1U;
    dcc_app_destroy(app);
    return ok ? 0 : 1;
}

static int check_listener_module_descriptor(void) {
    dcc_app_module_t module = DCC_LISTENERS_MODULE(listener_feature, &listener_state);
    if (module.size != sizeof(dcc_app_module_t) ||
        strcmp(module.name, "listener_feature") != 0 ||
        module.setup != DCC_LISTENERS_SETUP(listener_feature) ||
        module.user_data != &listener_state) {
        fprintf(stderr, "DCC_LISTENERS_MODULE descriptor mismatch\n");
        return 1;
    }
    return 0;
}

static int check_feature_alias_module_descriptor(void) {
    dcc_app_extension_t extension =
        DCC_FEATURE(
            "feature_alias",
            DCC_FEATURE_COMMANDS(
                DCC_ON_SLASH_DATA("feature", "Feature alias command", listener_slash, &listener_state)
            ),
            DCC_FEATURE_EVENTS(
                DCC_READY_DATA(listener_ready, &listener_state),
                DCC_MEMBER_JOIN_DATA(listener_member_join, &listener_state),
                DCC_EVENT_DATA(DCC_EVENT_READY, listener_event, &listener_state)
            )
        );
    dcc_app_module_t module = DCC_USE_FEATURE(feature_alias, &listener_state);
    dcc_app_definition_t bot =
        DCC_BOT_FEATURES(
            "feature.bot",
            DCC_USE_FEATURE(feature_alias, &listener_state)
        );
    if (extension.slash_command_count != 1U ||
        extension.slash_commands == NULL ||
        strcmp(extension.slash_commands[0].name, "feature") != 0 ||
        extension.slash_commands[0].user_data != &listener_state) {
        fprintf(stderr, "DCC_FEATURE descriptor mismatch\n");
        return 1;
    }
    if (extension.event_count != 3U ||
        extension.events == NULL ||
        extension.events[0].ready_handler != listener_ready ||
        extension.events[0].user_data != &listener_state ||
        extension.events[1].event_handler != listener_member_join_dcc_event_dispatch ||
        extension.events[1].event_type != DCC_EVENT_GUILD_MEMBER_ADD ||
        extension.events[2].event_handler != listener_event ||
        extension.events[2].event_type != DCC_EVENT_READY) {
        fprintf(stderr, "DCC event alias descriptor mismatch\n");
        return 1;
    }
    if (module.size != sizeof(dcc_app_module_t) ||
        strcmp(module.name, "feature_alias") != 0 ||
        module.setup != DCC_FEATURE_SETUP(feature_alias) ||
        module.user_data != &listener_state) {
        fprintf(stderr, "DCC_USE_FEATURE descriptor mismatch\n");
        return 1;
    }
    if (strcmp(bot.name, "feature.bot") != 0 ||
        bot.module_count != 1U ||
        bot.modules == NULL ||
        strcmp(bot.modules[0].name, "feature_alias") != 0 ||
        bot.modules[0].setup != DCC_FEATURE_SETUP(feature_alias) ||
        bot.modules[0].user_data != &listener_state ||
        bot.auto_defer_after_ms != 1500U ||
        bot.auto_defer_ephemeral != 1U ||
        bot.use_default_error_responses != 1U) {
        fprintf(stderr, "DCC_BOT_FEATURES descriptor mismatch\n");
        return 1;
    }

    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    if (dcc_app_create(&options, &app) != DCC_OK) {
        return 1;
    }

    int ok =
        DCC_LOAD_FEATURE_DATA(app, feature_alias, &listener_state) == DCC_OK &&
        dcc_command_registry_count(dcc_app_command_registry(app)) == 1U;
    dcc_app_destroy(app);

    if (!ok || dcc_app_create(&options, &app) != DCC_OK) {
        return 1;
    }

    ok =
        dcc_app_apply(app, &bot) == DCC_OK &&
        dcc_command_registry_count(dcc_app_command_registry(app)) == 1U;
    dcc_app_destroy(app);
    return ok ? 0 : 1;
}

int main(void) {
    if (check_listener_descriptors() != 0 ||
        check_decorated_listener_descriptors() != 0 ||
        check_listener_runtime_registration() != 0 ||
        check_simple_bot_definition() != 0 ||
        check_route_table_aliases() != 0 ||
        check_typed_component_listener_descriptors() != 0 ||
        check_command_schema_listener_apply() != 0 ||
        check_listener_module_descriptor() != 0 ||
        check_feature_alias_module_descriptor() != 0) {
        return 1;
    }
    return 0;
}
