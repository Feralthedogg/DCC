#include <dcc/sugar.h>

#include "internal/client/dcc_client_state_internal.h"
#include "internal/events/dcc_event_state_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct app_dx_sugar_state {
    unsigned ready_once_count;
    unsigned ready_count;
    unsigned task_count;
    unsigned view_count;
    unsigned explicit_view_count;
    unsigned group_view_count;
    unsigned member_join_count;
    unsigned role_update_count;
    unsigned typing_start_count;
    unsigned user_context_count;
    unsigned message_context_count;
    unsigned bad;
} app_dx_sugar_state_t;

typedef struct app_dx_direct_args {
    const char *query;
    const char *suffix;
    const char *value;
} app_dx_direct_args_t;

#define APP_DX_DIRECT_SLASH_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, query, "query", "Search query")

#define APP_DX_DIRECT_MODAL_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, query, "query")

#define APP_DX_DIRECT_COMPONENT_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_CUSTOM_ID_SUFFIX, suffix, "direct.typed.button:") \
    PARAM(type, FIRST_VALUE, value, NULL)

static int test_set_env(const char *name, const char *value) {
#if defined(_WIN32)
    return _putenv_s(name, value) == 0;
#else
    return setenv(name, value, 1) == 0;
#endif
}

static int test_unset_env(const char *name) {
#if defined(_WIN32)
    return _putenv_s(name, "") == 0;
#else
    return unsetenv(name) == 0;
#endif
}

static int text_contains(const char *text, const char *needle) {
    return text != NULL && needle != NULL && strstr(text, needle) != NULL;
}

DCC_READY_FN(on_ready_once) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (app != NULL && ready != NULL && ready->session_id != NULL) {
        state->ready_once_count++;
    }
}

DCC_READY_FN(on_ready) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (app != NULL && ready != NULL && ready->session_id != NULL) {
        state->ready_count++;
    }
}

DCC_TASK_FN(on_task) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (app != NULL) {
        state->task_count++;
    }
}

DCC_MIDDLEWARE_FN(app_dx_guard_middleware) {
    (void)ctx;
    return user_data != NULL ? DCC_OK : DCC_ERR_INVALID_ARG;
}

DCC_CHECK_FN(app_dx_guard_check) {
    (void)ctx;
    return user_data != NULL ? DCC_OK : DCC_ERR_INVALID_ARG;
}

DCC_HANDLER(on_view_default) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (ctx == NULL ||
        state == NULL ||
        dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        dcc_ctx_user_data(ctx) != state) {
        if (state != NULL) {
            state->bad = 1U;
        }
        return;
    }
    if (strcmp(DCC_CTX_USER_MENTION(ctx), "<@600>") != 0 ||
        strcmp(DCC_CTX_AUTHOR_MENTION(ctx), "<@600>") != 0 ||
        strcmp(DCC_CTX_MEMBER_MENTION(ctx), "<@!600>") != 0 ||
        strcmp(DCC_CTX_CHANNEL_MENTION(ctx), "<#400>") != 0 ||
        strcmp(DCC_CTX_MESSAGE_LINK(ctx), "https://discord.com/channels/300/400/500") != 0 ||
        DCC_USER_ID(ctx) != 600U ||
        DCC_AUTHOR_ID(ctx) != 600U ||
        DCC_GUILD_ID(ctx) != 300U ||
        DCC_CHANNEL_ID(ctx) != 400U ||
        DCC_MESSAGE_ID(ctx) != 500U ||
        DCC_USER_DATA(ctx) != state ||
        DCC_CUSTOM_ID(ctx) == NULL) {
        state->bad = 1U;
        return;
    }
    state->view_count++;
}

DCC_HANDLER(on_view_explicit) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (ctx == NULL ||
        state == NULL ||
        dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        dcc_ctx_user_data(ctx) != state) {
        if (state != NULL) {
            state->bad = 1U;
        }
        return;
    }
    state->explicit_view_count++;
}

DCC_HANDLER(on_group_view) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (ctx == NULL ||
        state == NULL ||
        dcc_ctx_event_type(ctx) != DCC_EVENT_BUTTON_CLICK ||
        dcc_ctx_user_data(ctx) != state) {
        if (state != NULL) {
            state->bad = 1U;
        }
        return;
    }
    state->group_view_count++;
}

DCC_SLASH_FN(on_alias_slash) {
    (void)ctx;
    (void)user_data;
}

DCC_COMMAND_FN(on_alias_command) {
    (void)ctx;
    (void)user_data;
}

DCC_BUTTON_FN(on_alias_button) {
    (void)ctx;
    (void)user_data;
}

DCC_SELECT_FN(on_alias_select) {
    (void)ctx;
    (void)user_data;
}

DCC_MODAL_FN(on_alias_modal) {
    (void)ctx;
    (void)user_data;
}

DCC_TYPED_SLASH_IMPL(on_direct_typed_search, app_dx_direct_args_t, command) {
    (void)ctx;
    (void)command;
    (void)user_data;
}

DCC_TYPED_COMMAND_IMPL(on_direct_typed_lookup, app_dx_direct_args_t, command) {
    (void)ctx;
    (void)command;
    (void)user_data;
}

DCC_TYPED_MODAL_IMPL(on_direct_typed_modal, app_dx_direct_args_t, modal) {
    (void)ctx;
    (void)modal;
    (void)user_data;
}

DCC_TYPED_COMPONENT_IMPL(on_direct_typed_component, app_dx_direct_args_t, component) {
    (void)ctx;
    (void)component;
    (void)user_data;
}

DCC_HANDLER(on_user_context_menu) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (ctx == NULL ||
        state == NULL ||
        dcc_ctx_event_type(ctx) != DCC_EVENT_USER_CONTEXT_MENU ||
        DCC_CTX_TARGET_ID(ctx) != 700U ||
        DCC_CTX_USER_DATA(ctx) != state) {
        if (state != NULL) {
            state->bad = 1U;
        }
        return;
    }
    state->user_context_count++;
}

DCC_HANDLER(on_message_context_menu) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (ctx == NULL ||
        state == NULL ||
        dcc_ctx_event_type(ctx) != DCC_EVENT_MESSAGE_CONTEXT_MENU ||
        DCC_CTX_TARGET_ID(ctx) != 701U ||
        DCC_CTX_USER_DATA(ctx) != state) {
        if (state != NULL) {
            state->bad = 1U;
        }
        return;
    }
    state->message_context_count++;
}

DCC_MEMBER_JOIN_FN(on_member_join) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (app == NULL ||
        member == NULL ||
        event == NULL ||
        state == NULL ||
        dcc_event_type(event) != DCC_EVENT_GUILD_MEMBER_ADD ||
        member->guild_id != 300U ||
        member->user.id != 601U) {
        if (state != NULL) {
            state->bad = 1U;
        }
        return;
    }
    state->member_join_count++;
}

DCC_ROLE_UPDATE_FN(on_role_update) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (app == NULL ||
        role == NULL ||
        event == NULL ||
        state == NULL ||
        dcc_event_type(event) != DCC_EVENT_GUILD_ROLE_UPDATE ||
        role->guild_id != 300U ||
        role->id != 602U) {
        if (state != NULL) {
            state->bad = 1U;
        }
        return;
    }
    state->role_update_count++;
}

DCC_TYPING_START_FN(on_typing_start) {
    app_dx_sugar_state_t *state = (app_dx_sugar_state_t *)user_data;
    if (app == NULL ||
        typing == NULL ||
        event == NULL ||
        state == NULL ||
        dcc_event_type(event) != DCC_EVENT_TYPING_START ||
        typing->guild_id != 300U ||
        typing->channel_id != 400U ||
        typing->user_id != 603U) {
        if (state != NULL) {
            state->bad = 1U;
        }
        return;
    }
    state->typing_start_count++;
}

#define APP_DX_VIEW_ACTIONS \
    DCC_VIEW_ACTION_PRIMARY("Default", "dx.action.default", on_view_default), \
    DCC_VIEW_ACTION_DANGER_DATA("Explicit", "dx.action.explicit", on_view_explicit, &explicit_state)

#define APP_DX_VIEW_ROUTE_ACTIONS \
    DCC_VIEW_ACTION_SUCCESS("Route", "dx.route.default", on_view_default), \
    DCC_VIEW_ACTION_SECONDARY_DATA("Route Explicit", "dx.route.explicit", on_view_explicit, &explicit_state)

