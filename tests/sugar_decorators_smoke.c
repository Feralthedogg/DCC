#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

static int decorated_state;

typedef struct decorated_echo_args {
    const char *text;
    int64_t count;
} decorated_echo_args_t;

typedef struct decorated_profile_modal_args {
    const char *display_name;
    uint8_t public_profile;
} decorated_profile_modal_args_t;

typedef struct decorated_button_args {
    const char *action;
} decorated_button_args_t;

typedef struct decorated_select_args {
    const char *selected_role;
} decorated_select_args_t;

#define DECORATED_ECHO_PARAMS(X, T) \
    X(T, REQUIRED_STRING, text, "text", "Text to echo") \
    X(T, INT, count, "count", "Repeat count", 0U, 1)

#define DECORATED_PROFILE_MODAL_PARAMS(X, T) \
    X(T, REQUIRED_STRING, display_name, "display_name") \
    X(T, BOOL, public_profile, "public_profile", 1U)

#define DECORATED_BUTTON_PARAMS(X, T) \
    X(T, REQUIRED_CUSTOM_ID_SUFFIX, action, "status.action.")

#define DECORATED_SELECT_PARAMS(X, T) \
    X(T, REQUIRED_FIRST_VALUE, selected_role)

DCC_DECORATE_SLASH(ping, "Reply with pong") {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_SLASH_PARAMS_DATA_GUARDED(
    echo,
    "Echo text",
    decorated_echo_args_t,
    args,
    &decorated_state,
    DECORATED_ECHO_PARAMS,
    DCC_ROUTE_GUILD_ONLY()
) {
    (void)ctx;
    (void)user_data;
    (void)args;
}

DCC_DECORATE_SLASH_DATA_GUARDED(
    guarded_ping,
    "Guarded pong",
    &decorated_state,
    DCC_ROUTE_GUILD_ONLY(),
    DCC_ROUTE_COOLDOWN_USER(1U, 1000U)
) {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_SLASH_AS(named_ping, "ping-named", "Named pong") {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_SUBCOMMAND(admin_reload, "admin", "tools/reload") {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_AUTOCOMPLETE_AS(search_complete, "search") {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_USER_CONTEXT_MENU(inspect_user) {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS(inspect_message, "Inspect message") {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_BUTTON_NS(refresh_button, "status", "refresh") {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_BUTTON_NS_DATA_GUARDED(
    admin_refresh_button,
    "status",
    "admin-refresh",
    &decorated_state,
    DCC_ROUTE_OWNER(123ULL)
) {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_SELECT_NS_PREFIX(role_select, "roles", "pick.") {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_MODAL_NS(edit_profile, "profile", "edit") {
    (void)ctx;
    (void)user_data;
}

DCC_DECORATE_MODAL_NS_PARAMS_DATA_GUARDED(
    edit_profile_typed,
    "profile",
    "typed-edit",
    decorated_profile_modal_args_t,
    profile,
    &decorated_state,
    DECORATED_PROFILE_MODAL_PARAMS,
    DCC_ROUTE_GUILD_ONLY()
) {
    (void)ctx;
    (void)profile;
    (void)user_data;
}

DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED(
    status_action_button,
    "status",
    "action.",
    decorated_button_args_t,
    button,
    &decorated_state,
    DECORATED_BUTTON_PARAMS,
    DCC_ROUTE_OWNER(123ULL)
) {
    (void)ctx;
    (void)button;
    (void)user_data;
}

DCC_DECORATE_SELECT_NS_PARAMS_DATA_GUARDED(
    role_select_typed,
    "roles",
    "typed-pick",
    decorated_select_args_t,
    select,
    &decorated_state,
    DECORATED_SELECT_PARAMS,
    DCC_ROUTE_GUILD_ONLY()
) {
    (void)ctx;
    (void)select;
    (void)user_data;
}

DCC_DECORATE_READY_ONCE(on_ready) {
    (void)app;
    (void)ready;
    (void)user_data;
}

DCC_DECORATE_READY_ONCE_DATA(on_ready_with_state, &decorated_state) {
    (void)app;
    (void)ready;
    (void)user_data;
}

DCC_DECORATE_EVENT(on_guild_create, DCC_EVENT_GUILD_CREATE) {
    (void)app;
    (void)event;
    (void)user_data;
}

DCC_DECORATE_MEMBER_JOIN(on_typed_member_join) {
    (void)app;
    (void)user_data;
    (void)member;
    (void)event;
}

DCC_DECORATE_MEMBER_JOIN_DATA(on_typed_member_join_with_state, &decorated_state) {
    (void)app;
    (void)user_data;
    (void)member;
    (void)event;
}

DCC_DECORATE_MESSAGE_CREATE(on_message) {
    (void)app;
    (void)message;
    (void)event;
    (void)user_data;
}

DCC_DECORATE_MESSAGE_COMMAND(on_ping_text, "!", "ping") {
    (void)app;
    (void)message;
    (void)args;
    (void)event;
    (void)user_data;
}

DCC_DECORATE_TASK_DAILY_KST(on_midnight, "00:00") {
    (void)app;
    (void)user_data;
}

DCC_DECORATE_TASK_DAILY_KST_DATA(on_state_midnight, "09:30", &decorated_state) {
    (void)app;
    (void)user_data;
}

static int check_slash_routes(void) {
    if (DCC_ROUTE(ping).size != sizeof(dcc_app_extension_slash_command_t) ||
        strcmp(DCC_ROUTE(ping).name, "ping") != 0 ||
        strcmp(DCC_ROUTE(ping).description, "Reply with pong") != 0 ||
        DCC_ROUTE(ping).handler != ping) {
        fprintf(stderr, "DCC_DECORATE_SLASH route mismatch\n");
        return 1;
    }
    if (strcmp(DCC_ROUTE(named_ping).name, "ping-named") != 0 ||
        strcmp(DCC_ROUTE(named_ping).description, "Named pong") != 0 ||
        DCC_ROUTE(named_ping).handler != named_ping) {
        fprintf(stderr, "DCC_DECORATE_SLASH_AS route mismatch\n");
        return 1;
    }
    if (strcmp(DCC_ROUTE(guarded_ping).name, "guarded_ping") != 0 ||
        DCC_ROUTE(guarded_ping).handler != guarded_ping ||
        DCC_ROUTE(guarded_ping).user_data != &decorated_state ||
        DCC_ROUTE(guarded_ping).policy.guild_only != 1U ||
        DCC_ROUTE(guarded_ping).policy.cooldown.bucket != DCC_APP_COOLDOWN_USER ||
        DCC_ROUTE(guarded_ping).policy.cooldown.limit != 1U ||
        DCC_ROUTE(guarded_ping).policy.cooldown.window_ms != 1000U) {
        fprintf(stderr, "DCC_DECORATE_SLASH_DATA_GUARDED route mismatch\n");
        return 1;
    }
    if (DCC_TYPED_ROUTE(echo).size != sizeof(dcc_app_typed_slash_command_t) ||
        DCC_TYPED_ROUTE(echo).command == NULL ||
        strcmp(DCC_TYPED_ROUTE(echo).command->name, "echo") != 0 ||
        strcmp(DCC_TYPED_ROUTE(echo).command->description, "Echo text") != 0 ||
        DCC_TYPED_ROUTE(echo).command->options_count != 2U ||
        DCC_TYPED_ROUTE(echo).command->options[0].type != DCC_APPLICATION_COMMAND_OPTION_STRING ||
        DCC_TYPED_ROUTE(echo).command->options[0].required != 1U ||
        DCC_TYPED_ROUTE(echo).command->options[1].type != DCC_APPLICATION_COMMAND_OPTION_INTEGER ||
        DCC_TYPED_ROUTE(echo).args_size != sizeof(decorated_echo_args_t) ||
        DCC_TYPED_ROUTE(echo).bindings.count != 2U ||
        DCC_TYPED_ROUTE(echo).handler != echo ||
        DCC_TYPED_ROUTE(echo).user_data != &decorated_state ||
        DCC_TYPED_ROUTE(echo).policy.guild_only != 1U) {
        fprintf(stderr, "DCC_DECORATE_SLASH_PARAMS_DATA_GUARDED route mismatch\n");
        return 1;
    }
    if (strcmp(DCC_ROUTE(admin_reload).command_name, "admin") != 0 ||
        strcmp(DCC_ROUTE(admin_reload).subcommand_path, "tools/reload") != 0 ||
        DCC_ROUTE(admin_reload).handler != admin_reload) {
        fprintf(stderr, "DCC_DECORATE_SUBCOMMAND route mismatch\n");
        return 1;
    }
    if (strcmp(DCC_ROUTE(search_complete).name, "search") != 0 ||
        DCC_ROUTE(search_complete).handler != search_complete) {
        fprintf(stderr, "DCC_DECORATE_AUTOCOMPLETE_AS route mismatch\n");
        return 1;
    }
    return 0;
}

static int check_context_and_component_routes(void) {
    if (DCC_ROUTE(inspect_user).type != DCC_APP_EXTENSION_USER_CONTEXT_MENU ||
        strcmp(DCC_ROUTE(inspect_user).name, "inspect_user") != 0 ||
        DCC_ROUTE(inspect_user).handler != inspect_user) {
        fprintf(stderr, "DCC_DECORATE_USER_CONTEXT_MENU route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(inspect_message).type != DCC_APP_EXTENSION_MESSAGE_CONTEXT_MENU ||
        strcmp(DCC_ROUTE(inspect_message).name, "Inspect message") != 0 ||
        DCC_ROUTE(inspect_message).handler != inspect_message) {
        fprintf(stderr, "DCC_DECORATE_MESSAGE_CONTEXT_MENU_AS route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(refresh_button).type != DCC_APP_EXTENSION_BUTTON ||
        strcmp(DCC_ROUTE(refresh_button).custom_id, "status.refresh") != 0 ||
        DCC_ROUTE(refresh_button).handler != refresh_button) {
        fprintf(stderr, "DCC_DECORATE_BUTTON_NS route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(admin_refresh_button).type != DCC_APP_EXTENSION_BUTTON ||
        strcmp(DCC_ROUTE(admin_refresh_button).custom_id, "status.admin-refresh") != 0 ||
        DCC_ROUTE(admin_refresh_button).handler != admin_refresh_button ||
        DCC_ROUTE(admin_refresh_button).user_data != &decorated_state ||
        DCC_ROUTE(admin_refresh_button).policy.owner_user_id_count != 1U ||
        DCC_ROUTE(admin_refresh_button).policy.owner_user_ids[0] != 123ULL) {
        fprintf(stderr, "DCC_DECORATE_BUTTON_NS_DATA_GUARDED route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(role_select).type != DCC_APP_EXTENSION_SELECT_PREFIX ||
        strcmp(DCC_ROUTE(role_select).custom_id, "roles.pick.") != 0 ||
        DCC_ROUTE(role_select).handler != role_select) {
        fprintf(stderr, "DCC_DECORATE_SELECT_NS_PREFIX route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(edit_profile).type != DCC_APP_EXTENSION_MODAL ||
        strcmp(DCC_ROUTE(edit_profile).custom_id, "profile.edit") != 0 ||
        DCC_ROUTE(edit_profile).handler != edit_profile) {
        fprintf(stderr, "DCC_DECORATE_MODAL_NS route mismatch\n");
        return 1;
    }
    if (DCC_TYPED_ROUTE(edit_profile_typed).size != sizeof(dcc_app_typed_modal_t) ||
        strcmp(DCC_TYPED_ROUTE(edit_profile_typed).custom_id, "profile.typed-edit") != 0 ||
        DCC_TYPED_ROUTE(edit_profile_typed).args_size != sizeof(decorated_profile_modal_args_t) ||
        DCC_TYPED_ROUTE(edit_profile_typed).bindings.count != 2U ||
        DCC_TYPED_ROUTE(edit_profile_typed).handler != edit_profile_typed ||
        DCC_TYPED_ROUTE(edit_profile_typed).user_data != &decorated_state ||
        DCC_TYPED_ROUTE(edit_profile_typed).policy.guild_only != 1U) {
        fprintf(stderr, "DCC_DECORATE_MODAL_NS_PARAMS_DATA_GUARDED route mismatch\n");
        return 1;
    }
    if (DCC_TYPED_ROUTE(status_action_button).size != sizeof(dcc_app_typed_component_t) ||
        strcmp(DCC_TYPED_ROUTE(status_action_button).custom_id, "status.action.") != 0 ||
        DCC_TYPED_ROUTE(status_action_button).prefix != 1U ||
        DCC_TYPED_ROUTE(status_action_button).args_size != sizeof(decorated_button_args_t) ||
        DCC_TYPED_ROUTE(status_action_button).bindings.count != 1U ||
        DCC_TYPED_ROUTE(status_action_button).handler != status_action_button ||
        DCC_TYPED_ROUTE(status_action_button).user_data != &decorated_state ||
        DCC_TYPED_ROUTE(status_action_button).policy.owner_user_id_count != 1U ||
        DCC_TYPED_ROUTE(status_action_button).policy.owner_user_ids[0] != 123ULL) {
        fprintf(stderr, "DCC_DECORATE_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED route mismatch\n");
        return 1;
    }
    if (DCC_TYPED_ROUTE(role_select_typed).size != sizeof(dcc_app_typed_component_t) ||
        strcmp(DCC_TYPED_ROUTE(role_select_typed).custom_id, "roles.typed-pick") != 0 ||
        DCC_TYPED_ROUTE(role_select_typed).args_size != sizeof(decorated_select_args_t) ||
        DCC_TYPED_ROUTE(role_select_typed).bindings.count != 1U ||
        DCC_TYPED_ROUTE(role_select_typed).handler != role_select_typed ||
        DCC_TYPED_ROUTE(role_select_typed).policy.guild_only != 1U) {
        fprintf(stderr, "DCC_DECORATE_SELECT_NS_PARAMS_DATA_GUARDED route mismatch\n");
        return 1;
    }
    return 0;
}

static int check_event_and_task_routes(void) {
    if (DCC_ROUTE(on_ready).type != DCC_APP_EXTENSION_READY ||
        DCC_ROUTE(on_ready).ready_handler != on_ready ||
        DCC_ROUTE(on_ready).once != 1U) {
        fprintf(stderr, "DCC_DECORATE_READY_ONCE route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(on_ready_with_state).type != DCC_APP_EXTENSION_READY ||
        DCC_ROUTE(on_ready_with_state).ready_handler != on_ready_with_state ||
        DCC_ROUTE(on_ready_with_state).user_data != &decorated_state ||
        DCC_ROUTE(on_ready_with_state).once != 1U) {
        fprintf(stderr, "DCC_DECORATE_READY_ONCE_DATA route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(on_guild_create).type != DCC_APP_EXTENSION_EVENT ||
        DCC_ROUTE(on_guild_create).event_type != DCC_EVENT_GUILD_CREATE ||
        DCC_ROUTE(on_guild_create).event_handler != on_guild_create) {
        fprintf(stderr, "DCC_DECORATE_EVENT route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(on_typed_member_join).type != DCC_APP_EXTENSION_EVENT ||
        DCC_ROUTE(on_typed_member_join).event_type != DCC_EVENT_GUILD_MEMBER_ADD ||
        DCC_ROUTE(on_typed_member_join).event_handler == NULL ||
        DCC_ROUTE(on_typed_member_join).event_handler == on_guild_create) {
        fprintf(stderr, "DCC_DECORATE_MEMBER_JOIN route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(on_typed_member_join_with_state).type != DCC_APP_EXTENSION_EVENT ||
        DCC_ROUTE(on_typed_member_join_with_state).event_type != DCC_EVENT_GUILD_MEMBER_ADD ||
        DCC_ROUTE(on_typed_member_join_with_state).event_handler == NULL ||
        DCC_ROUTE(on_typed_member_join_with_state).user_data != &decorated_state) {
        fprintf(stderr, "DCC_DECORATE_MEMBER_JOIN_DATA route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(on_message).type != DCC_APP_EXTENSION_MESSAGE_CREATE ||
        DCC_ROUTE(on_message).message_handler != on_message) {
        fprintf(stderr, "DCC_DECORATE_MESSAGE_CREATE route mismatch\n");
        return 1;
    }
    if (strcmp(DCC_ROUTE(on_ping_text).prefix, "!") != 0 ||
        strcmp(DCC_ROUTE(on_ping_text).name, "ping") != 0 ||
        DCC_ROUTE(on_ping_text).handler != on_ping_text) {
        fprintf(stderr, "DCC_DECORATE_MESSAGE_COMMAND route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(on_midnight).type != DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST ||
        strcmp(DCC_ROUTE(on_midnight).hhmm, "00:00") != 0 ||
        DCC_ROUTE(on_midnight).handler != on_midnight) {
        fprintf(stderr, "DCC_DECORATE_TASK_DAILY_KST route mismatch\n");
        return 1;
    }
    if (DCC_ROUTE(on_state_midnight).type != DCC_APP_EXTENSION_TASK_EVERY_DAY_AT_KST ||
        strcmp(DCC_ROUTE(on_state_midnight).hhmm, "09:30") != 0 ||
        DCC_ROUTE(on_state_midnight).handler != on_state_midnight ||
        DCC_ROUTE(on_state_midnight).user_data != &decorated_state) {
        fprintf(stderr, "DCC_DECORATE_TASK_DAILY_KST_DATA route mismatch\n");
        return 1;
    }
    return 0;
}

static int check_cog_composition(void) {
    dcc_app_extension_t cog =
        DCC_COG(
            "decorated",
            DCC_COG_SLASHES(DCC_ROUTE(ping), DCC_ROUTE(named_ping), DCC_ROUTE(guarded_ping)),
            DCC_COG_TYPED_SLASHES(DCC_TYPED_ROUTE(echo)),
            DCC_COG_SUBCOMMANDS(DCC_ROUTE(admin_reload)),
            DCC_COG_AUTOCOMPLETES(DCC_ROUTE(search_complete)),
            DCC_COG_CONTEXT_MENUS(DCC_ROUTE(inspect_user), DCC_ROUTE(inspect_message)),
            DCC_COG_COMPONENTS(
                DCC_ROUTE(refresh_button),
                DCC_ROUTE(admin_refresh_button),
                DCC_ROUTE(role_select),
                DCC_ROUTE(edit_profile)
            ),
            DCC_COG_TYPED_MODALS(DCC_TYPED_ROUTE(edit_profile_typed)),
            DCC_COG_TYPED_BUTTONS(DCC_TYPED_ROUTE(status_action_button)),
            DCC_COG_TYPED_SELECTS(DCC_TYPED_ROUTE(role_select_typed)),
            DCC_COG_EVENTS(
                DCC_ROUTE(on_ready),
                DCC_ROUTE(on_ready_with_state),
                DCC_ROUTE(on_guild_create),
                DCC_ROUTE(on_typed_member_join),
                DCC_ROUTE(on_typed_member_join_with_state),
                DCC_ROUTE(on_message)
            ),
            DCC_COG_MESSAGE_COMMANDS(DCC_ROUTE(on_ping_text)),
            DCC_COG_TASKS(DCC_ROUTE(on_midnight), DCC_ROUTE(on_state_midnight))
        );

    if (strcmp(cog.name, "decorated") != 0 ||
        cog.slash_command_count != 3U ||
        cog.typed_slash_command_count != 1U ||
        cog.subcommand_count != 1U ||
        cog.autocomplete_count != 1U ||
        cog.context_menu_count != 2U ||
        cog.component_route_count != 4U ||
        cog.typed_modal_count != 1U ||
        cog.typed_button_count != 1U ||
        cog.typed_select_count != 1U ||
        cog.event_count != 6U ||
        cog.message_command_count != 1U ||
        cog.task_count != 2U) {
        fprintf(stderr, "decorated cog composition mismatch\n");
        return 1;
    }
    if (cog.slash_commands[0].handler != ping ||
        cog.typed_slash_commands[0].handler != echo ||
        cog.component_routes[0].handler != refresh_button ||
        cog.typed_modals[0].handler != edit_profile_typed ||
        cog.typed_buttons[0].handler != status_action_button ||
        cog.typed_selects[0].handler != role_select_typed ||
        cog.events[0].ready_handler != on_ready ||
        cog.message_commands[0].handler != on_ping_text ||
        cog.tasks[0].handler != on_midnight) {
        fprintf(stderr, "decorated cog route arrays mismatch\n");
        return 1;
    }
    return 0;
}

int main(void) {
    if (check_slash_routes() != 0 ||
        check_context_and_component_routes() != 0 ||
        check_event_and_task_routes() != 0 ||
        check_cog_composition() != 0) {
        return 1;
    }
    return 0;
}