#define APP_DX_NS_VIEW_ACTIONS \
    DCC_VIEW_ACTION_PRIMARY_NS("NS Default", "dx.ns", "default", on_view_default), \
    DCC_VIEW_ACTION_DANGER_NS_DATA("NS Explicit", "dx.ns", "explicit", on_view_explicit, &explicit_state)

#define APP_DX_NS_USE_ACTIONS \
    DCC_VIEW_ACTION_SUCCESS_NS("NS Use", "dx.ns.use", "default", on_view_default), \
    DCC_VIEW_ACTION_SECONDARY_NS_DATA("NS Use Explicit", "dx.ns.use", "explicit", on_view_explicit, &explicit_state)

static dcc_status_t dispatch_ready(dcc_app_t *app) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_READY;
    event.data.ready.session_id = "dx-ready";
    event.data.ready.shard_count = 1U;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_status_t dispatch_button(dcc_app_t *app, const char *custom_id) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_BUTTON_CLICK;
    event.data.interaction.id = 100U;
    event.data.interaction.application_id = 200U;
    event.data.interaction.guild_id = 300U;
    event.data.interaction.channel_id = 400U;
    event.data.interaction.message_id = 500U;
    event.data.interaction.user_id = 600U;
    event.data.interaction.type = 3U;
    event.data.interaction.component_type = 2U;
    event.data.interaction.custom_id = custom_id;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_status_t dispatch_context_menu(
    dcc_app_t *app,
    dcc_event_type_t type,
    const char *name,
    dcc_snowflake_t target_id
) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = type;
    event.data.interaction.id = 101U;
    event.data.interaction.application_id = 200U;
    event.data.interaction.guild_id = 300U;
    event.data.interaction.channel_id = 400U;
    event.data.interaction.user_id = 600U;
    event.data.interaction.target_id = target_id;
    event.data.interaction.name = name;
    event.data.interaction.type = 2U;
    event.data.interaction.command_type = type == DCC_EVENT_USER_CONTEXT_MENU
        ? DCC_APPLICATION_COMMAND_USER
        : DCC_APPLICATION_COMMAND_MESSAGE;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_status_t dispatch_member_join(dcc_app_t *app) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_GUILD_MEMBER_ADD;
    event.data.member.guild_id = 300U;
    event.data.member.user.id = 601U;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_status_t dispatch_role_update(dcc_app_t *app) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_GUILD_ROLE_UPDATE;
    event.data.role.guild_id = 300U;
    event.data.role.id = 602U;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static dcc_status_t dispatch_typing_start(dcc_app_t *app) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_TYPING_START;
    event.data.typing_start.guild_id = 300U;
    event.data.typing_start.channel_id = 400U;
    event.data.typing_start.user_id = 603U;
    return dcc_event_bus_dispatch(&dcc_app_client(app)->events, dcc_app_client(app), &event);
}

static int check_env_requirements(void) {
    const char present[] = "DCC_DX_SUGAR_PRESENT";
    const char missing[] = "DCC_DX_SUGAR_MISSING";
    if (!test_set_env(present, "ok") || !test_unset_env(missing)) {
        return 0;
    }

    dcc_app_env_requirement_t requirements[] = {
        DCC_ENV(present),
        DCC_ENV_DESCRIBED(missing, "Used by app DX sugar smoke")
    };
    if (dcc_app_validate_env_requirements(requirements, DCC_ARRAY_LEN(requirements)) !=
        DCC_ERR_NOT_FOUND) {
        return 0;
    }

    if (!test_set_env(missing, "ok") ||
        dcc_app_validate_env_requirements(requirements, DCC_ARRAY_LEN(requirements)) != DCC_OK) {
        return 0;
    }

    dcc_app_definition_t definition =
        DCC_APP("dx.env", DCC_REQUIRE_ENV(DCC_ENV(present), DCC_ENV(missing)));
    return dcc_app_validate_definition_env(&definition) == DCC_OK &&
        dcc_app_validate_definition_env(NULL) == DCC_ERR_INVALID_ARG &&
        dcc_app_validate_env_requirements(NULL, 1U) == DCC_ERR_INVALID_ARG;
}

static int check_env_config_sugar(void) {
    if (!test_set_env("DCC_DX_CFG_TOKEN", "token-value") ||
        !test_set_env("DCC_DX_CFG_ENABLED", "yes") ||
        !test_set_env("DCC_DX_CFG_LIMIT", "42") ||
        !test_set_env("DCC_DX_CFG_SIGNED", "-7") ||
        !test_set_env("DCC_DX_CFG_CHANNEL", "123456789012345678") ||
        !test_unset_env("DCC_DX_CFG_OPTIONAL_CHANNEL") ||
        !test_unset_env("DCC_DX_CFG_FALLBACK") ||
        !test_unset_env("DCC_DX_CFG_MISSING")) {
        return 0;
    }

    const char *token = NULL;
    const char *fallback = NULL;
    uint8_t enabled = 0U;
    uint8_t default_bool = 0U;
    uint64_t limit = 0U;
    int64_t signed_value = 0;
    dcc_snowflake_t channel_id = 0U;
    dcc_snowflake_t optional_channel_id = 999U;

    if (DCC_CONFIG(
            DCC_CONFIG_STRING("DCC_DX_CFG_TOKEN", &token),
            DCC_CONFIG_BOOL("DCC_DX_CFG_ENABLED", &enabled),
            DCC_CONFIG_U64("DCC_DX_CFG_LIMIT", &limit),
            DCC_CONFIG_I64("DCC_DX_CFG_SIGNED", &signed_value),
            DCC_CONFIG_CHANNEL("DCC_DX_CFG_CHANNEL", &channel_id),
            DCC_CONFIG_CHANNEL_OPTIONAL("DCC_DX_CFG_OPTIONAL_CHANNEL", &optional_channel_id),
            DCC_CONFIG_STRING_DEFAULT("DCC_DX_CFG_FALLBACK", "fallback-value", &fallback),
            DCC_CONFIG_BOOL_DEFAULT("DCC_DX_CFG_DEFAULT_BOOL", 1U, &default_bool)
        ) != DCC_OK) {
        return 0;
    }

    const char *direct_string = NULL;
    uint8_t direct_bool = 0U;
    uint64_t direct_u64 = 0U;
    int64_t direct_i64 = 0;
    dcc_snowflake_t direct_snowflake = 0U;
    int ok =
        token != NULL &&
        strcmp(token, "token-value") == 0 &&
        fallback != NULL &&
        strcmp(fallback, "fallback-value") == 0 &&
        enabled == 1U &&
        default_bool == 1U &&
        limit == 42U &&
        signed_value == -7 &&
        channel_id == 123456789012345678ULL &&
        optional_channel_id == 999U &&
        DCC_ENV_STRING("DCC_DX_CFG_TOKEN", &direct_string) == DCC_OK &&
        direct_string != NULL &&
        strcmp(direct_string, "token-value") == 0 &&
        DCC_ENV_BOOL("DCC_DX_CFG_ENABLED", &direct_bool) == DCC_OK &&
        direct_bool == 1U &&
        DCC_ENV_U64("DCC_DX_CFG_LIMIT", &direct_u64) == DCC_OK &&
        direct_u64 == 42U &&
        DCC_ENV_I64("DCC_DX_CFG_SIGNED", &direct_i64) == DCC_OK &&
        direct_i64 == -7 &&
        DCC_ENV_CHANNEL("DCC_DX_CFG_CHANNEL", &direct_snowflake) == DCC_OK &&
        direct_snowflake == 123456789012345678ULL &&
        DCC_ENV_STRING_OR("DCC_DX_CFG_MISSING", "default", &direct_string) == DCC_OK &&
        strcmp(direct_string, "default") == 0 &&
        DCC_ENV_BOOL_OR("DCC_DX_CFG_MISSING", 1U, &direct_bool) == DCC_OK &&
        direct_bool == 1U &&
        DCC_ENV_U64_OR("DCC_DX_CFG_MISSING", 123U, &direct_u64) == DCC_OK &&
        direct_u64 == 123U &&
        DCC_ENV_I64_OR("DCC_DX_CFG_MISSING", -123, &direct_i64) == DCC_OK &&
        direct_i64 == -123 &&
        DCC_ENV_CHANNEL_OR("DCC_DX_CFG_MISSING", 456U, &direct_snowflake) == DCC_OK &&
        direct_snowflake == 456U &&
        DCC_CONFIG(DCC_CONFIG_STRING_DESCRIBED("DCC_DX_CFG_MISSING", "missing smoke", &token)) ==
            DCC_ERR_NOT_FOUND;

    if (!ok) {
        return 0;
    }

    if (!test_set_env("DCC_DX_CFG_ENABLED", "maybe") ||
        DCC_ENV_BOOL("DCC_DX_CFG_ENABLED", &direct_bool) != DCC_ERR_INVALID_ARG ||
        !test_set_env("DCC_DX_CFG_LIMIT", "-1") ||
        DCC_ENV_U64("DCC_DX_CFG_LIMIT", &direct_u64) != DCC_ERR_INVALID_ARG ||
        !test_set_env("DCC_DX_CFG_CHANNEL", "0") ||
        DCC_ENV_CHANNEL("DCC_DX_CFG_CHANNEL", &direct_snowflake) != DCC_ERR_INVALID_ARG ||
        dcc_app_env_bind(NULL, 1U) != DCC_ERR_INVALID_ARG ||
        dcc_app_env_bind(NULL, 0U) != DCC_OK) {
        return 0;
    }

    return 1;
}

static int check_preset_macros(void) {
    app_dx_sugar_state_t state = {0};
    dcc_app_definition_t definition =
        DCC_APP(
            "dx.preset",
            DCC_APP_PRESET_DEV_GUILD(123ULL),
            DCC_REQUIRE_ENV(DCC_ENV("DCC_DX_SUGAR_PRESENT")),
            DCC_APP_EVENTS(DCC_ON_READY_ONCE_DATA(on_ready_once, &state)),
            DCC_APP_TASKS(
                DCC_TASK_DAILY_KST("00:00", on_task),
                DCC_TASK_DAILY_AT_KST_DATA(9U, 30U, on_task, &state)
            )
        );
    dcc_app_definition_t bot_definition =
        DCC_BOT(
            "dx.bot",
            DCC_APP_EVENTS(DCC_ON_READY_DATA(on_ready, &state))
        );
    dcc_app_definition_t guild_bot_definition =
        DCC_GUILD_BOT(
            "dx.guild_bot",
            789ULL,
            DCC_APP_TASKS(DCC_TASK_LOOP_SECONDS_DATA(60U, on_task, &state))
        );
    dcc_app_definition_t dev_guild_bot_definition =
        DCC_DEV_GUILD_BOT(
            "dx.dev_guild_bot",
            789ULL,
            DCC_APP_TASKS(DCC_TASK_LOOP_SECONDS_DATA(60U, on_task, &state))
        );
    dcc_app_definition_t plan_definition =
        DCC_DEV_PLAN_APP(
            "dx.plan",
            DCC_APP_EVENTS(DCC_ON_READY_DATA(on_ready, &state))
        );
    dcc_app_options_t bot_options =
        DCC_APP_OPTIONS_PRESET_BOT_INTENTS("token", DCC_INTENTS_ALL);
    dcc_app_options_t dev_options =
        DCC_APP_OPTIONS_PRESET_DEV_GUILD("token", 456ULL);

    if (0) {
        (void)DCC_RUN_BOT("dx.run", DCC_APP_EVENTS(DCC_ON_READY_DATA(on_ready, &state)));
        (void)DCC_RUN_BOT_ENV(
            "DCC_DX_TOKEN",
            "dx.run.env",
            DCC_APP_EVENTS(DCC_ON_READY_DATA(on_ready, &state))
        );
        (void)DCC_RUN_BOT_TOKEN(
            "DCC_DX_TOKEN",
            "dx.run.token",
            DCC_APP_EVENTS(DCC_ON_READY_DATA(on_ready, &state))
        );
        (void)DCC_RUN_GUILD_BOT(
            "dx.run.guild",
            789ULL,
            DCC_APP_EVENTS(DCC_ON_READY_DATA(on_ready, &state))
        );
        (void)DCC_RUN_GUILD_BOT_ENV(
            "DCC_DX_TOKEN",
            "dx.run.guild.env",
            789ULL,
            DCC_APP_EVENTS(DCC_ON_READY_DATA(on_ready, &state))
        );
        (void)DCC_RUN_GUILD_BOT_TOKEN(
            "DCC_DX_TOKEN",
            "dx.run.guild.token",
            789ULL,
            DCC_APP_EVENTS(DCC_ON_READY_DATA(on_ready, &state))
        );
        (void)DCC_RUN_SIMPLE_BOT_ENV(
            "DCC_DX_TOKEN",
            "dx.run.simple.env",
            DCC_LISTEN_READY_DATA(on_ready, &state)
        );
        (void)DCC_RUN_SIMPLE_BOT_TOKEN(
            "DCC_DX_TOKEN",
            "dx.run.simple.token",
            DCC_LISTEN_READY_DATA(on_ready, &state)
        );
        (void)DCC_RUN_BOT_LISTENERS_ENV(
            "DCC_DX_TOKEN",
            "dx.run.listeners.env",
            DCC_LISTEN_READY_DATA(on_ready, &state)
        );
        (void)DCC_RUN_BOT_LISTENERS_TOKEN(
            "DCC_DX_TOKEN",
            "dx.run.listeners.token",
            DCC_LISTEN_READY_DATA(on_ready, &state)
        );
        (void)DCC_RUN_SIMPLE_GUILD_BOT_ENV(
            "DCC_DX_TOKEN",
            "dx.run.simple.guild.env",
            789ULL,
            DCC_LISTEN_READY_DATA(on_ready, &state)
        );
        (void)DCC_RUN_SIMPLE_GUILD_BOT_TOKEN(
            "DCC_DX_TOKEN",
            "dx.run.simple.guild.token",
            789ULL,
            DCC_LISTEN_READY_DATA(on_ready, &state)
        );
        (void)DCC_RUN_GUILD_BOT_LISTENERS_ENV(
            "DCC_DX_TOKEN",
            "dx.run.guild.listeners.env",
            789ULL,
            DCC_LISTEN_READY_DATA(on_ready, &state)
        );
        (void)DCC_RUN_GUILD_BOT_LISTENERS_TOKEN(
            "DCC_DX_TOKEN",
            "dx.run.guild.listeners.token",
            789ULL,
            DCC_LISTEN_READY_DATA(on_ready, &state)
        );
    }

    return definition.command_sync != NULL &&
        definition.command_sync->infer_application_id == 1U &&
        definition.command_sync->apply == 1U &&
        definition.command_sync->command_registry.guild_id == 123ULL &&
        definition.auto_defer_after_ms == 1500U &&
        definition.auto_defer_ephemeral == 1U &&
        definition.use_default_error_responses == 1U &&
        definition.required_env_count == 1U &&
        definition.event_count == 1U &&
        definition.events[0].once == 1U &&
        definition.task_count == 2U &&
        definition.tasks[0].type == DCC_APP_EXTENSION_TASK_EVERY_KST &&
        strcmp(definition.tasks[0].hhmm, "00:00") == 0 &&
        definition.tasks[1].type == DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST &&
        definition.tasks[1].hour == 9U &&
        definition.tasks[1].minute == 30U &&
        bot_definition.command_sync == NULL &&
        bot_definition.auto_defer_after_ms == 1500U &&
        bot_definition.auto_defer_ephemeral == 1U &&
        bot_definition.use_default_error_responses == 1U &&
        bot_definition.event_count == 1U &&
        guild_bot_definition.command_sync == NULL &&
        guild_bot_definition.task_count == 1U &&
        dev_guild_bot_definition.command_sync != NULL &&
        dev_guild_bot_definition.command_sync->command_registry.guild_id == 789ULL &&
        plan_definition.command_sync != NULL &&
        plan_definition.command_sync->apply == 0U &&
        plan_definition.command_sync->infer_application_id == 1U &&
        bot_options.client.intents == DCC_INTENTS_ALL &&
        bot_options.client.enable_cache == 1U &&
        bot_options.client.infer_guild_id_from_channel == 1U &&
        dev_options.command_registry.guild_id == 456ULL &&
        dev_options.command_sync_on_ready == 1U &&
        dev_options.command_sync.command_registry.guild_id == 456ULL &&
        dev_options.auto_defer_after_ms == 1500U &&
        dev_options.auto_defer_ephemeral == 1U;
}

static int check_guard_sugar(void) {
    app_dx_sugar_state_t state = {0};
    dcc_app_route_policy_t policy =
        DCC_GUARDS(
            DCC_GUARD_MIDDLEWARE(DCC_MIDDLEWARE(app_dx_guard_middleware, &state)),
            DCC_GUARD_GUILD_ONLY(),
            DCC_GUARD_OWNER(42ULL),
            DCC_GUARD_HAS_ROLE(43ULL),
            DCC_GUARD_HAS_ANY_ROLE(44ULL, 45ULL),
            DCC_GUARD_COOLDOWN_USER(2U, 1000U),
            DCC_GUARD_CHECK_MESSAGE(app_dx_guard_check, &state, "Denied", "Nope")
        );
    dcc_app_route_policy_t decorator_policy =
        DCC_POLICY(
            DCC_USE(DCC_MIDDLEWARE(app_dx_guard_middleware, &state)),
            DCC_ONLY_GUILD(),
            DCC_OWNER_ONLY(42ULL),
            DCC_HAS_ROLE(43ULL),
            DCC_HAS_ANY_ROLE(44ULL, 45ULL),
            DCC_RATE_LIMIT_USER(2U, 1000U),
            DCC_WITH_CHECK_MESSAGE(app_dx_guard_check, &state, "Denied", "Nope")
        );

    return policy.size == sizeof(policy) &&
        policy.middleware_count == 1U &&
        policy.middlewares[0].middleware == app_dx_guard_middleware &&
        policy.middlewares[0].user_data == &state &&
        policy.guild_only == 1U &&
        policy.owner_user_id_count == 1U &&
        policy.owner_user_ids[0] == 42ULL &&
        policy.required_role_id_count == 1U &&
        policy.required_role_ids[0] == 43ULL &&
        policy.any_role_id_count == 2U &&
        policy.any_role_ids[0] == 44ULL &&
        policy.any_role_ids[1] == 45ULL &&
        policy.cooldown.bucket == DCC_APP_COOLDOWN_USER &&
        policy.cooldown.limit == 2U &&
        policy.cooldown.window_ms == 1000U &&
        policy.check_count == 1U &&
        policy.checks[0].check == app_dx_guard_check &&
        policy.checks[0].user_data == &state &&
        strcmp(policy.checks[0].title, "Denied") == 0 &&
        strcmp(policy.checks[0].description, "Nope") == 0 &&
        decorator_policy.size == sizeof(decorator_policy) &&
        decorator_policy.middleware_count == 1U &&
        decorator_policy.middlewares[0].middleware == app_dx_guard_middleware &&
        decorator_policy.guild_only == 1U &&
        decorator_policy.owner_user_id_count == 1U &&
        decorator_policy.owner_user_ids[0] == 42ULL &&
        decorator_policy.required_role_id_count == 1U &&
        decorator_policy.required_role_ids[0] == 43ULL &&
        decorator_policy.any_role_id_count == 2U &&
        decorator_policy.any_role_ids[0] == 44ULL &&
        decorator_policy.any_role_ids[1] == 45ULL &&
        decorator_policy.cooldown.bucket == DCC_APP_COOLDOWN_USER &&
        decorator_policy.cooldown.limit == 2U &&
        decorator_policy.cooldown.window_ms == 1000U &&
        decorator_policy.check_count == 1U &&
        strcmp(decorator_policy.checks[0].title, "Denied") == 0;
}

static int check_handler_sugar(void) {
    app_dx_sugar_state_t state = {0};
    dcc_app_extension_slash_command_t slash =
        DCC_ON_SLASH_FN(on_view_default, "Default handler route");
    dcc_app_extension_slash_command_t slash_data =
        DCC_ON_SLASH_FN_DATA(on_view_explicit, "Explicit data handler route", &state);
    dcc_app_extension_slash_command_t command =
        DCC_ON_COMMAND_FN(on_group_view, "Command handler route");
    dcc_app_extension_slash_command_t alias_slash =
        DCC_ON_SLASH("alias", "Alias slash", on_alias_slash);
    dcc_app_extension_slash_command_t alias_command =
        DCC_ON_COMMAND("alias_command", "Alias command", on_alias_command);
    dcc_app_extension_component_route_t alias_button =
        DCC_ON_BUTTON("alias.button", on_alias_button);
    dcc_app_extension_component_route_t alias_select =
        DCC_ON_SELECT("alias.select", on_alias_select);
    dcc_app_extension_component_route_t alias_modal =
        DCC_ON_MODAL("alias.modal", on_alias_modal);
    dcc_app_extension_slash_command_t guarded_slash =
        DCC_ON_SLASH_GUARDED(
            "guarded",
            "Guarded slash",
            on_alias_slash,
            DCC_ONLY_GUILD(),
            DCC_HAS_ROLE(99ULL),
            DCC_RATE_LIMIT_USER(1U, 500U)
        );
    dcc_app_extension_component_route_t guarded_button =
        DCC_ON_BUTTON_GUARDED(
            "guarded.button",
            on_alias_button,
            DCC_ONLY_GUILD(),
            DCC_HAS_ANY_ROLE(100ULL, 101ULL)
        );
    dcc_app_extension_component_route_t guarded_select =
        DCC_ON_SELECT_GUARDED(
            "guarded.select",
            on_alias_select,
            DCC_ONLY_DM()
        );
    dcc_app_extension_component_route_t guarded_modal =
        DCC_ON_MODAL_GUARDED(
            "guarded.modal",
            on_alias_modal,
            DCC_OWNER_ONLY(102ULL)
        );
    dcc_app_extension_event_t member_join = DCC_ON_MEMBER_JOIN_DATA(on_member_join, &state);
    dcc_app_extension_event_t role_update = DCC_ON_ROLE_UPDATE(on_role_update);
    dcc_app_extension_context_menu_t user_menu =
        DCC_ON_USER_CONTEXT_MENU_DATA("Inspect user", on_user_context_menu, &state);
    dcc_app_extension_context_menu_t message_menu =
        DCC_ON_MESSAGE_CONTEXT_MENU_DATA("Inspect message", on_message_context_menu, &state);
    dcc_app_extension_context_menu_t guarded_user_menu =
        DCC_ON_USER_CONTEXT_MENU_GUARDED(
            "Guarded user",
            on_user_context_menu,
            DCC_ONLY_GUILD(),
            DCC_HAS_ROLE(103ULL)
        );
    dcc_app_extension_autocomplete_t autocomplete_fn =
        DCC_ON_AUTOCOMPLETE_FN(on_alias_command);
    dcc_app_extension_autocomplete_t autocomplete_fn_data =
        DCC_ON_AUTOCOMPLETE_FN_DATA(on_alias_slash, &state);
    dcc_app_extension_context_menu_t user_menu_fn =
        DCC_ON_USER_CONTEXT_MENU_FN(on_user_context_menu);
    dcc_app_extension_context_menu_t message_menu_fn =
        DCC_ON_MESSAGE_CONTEXT_MENU_FN_DATA(on_message_context_menu, &state);
    dcc_app_extension_component_route_t button_fn =
        DCC_ON_BUTTON_FN(on_alias_button);
    dcc_app_extension_component_route_t button_fn_data =
        DCC_ON_BUTTON_FN_DATA(on_alias_button, &state);
    dcc_app_extension_component_route_t button_prefix_fn =
        DCC_ON_BUTTON_PREFIX_FN_GUARDED(on_alias_button, DCC_ONLY_GUILD());
    dcc_app_extension_component_route_t select_fn =
        DCC_ON_SELECT_FN(on_alias_select);
    dcc_app_extension_component_route_t select_prefix_fn =
        DCC_ON_SELECT_PREFIX_FN_DATA(on_alias_select, &state);
    dcc_app_extension_component_route_t modal_fn =
        DCC_ON_MODAL_FN(on_alias_modal);
    dcc_app_extension_component_route_t modal_prefix_fn =
        DCC_ON_MODAL_PREFIX_FN_DATA_GUARDED(on_alias_modal, &state, DCC_ONLY_NSFW());
    dcc_app_extension_component_route_t button_ns_fn =
        DCC_ON_BUTTON_NS_FN("fnns", on_alias_button);
    dcc_app_extension_component_route_t button_ns_prefix_fn =
        DCC_ON_BUTTON_NS_PREFIX_FN_DATA_GUARDED("fnns", on_alias_button, &state, DCC_ONLY_GUILD());
    dcc_app_extension_component_route_t select_ns_fn =
        DCC_ON_SELECT_NS_FN_DATA("fnns", on_alias_select, &state);
    dcc_app_extension_component_route_t select_ns_prefix_fn =
        DCC_ON_SELECT_NS_PREFIX_FN_GUARDED("fnns", on_alias_select, DCC_ONLY_DM());
    dcc_app_extension_component_route_t modal_ns_fn =
        DCC_ON_MODAL_NS_FN("fnns", on_alias_modal);
    dcc_app_extension_component_route_t modal_ns_prefix_fn =
        DCC_ON_MODAL_NS_PREFIX_FN_DATA("fnns", on_alias_modal, &state);

    return slash.size == sizeof(slash) &&
        strcmp(slash.name, "on_view_default") == 0 &&
        strcmp(slash.description, "Default handler route") == 0 &&
        slash.handler == on_view_default &&
        slash.user_data == NULL &&
        strcmp(slash_data.name, "on_view_explicit") == 0 &&
        slash_data.handler == on_view_explicit &&
        slash_data.user_data == &state &&
        strcmp(command.name, "on_group_view") == 0 &&
        command.handler == on_group_view &&
        alias_slash.handler == on_alias_slash &&
        alias_command.handler == on_alias_command &&
        alias_button.handler == on_alias_button &&
        alias_select.handler == on_alias_select &&
        alias_modal.handler == on_alias_modal &&
        guarded_slash.policy.guild_only == 1U &&
        guarded_slash.policy.required_role_id_count == 1U &&
        guarded_slash.policy.required_role_ids[0] == 99ULL &&
        guarded_slash.policy.cooldown.bucket == DCC_APP_COOLDOWN_USER &&
        guarded_button.policy.any_role_id_count == 2U &&
        guarded_button.policy.any_role_ids[1] == 101ULL &&
        guarded_select.policy.dm_only == 1U &&
        guarded_modal.policy.owner_user_id_count == 1U &&
        guarded_modal.policy.owner_user_ids[0] == 102ULL &&
        member_join.type == DCC_APP_EXTENSION_EVENT &&
        member_join.event_type == DCC_EVENT_GUILD_MEMBER_ADD &&
        member_join.event_handler == on_member_join_dcc_event_dispatch &&
        member_join.user_data == &state &&
        role_update.type == DCC_APP_EXTENSION_EVENT &&
        role_update.event_type == DCC_EVENT_GUILD_ROLE_UPDATE &&
        role_update.event_handler == on_role_update_dcc_event_dispatch &&
        user_menu.type == DCC_APP_EXTENSION_USER_CONTEXT_MENU &&
        strcmp(user_menu.name, "Inspect user") == 0 &&
        user_menu.handler == on_user_context_menu &&
        user_menu.user_data == &state &&
        message_menu.type == DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU &&
        strcmp(message_menu.name, "Inspect message") == 0 &&
        message_menu.handler == on_message_context_menu &&
        message_menu.user_data == &state &&
        guarded_user_menu.policy.guild_only == 1U &&
        guarded_user_menu.policy.required_role_id_count == 1U &&
        guarded_user_menu.policy.required_role_ids[0] == 103ULL &&
        autocomplete_fn.name != NULL &&
        strcmp(autocomplete_fn.name, "on_alias_command") == 0 &&
        autocomplete_fn.handler == on_alias_command &&
        autocomplete_fn_data.user_data == &state &&
        user_menu_fn.type == DCC_APP_EXTENSION_USER_CONTEXT_MENU &&
        strcmp(user_menu_fn.name, "on_user_context_menu") == 0 &&
        user_menu_fn.handler == on_user_context_menu &&
        message_menu_fn.type == DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU &&
        strcmp(message_menu_fn.name, "on_message_context_menu") == 0 &&
        message_menu_fn.user_data == &state &&
        button_fn.type == DCC_APP_EXTENSION_BUTTON &&
        strcmp(button_fn.custom_id, "on_alias_button") == 0 &&
        button_fn.handler == on_alias_button &&
        button_fn_data.user_data == &state &&
        button_prefix_fn.type == DCC_APP_EXTENSION_BUTTON_PREFIX &&
        strcmp(button_prefix_fn.custom_id, "on_alias_button") == 0 &&
        button_prefix_fn.policy.guild_only == 1U &&
        select_fn.type == DCC_APP_EXTENSION_SELECT &&
        strcmp(select_fn.custom_id, "on_alias_select") == 0 &&
        select_prefix_fn.type == DCC_APP_EXTENSION_SELECT_PREFIX &&
        select_prefix_fn.user_data == &state &&
        modal_fn.type == DCC_APP_EXTENSION_MODAL &&
        strcmp(modal_fn.custom_id, "on_alias_modal") == 0 &&
        modal_prefix_fn.type == DCC_APP_EXTENSION_MODAL_PREFIX &&
        modal_prefix_fn.user_data == &state &&
        modal_prefix_fn.policy.nsfw_only == 1U &&
        button_ns_fn.type == DCC_APP_EXTENSION_BUTTON &&
        strcmp(button_ns_fn.custom_id, "fnns.on_alias_button") == 0 &&
        button_ns_prefix_fn.type == DCC_APP_EXTENSION_BUTTON_PREFIX &&
        strcmp(button_ns_prefix_fn.custom_id, "fnns.on_alias_button") == 0 &&
        button_ns_prefix_fn.user_data == &state &&
        button_ns_prefix_fn.policy.guild_only == 1U &&
        select_ns_fn.type == DCC_APP_EXTENSION_SELECT &&
        strcmp(select_ns_fn.custom_id, "fnns.on_alias_select") == 0 &&
        select_ns_fn.user_data == &state &&
        select_ns_prefix_fn.type == DCC_APP_EXTENSION_SELECT_PREFIX &&
        strcmp(select_ns_prefix_fn.custom_id, "fnns.on_alias_select") == 0 &&
        select_ns_prefix_fn.policy.dm_only == 1U &&
        modal_ns_fn.type == DCC_APP_EXTENSION_MODAL &&
        strcmp(modal_ns_fn.custom_id, "fnns.on_alias_modal") == 0 &&
        modal_ns_prefix_fn.type == DCC_APP_EXTENSION_MODAL_PREFIX &&
        strcmp(modal_ns_prefix_fn.custom_id, "fnns.on_alias_modal") == 0 &&
        modal_ns_prefix_fn.user_data == &state;
}

static int check_ready_once_runtime(void) {
    app_dx_sugar_state_t state = {0};
    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK) {
        return 0;
    }

    int ok =
        DCC_APP_ON_READY_ONCE_DATA(app, on_ready_once, &state) == DCC_OK &&
        DCC_APP_ON_READY_DATA(app, on_ready, &state) == DCC_OK &&
        dcc_app_on_ready_once(NULL, on_ready_once, &state) == DCC_ERR_INVALID_ARG &&
        dispatch_ready(app) == DCC_OK &&
        dispatch_ready(app) == DCC_OK &&
        state.ready_once_count == 1U &&
        state.ready_count == 2U;

    dcc_app_destroy(app);
    return ok;
}

static int check_typed_event_sugar_runtime(void) {
    app_dx_sugar_state_t state = {0};
    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK) {
        return 0;
    }

    dcc_app_definition_t definition =
        DCC_APP(
            "dx.typed.events",
            DCC_APP_EVENTS(
                DCC_ON_ROLE_UPDATE_DATA(on_role_update, &state),
                DCC_ON_TYPING_START_DATA(on_typing_start, &state)
            )
        );

    int ok =
        DCC_APP_ON_MEMBER_JOIN_DATA(app, on_member_join, &state) == DCC_OK &&
        DCC_APP_ON_MEMBER_JOIN(NULL, on_member_join) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_ROLE_UPDATE(NULL, on_role_update) == DCC_ERR_INVALID_ARG &&
        dcc_app_apply(app, &definition) == DCC_OK &&
        dispatch_member_join(app) == DCC_OK &&
        dispatch_role_update(app) == DCC_OK &&
        dispatch_typing_start(app) == DCC_OK &&
        state.member_join_count == 1U &&
        state.role_update_count == 1U &&
        state.typing_start_count == 1U &&
        state.bad == 0U;

    dcc_app_destroy(app);
    return ok;
}

static int check_direct_guarded_runtime(void) {
    app_dx_sugar_state_t state = {0};
    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK) {
        return 0;
    }

    dcc_application_command_builder_t command =
        DCC_SLASH_CMD("guarded_builder", "Guarded builder command");
    int ok =
        DCC_APP_ON_COMMAND(app, "plain_command", "Plain command alias", on_alias_command) == DCC_OK &&
        DCC_APP_ON_COMMAND_DATA(app, "plain_command_data", "Plain command data alias", on_alias_command, &state) ==
            DCC_OK &&
        DCC_APP_ON_COMMAND_FN(app, on_alias_command, "Function-name command alias") == DCC_OK &&
        DCC_APP_ON_COMMAND_FN_DATA(app, on_alias_slash, "Function-name slash data alias", &state) == DCC_OK &&
        DCC_APP_ON_AUTOCOMPLETE_FN(app, on_alias_command) == DCC_OK &&
        DCC_APP_ON_AUTOCOMPLETE_FN_DATA(app, on_alias_slash, &state) == DCC_OK &&
        DCC_APP_ON_USER_CONTEXT_MENU_FN(app, on_user_context_menu) == DCC_OK &&
        DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN(app, on_message_context_menu) == DCC_OK &&
        DCC_APP_ON_BUTTON_FN(app, on_alias_button) == DCC_OK &&
        DCC_APP_ON_BUTTON_PREFIX_FN(app, on_alias_button) == DCC_OK &&
        DCC_APP_ON_SELECT_FN(app, on_alias_select) == DCC_OK &&
        DCC_APP_ON_SELECT_PREFIX_FN(app, on_alias_select) == DCC_OK &&
        DCC_APP_ON_MODAL_FN(app, on_alias_modal) == DCC_OK &&
        DCC_APP_ON_MODAL_PREFIX_FN(app, on_alias_modal) == DCC_OK &&
        DCC_APP_ON_BUTTON_NS_FN(app, "direct_button_ns", on_alias_button) == DCC_OK &&
        DCC_APP_ON_BUTTON_NS_PREFIX_FN(app, "direct_button_ns_prefix", on_alias_button) == DCC_OK &&
        DCC_APP_ON_SELECT_NS_FN(app, "direct_select_ns", on_alias_select) == DCC_OK &&
        DCC_APP_ON_SELECT_NS_PREFIX_FN(app, "direct_select_ns_prefix", on_alias_select) == DCC_OK &&
        DCC_APP_ON_MODAL_NS_FN(app, "direct_modal_ns", on_alias_modal) == DCC_OK &&
        DCC_APP_ON_MODAL_NS_PREFIX_FN(app, "direct_modal_ns_prefix", on_alias_modal) == DCC_OK &&
        DCC_APP_ON_COMMAND_GUARDED(
            app,
            "guarded_command",
            "Guarded command alias",
            on_alias_command,
            DCC_ONLY_GUILD(),
            DCC_CAN(DCC_PERMISSION_MANAGE_MESSAGES),
            DCC_RATE_LIMIT_USER(1U, 500U)
        ) == DCC_OK &&
        DCC_APP_ON_COMMAND_BUILDER_GUARDED(
            app,
            &command,
            on_alias_command,
            DCC_ONLY_GUILD()
        ) == DCC_OK &&
        DCC_APP_ON_SUBCOMMAND_DATA_GUARDED(
            app,
            "guarded_command",
            "admin/reload",
            on_alias_command,
            &state,
            DCC_OWNER_ONLY(42ULL)
        ) == DCC_OK &&
        DCC_APP_ON_AUTOCOMPLETE_GUARDED(
            app,
            "guarded_command",
            on_alias_command,
            DCC_RATE_LIMIT_GLOBAL(2U, 1000U)
        ) == DCC_OK &&
        DCC_APP_TYPED_SLASH(
            app,
            DCC_COMMAND_ROUTE_DATA(
                "direct_typed_inline",
                "Direct typed inline",
                app_dx_direct_args_t,
                on_direct_typed_search,
                &state,
                APP_DX_DIRECT_SLASH_PARAMS
            )
        ) == DCC_OK &&
        DCC_APP_SLASH_ROUTE_FN_DATA(
            app,
            on_direct_typed_lookup,
            "Direct typed function-name",
            app_dx_direct_args_t,
            &state,
            APP_DX_DIRECT_SLASH_PARAMS
        ) == DCC_OK &&
        DCC_APP_SUBCOMMAND_ROUTE_FN_DATA(
            app,
            "direct_typed_inline",
            on_direct_typed_lookup,
            app_dx_direct_args_t,
            &state,
            APP_DX_DIRECT_SLASH_PARAMS
        ) == DCC_OK &&
        DCC_APP_AUTOCOMPLETE_PARAMS_FN_DATA(
            app,
            on_direct_typed_lookup,
            app_dx_direct_args_t,
            &state,
            APP_DX_DIRECT_SLASH_PARAMS
        ) == DCC_OK &&
        DCC_APP_MODAL_NS_PARAMS_FN_DATA(
            app,
            "direct.typed.modal",
            on_direct_typed_modal,
            app_dx_direct_args_t,
            &state,
            APP_DX_DIRECT_MODAL_PARAMS
        ) == DCC_OK &&
        DCC_APP_MODAL_V2_FLOW_NS_PARAMS_FN_DATA(
            app,
            "direct.typed.flow",
            "Direct typed flow",
            app_dx_direct_args_t,
            on_direct_typed_modal,
            &state,
            APP_DX_DIRECT_MODAL_PARAMS,
            DCC_UI_LABEL("Query", DCC_UI_INPUT_NS("direct.typed.flow", "query", "Query", 1U))
        ) == DCC_OK &&
        DCC_APP_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED(
            app,
            "direct.typed.button",
            on_direct_typed_component,
            app_dx_direct_args_t,
            &state,
            APP_DX_DIRECT_COMPONENT_PARAMS,
            DCC_ONLY_GUILD()
        ) == DCC_OK &&
        DCC_APP_SELECT_NS_PARAMS_FN_DATA(
            app,
            "direct.typed.select",
            on_direct_typed_component,
            app_dx_direct_args_t,
            &state,
            APP_DX_DIRECT_COMPONENT_PARAMS
        ) == DCC_OK &&
        DCC_APP_ON_USER_CONTEXT_MENU_DATA_GUARDED(
            app,
            "Direct user menu",
            on_user_context_menu,
            &state,
            DCC_ONLY_GUILD()
        ) == DCC_OK &&
        DCC_APP_ON_MESSAGE_CONTEXT_MENU_GUARDED(
            app,
            "Direct message menu",
            on_message_context_menu,
            DCC_ONLY_GUILD()
        ) == DCC_OK &&
        DCC_APP_ON_BUTTON_PREFIX_DATA_GUARDED(
            app,
            "guarded.button:",
            on_alias_button,
            &state,
            DCC_HAS_ANY_ROLE(43ULL, 44ULL)
        ) == DCC_OK &&
        DCC_APP_ON_SELECT_GUARDED(
            app,
            "guarded.select",
            on_alias_select,
            DCC_ONLY_DM()
        ) == DCC_OK &&
        DCC_APP_ON_MODAL_PREFIX_DATA_GUARDED(
            app,
            "guarded.modal:",
            on_alias_modal,
            &state,
            DCC_ONLY_NSFW()
        ) == DCC_OK &&
        DCC_APP_ON_COMMAND(NULL, "bad", "Bad", on_alias_command) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_COMMAND_GUARDED(NULL, "bad", "Bad", on_alias_command, DCC_ONLY_GUILD()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_TYPED_SLASH(
            NULL,
            DCC_COMMAND_ROUTE_FN(
                on_direct_typed_search,
                "Bad typed slash",
                app_dx_direct_args_t,
                APP_DX_DIRECT_SLASH_PARAMS
            )
        ) == DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_USER_CONTEXT_MENU_GUARDED(NULL, "bad", on_user_context_menu, DCC_ONLY_GUILD()) ==
            DCC_ERR_INVALID_ARG &&
        DCC_APP_ON_BUTTON_GUARDED(NULL, "bad.button", on_alias_button, DCC_HAS_ROLE(43ULL)) ==
            DCC_ERR_INVALID_ARG;

    dcc_app_destroy(app);
    return ok;
}

static int check_context_menu_runtime(void) {
    app_dx_sugar_state_t cog_state = {0};
    app_dx_sugar_state_t direct_state = {0};
    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK) {
        return 0;
    }

    dcc_app_extension_t context_cog =
        DCC_COG(
            "dx.context",
            DCC_COG_CONTEXT_MENUS(
                DCC_ON_USER_CONTEXT_MENU_DATA("Inspect user", on_user_context_menu, &cog_state),
                DCC_ON_MESSAGE_CONTEXT_MENU_DATA("Inspect message", on_message_context_menu, &cog_state)
            )
        );

    int ok =
        context_cog.context_menu_count == 2U &&
        context_cog.context_menus[0].type == DCC_APP_EXTENSION_USER_CONTEXT_MENU &&
        context_cog.context_menus[1].type == DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU &&
        dcc_app_extension_register(app, &context_cog) == DCC_OK &&
        DCC_APP_ON_USER_CONTEXT_MENU_DATA(app, "Direct user", on_user_context_menu, &direct_state) == DCC_OK &&
        DCC_APP_ON_MESSAGE_CONTEXT_MENU_DATA(app, "Direct message", on_message_context_menu, &direct_state) ==
            DCC_OK &&
        dcc_app_user_context_menu(NULL, "bad", on_user_context_menu, &direct_state) == DCC_ERR_INVALID_ARG &&
        dcc_app_message_context_menu(NULL, "bad", on_message_context_menu, &direct_state) == DCC_ERR_INVALID_ARG &&
        dispatch_context_menu(app, DCC_EVENT_USER_CONTEXT_MENU, "Inspect user", 700U) == DCC_OK &&
        dispatch_context_menu(app, DCC_EVENT_MESSAGE_CONTEXT_MENU, "Inspect message", 701U) == DCC_OK &&
        dispatch_context_menu(app, DCC_EVENT_USER_CONTEXT_MENU, "Direct user", 700U) == DCC_OK &&
        dispatch_context_menu(app, DCC_EVENT_MESSAGE_CONTEXT_MENU, "Direct message", 701U) == DCC_OK &&
        cog_state.user_context_count == 1U &&
        cog_state.message_context_count == 1U &&
        direct_state.user_context_count == 1U &&
        direct_state.message_context_count == 1U &&
        cog_state.bad == 0U &&
        direct_state.bad == 0U;

    dcc_app_destroy(app);
    return ok;
}

static int check_view_sugar_runtime(void) {
    app_dx_sugar_state_t state = {0};
    app_dx_sugar_state_t explicit_state = {0};
    app_dx_sugar_state_t group_state = {0};
    dcc_app_view_t view =
        DCC_VIEW_DATA(
            &state,
            DCC_VIEW_BUTTON("dx.view.default", on_view_default),
            DCC_VIEW_MODAL("dx.view.modal", on_view_default),
            DCC_VIEW_BUTTON("dx.view.explicit", on_view_explicit, &explicit_state)
        );
    dcc_app_view_t action_view =
        DCC_VIEW_ACTION_ROUTES_DATA(&state, APP_DX_VIEW_ROUTE_ACTIONS);
    dcc_app_view_t namespace_view =
        DCC_VIEW_DATA(
            &state,
            DCC_VIEW_BUTTON_NS("dx.ns.view", "default", on_view_default),
            DCC_VIEW_BUTTON_NS_DATA("dx.ns.view", "explicit", on_view_explicit, &explicit_state),
            DCC_VIEW_BUTTON_NS_PREFIX("dx.ns.view", "group:", on_view_default),
            DCC_VIEW_SELECT_NS("dx.ns.view", "select", on_view_default),
            DCC_VIEW_SELECT_NS_PREFIX_DATA("dx.ns.view", "select:", on_view_explicit, &explicit_state),
            DCC_VIEW_MODAL_NS("dx.ns.view", "modal", on_view_default),
            DCC_VIEW_MODAL_NS_PREFIX("dx.ns.view", "modal:", on_view_default)
        );
    dcc_app_view_t namespace_action_view =
        DCC_VIEW_ACTION_ROUTES_DATA(&state, APP_DX_NS_VIEW_ACTIONS);

    if (view.user_data != &state ||
        view.item_count != 3U ||
        view.items[0].type != DCC_APP_VIEW_BUTTON ||
        view.items[0].user_data != NULL ||
        view.items[2].user_data != &explicit_state ||
        action_view.user_data != &state ||
        action_view.item_count != 2U ||
        action_view.items[0].type != DCC_APP_VIEW_BUTTON ||
        action_view.items[0].custom_id == NULL ||
        strcmp(action_view.items[0].custom_id, "dx.route.default") != 0 ||
        action_view.items[0].user_data != NULL ||
        action_view.items[1].user_data != &explicit_state ||
        namespace_view.user_data != &state ||
        namespace_view.item_count != 7U ||
        strcmp(namespace_view.items[0].custom_id, "dx.ns.view.default") != 0 ||
        namespace_view.items[1].user_data != &explicit_state ||
        namespace_view.items[2].type != DCC_APP_VIEW_BUTTON_PREFIX ||
        strcmp(namespace_view.items[2].custom_id, "dx.ns.view.group:") != 0 ||
        namespace_view.items[3].type != DCC_APP_VIEW_SELECT ||
        strcmp(namespace_view.items[3].custom_id, "dx.ns.view.select") != 0 ||
        namespace_view.items[4].type != DCC_APP_VIEW_SELECT_PREFIX ||
        strcmp(namespace_view.items[4].custom_id, "dx.ns.view.select:") != 0 ||
        namespace_view.items[5].type != DCC_APP_VIEW_MODAL ||
        strcmp(namespace_view.items[5].custom_id, "dx.ns.view.modal") != 0 ||
        namespace_view.items[6].type != DCC_APP_VIEW_MODAL_PREFIX ||
        strcmp(namespace_view.items[6].custom_id, "dx.ns.view.modal:") != 0 ||
        namespace_action_view.user_data != &state ||
        namespace_action_view.item_count != 2U ||
        strcmp(namespace_action_view.items[0].custom_id, "dx.ns.default") != 0 ||
        namespace_action_view.items[1].user_data != &explicit_state) {
        return 0;
    }

    dcc_message_builder_t action_message =
        DCC_MESSAGE_COMPONENTS_V2(DCC_VIEW_ACTION_ROW(APP_DX_VIEW_ACTIONS));
    char *action_json = NULL;
    if (dcc_message_builder_build_json(&action_message, &action_json) != DCC_OK) {
        return 0;
    }
    int action_json_ok =
        text_contains(action_json, "\"label\":\"Default\"") &&
        text_contains(action_json, "\"custom_id\":\"dx.action.default\"") &&
        text_contains(action_json, "\"style\":1") &&
        text_contains(action_json, "\"label\":\"Explicit\"") &&
        text_contains(action_json, "\"custom_id\":\"dx.action.explicit\"") &&
        text_contains(action_json, "\"style\":4");
    dcc_message_builder_json_free(action_json);
    if (!action_json_ok) {
        return 0;
    }

    dcc_message_builder_t namespace_action_message =
        DCC_MESSAGE_COMPONENTS_V2(DCC_VIEW_ACTION_ROW(APP_DX_NS_VIEW_ACTIONS));
    char *namespace_action_json = NULL;
    if (dcc_message_builder_build_json(&namespace_action_message, &namespace_action_json) != DCC_OK) {
        return 0;
    }
    int namespace_action_json_ok =
        text_contains(namespace_action_json, "\"label\":\"NS Default\"") &&
        text_contains(namespace_action_json, "\"custom_id\":\"dx.ns.default\"") &&
        text_contains(namespace_action_json, "\"style\":1") &&
        text_contains(namespace_action_json, "\"label\":\"NS Explicit\"") &&
        text_contains(namespace_action_json, "\"custom_id\":\"dx.ns.explicit\"") &&
        text_contains(namespace_action_json, "\"style\":4");
    dcc_message_builder_json_free(namespace_action_json);
    if (!namespace_action_json_ok) {
        return 0;
    }

    dcc_app_options_t options = DCC_APP_OPTIONS("token", DCC_INTENTS_DEFAULT);
    dcc_app_t *app = NULL;
    dcc_status_t status = dcc_app_create(&options, &app);
    if (status != DCC_OK) {
        return 0;
    }

    dcc_app_route_group_t group = DCC_GROUP_DATA(app, "dx.group.", &group_state);
    int ok =
        DCC_APP_USE_VIEW_DATA(
            app,
            &state,
            DCC_VIEW_BUTTON("dx.use.default", on_view_default),
            DCC_VIEW_BUTTON("dx.use.explicit", on_view_explicit, &explicit_state)
        ) == DCC_OK &&
        dcc_app_view_register(app, &action_view) == DCC_OK &&
        dcc_app_view_register(app, &namespace_view) == DCC_OK &&
        dcc_app_view_register(app, &namespace_action_view) == DCC_OK &&
        DCC_APP_USE_VIEW_ACTIONS(app, &state, APP_DX_VIEW_ACTIONS) == DCC_OK &&
        DCC_APP_USE_VIEW_ACTIONS(app, &state, APP_DX_NS_USE_ACTIONS) == DCC_OK &&
        DCC_GROUP_USE_VIEW_DATA(&group, &state, DCC_VIEW_BUTTON("view", on_view_default)) == DCC_OK &&
        DCC_GROUP_USE_VIEW(&group, DCC_VIEW_BUTTON("fallback", on_group_view)) == DCC_OK &&
        DCC_APP_USE_VIEW(NULL, DCC_VIEW_BUTTON("bad.view", on_view_default)) == DCC_ERR_INVALID_ARG &&
        DCC_APP_USE_VIEW_ACTIONS(NULL, &state, APP_DX_VIEW_ACTIONS) == DCC_ERR_INVALID_ARG &&
        dispatch_button(app, "dx.use.default") == DCC_OK &&
        dispatch_button(app, "dx.use.explicit") == DCC_OK &&
        dispatch_button(app, "dx.route.default") == DCC_OK &&
        dispatch_button(app, "dx.route.explicit") == DCC_OK &&
        dispatch_button(app, "dx.action.default") == DCC_OK &&
        dispatch_button(app, "dx.action.explicit") == DCC_OK &&
        dispatch_button(app, "dx.ns.view.default") == DCC_OK &&
        dispatch_button(app, "dx.ns.view.explicit") == DCC_OK &&
        dispatch_button(app, "dx.ns.view.group:item") == DCC_OK &&
        dispatch_button(app, "dx.ns.default") == DCC_OK &&
        dispatch_button(app, "dx.ns.explicit") == DCC_OK &&
        dispatch_button(app, "dx.ns.use.default") == DCC_OK &&
        dispatch_button(app, "dx.ns.use.explicit") == DCC_OK &&
        dispatch_button(app, "dx.group.view") == DCC_OK &&
        dispatch_button(app, "dx.group.fallback") == DCC_OK &&
        state.view_count == 8U &&
        explicit_state.explicit_view_count == 6U &&
        group_state.group_view_count == 1U &&
        state.bad == 0U &&
        explicit_state.bad == 0U &&
        group_state.bad == 0U;

    dcc_app_destroy(app);
    return ok;
}

static int check_message_action_sugar(void) {
    dcc_message_builder_t text_actions =
        DCC_MESSAGE_TEXT_ACTIONS(
            "Refresh status",
            DCC_ACTION_ROW(DCC_BUTTON_PRIMARY_BUILDER("Refresh", "dx.refresh"))
        );
    dcc_message_builder_t embed_actions =
        DCC_MESSAGE_TEXT_EMBED_ACTIONS(
            "Current runtime state",
            DCC_EMBED_COLOR("Runtime", "Gateway online", 0x57F287U),
            DCC_ACTION_ROW(
                DCC_BUTTON_PRIMARY_BUILDER("Refresh", "dx.embed.refresh"),
                DCC_BUTTON_DANGER_BUILDER("Close", "dx.embed.close")
            )
        );
    dcc_message_builder_t private_actions =
        DCC_MESSAGE_EPHEMERAL_EMBED_ACTIONS(
            DCC_EMBED("Private status", "Only you can see this."),
            DCC_ACTION_ROW(DCC_BUTTON_SECONDARY_BUILDER("Dismiss", "dx.private.dismiss"))
        );

    return text_actions.has_content == 1U &&
        strcmp(text_actions.content, "Refresh status") == 0 &&
        text_actions.components_count == 1U &&
        text_actions.components[0].children_count == 1U &&
        text_actions.components[0].children[0].style == DCC_BUTTON_PRIMARY &&
        text_actions.has_flags == 0U &&
        embed_actions.has_content == 1U &&
        embed_actions.embeds_count == 1U &&
        embed_actions.embeds[0].has_color == 1U &&
        embed_actions.embeds[0].color == 0x57F287U &&
        embed_actions.components_count == 1U &&
        embed_actions.components[0].children_count == 2U &&
        private_actions.has_flags == 1U &&
        (private_actions.flags & DCC_MESSAGE_FLAG_EPHEMERAL) != 0U &&
        private_actions.embeds_count == 1U &&
        private_actions.components_count == 1U;
}

static int check_modal_field_sugar(void) {
    dcc_modal_builder_t legacy =
        DCC_MODAL(
            "dx.legacy.modal",
            "Legacy modal",
            DCC_MODAL_FIELD_TEXT_PLACEHOLDER("dx.name", "Name", "Display name"),
            DCC_MODAL_FIELD_PARAGRAPH_OPTIONAL("dx.bio", "Bio")
        );
    char *json = NULL;
    if (dcc_modal_builder_build_json(&legacy, &json) != DCC_OK) {
        return 0;
    }
    int ok =
        text_contains(json, "\"custom_id\":\"dx.legacy.modal\"") &&
        text_contains(json, "\"placeholder\":\"Display name\"") &&
        text_contains(json, "\"style\":2") &&
        text_contains(json, "\"required\":false");
    dcc_modal_builder_json_free(json);
    if (!ok) {
        return 0;
    }

    dcc_modal_builder_t modal_v2 =
        DCC_MODAL_V2(
            "dx.v2.modal",
            "V2 modal",
            DCC_MODAL_V2_FIELD_TEXT_PLACEHOLDER("dx.email", "Email", "name@example.com"),
            DCC_MODAL_V2_FIELD_CHECKBOX("dx.public", "Show profile publicly", 1U),
            DCC_MODAL_V2_FIELD_FILE("dx.avatar", "Avatar")
        );
    json = NULL;
    if (dcc_modal_builder_build_json(&modal_v2, &json) != DCC_OK) {
        return 0;
    }
    ok =
        text_contains(json, "\"custom_id\":\"dx.v2.modal\"") &&
        text_contains(json, "\"type\":18") &&
        text_contains(json, "\"component\":{\"type\":4") &&
        text_contains(json, "\"placeholder\":\"name@example.com\"") &&
        text_contains(json, "\"type\":23") &&
        text_contains(json, "\"default\":true") &&
        text_contains(json, "\"type\":19");
    dcc_modal_builder_json_free(json);
    return ok;
}

int main(void) {
    if (!check_env_requirements()) {
        fprintf(stderr, "app dx env sugar smoke failed\n");
        return 1;
    }
    if (!check_env_config_sugar()) {
        fprintf(stderr, "app dx env config sugar smoke failed\n");
        return 1;
    }
    if (!check_preset_macros()) {
        fprintf(stderr, "app dx preset sugar smoke failed\n");
        return 1;
    }
    if (!check_guard_sugar()) {
        fprintf(stderr, "app dx guard sugar smoke failed\n");
        return 1;
    }
    if (!check_handler_sugar()) {
        fprintf(stderr, "app dx handler sugar smoke failed\n");
        return 1;
    }
    if (!check_ready_once_runtime()) {
        fprintf(stderr, "app dx ready once smoke failed\n");
        return 1;
    }
    if (!check_typed_event_sugar_runtime()) {
        fprintf(stderr, "app dx typed event sugar smoke failed\n");
        return 1;
    }
    if (!check_direct_guarded_runtime()) {
        fprintf(stderr, "app dx direct guarded sugar smoke failed\n");
        return 1;
    }
    if (!check_context_menu_runtime()) {
        fprintf(stderr, "app dx context menu sugar smoke failed\n");
        return 1;
    }
    if (!check_view_sugar_runtime()) {
        fprintf(stderr, "app dx view sugar smoke failed\n");
        return 1;
    }
    if (!check_message_action_sugar()) {
        fprintf(stderr, "app dx message action sugar smoke failed\n");
        return 1;
    }
    if (!check_modal_field_sugar()) {
        fprintf(stderr, "app dx modal field sugar smoke failed\n");
        return 1;
    }
    return 0;
}
