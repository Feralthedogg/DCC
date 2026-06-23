# App Runtime

`dcc_app_t` is the high-level bot surface for application code. It keeps the
gateway client, slash command registry, component routing, interaction context,
and scheduled tasks behind one object.

Use it when you want code shaped more like discord.js or discord.py:

Generated projects use the same callback sugar shown below: `DCC_HANDLER` for
plain interaction handlers, `DCC_COMMAND_ARGS_IMPL(name, args_t, command)`,
`DCC_BUTTON_ARGS_IMPL(name, args_t, button)`, and the matching modal/select
forms for typed routes, `DCC_READY_FN`/`DCC_EVENT_FN` for gateway events, and
`DCC_TASK_FN` for scheduled background work. The lower-level
`DCC_TYPED_HANDLER` plus `DCC_TYPED_ARGS(...)` pattern remains available when a
hand-written handler needs direct access to the raw `void *args` payload.

```sh
dcc_new_app mybot
dcc_new_app add cog mybot profile
dcc_new_app add command mybot profile greet --guild-only --cooldown-user 1:5000
dcc_new_app add subcommand mybot profile greet stats
dcc_new_app add button mybot profile refresh
dcc_new_app add select mybot profile role
dcc_new_app add modal mybot profile edit
dcc_new_app add autocomplete mybot profile profile --guild-only
dcc_new_app add context-menu mybot profile inspect_user user --guild-only
dcc_new_app add context-menu mybot profile quote_message message
dcc_new_app add middleware mybot profile require_config config
dcc_new_app add event mybot profile ready
dcc_new_app add view mybot profile dashboard
dcc_new_app add config mybot log_channel channel LOG_CHANNEL
dcc_new_app add error-handler mybot bot_error friendly
dcc_new_app add latest-message mybot profile status log_channel
dcc_new_app add scheduled-latest mybot profile daily_status log_channel --daily-kst 09:00
dcc_new_app add message-command mybot profile hello --prefix '!'
dcc_new_app add task mybot profile heartbeat --every-seconds 60
dcc_new_app add action mybot profile gateway_status gateway --guild-only
dcc_new_app add action mybot profile dm_user dm --guild-only
dcc_new_app add action mybot profile grant_role role-add --guild-only --permission DCC_PERMISSION_MANAGE_ROLES
dcc_new_app add preset mybot profile announcement --guild-only
dcc_new_app add preset mybot profile confirm --guild-only
dcc_new_app add preset mybot profile paginator
dcc_new_app add preset mybot profile form --owner 803177060956766231ULL
dcc_new_app add preset mybot profile settings --role 123456789012345678ULL
dcc_new_app add preset mybot profile wizard --cooldown-user 1:1000
dcc_new_app add preset mybot profile counter --cooldown-user 5:10000
dcc_new_app add preset mybot profile crud --permission DCC_PERMISSION_MANAGE_GUILD
dcc_new_app add preset mybot profile help
dcc_new_app add preset mybot profile menu --dm-only
dcc_new_app add preset mybot profile poll --cooldown-user 3:10000
dcc_new_app add preset mybot profile profile --name profile_card
dcc_new_app add preset mybot profile roles --guild-only
dcc_new_app add preset mybot profile ticket --guild-only
dcc_new_app add preset mybot profile welcome
dcc_new_app add preset mybot profile flow --guild-only
```

```c
#include <dcc/sugar.h>

DCC_SLASH_FN(ping) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_READY_FN(on_ready) {
    (void)app;
    (void)ready;
    (void)user_data;
}

DCC_SIMPLE_BOT_MAIN(
    "ping-bot",
    DCC_LISTEN_SLASH("ping", "Reply with pong", ping),
    DCC_LISTEN_READY_ONCE(on_ready)
)
```

`DCC_SIMPLE_BOT_MAIN()` creates `main()`, reads `.env` when it exists, creates
the app from environment variables, runs it, and destroys it before returning.
Use `DCC_RUN_SIMPLE_BOT()` when you want to keep your own `main()`.
`dcc_app_load_dotenv()` reads `.env` into the process environment when you want
to do those steps manually.
`dcc_app_options_from_env()` then reads `DCC_TOKEN`, `BOT_TOKEN`, or `DISCORD_TOKEN`,
`DCC_APPLICATION_ID`, `DCC_INTENTS`, `DCC_COMMAND_GUILD_ID`,
`DCC_COMMAND_SYNC`, `DCC_COMMAND_SYNC_DELETE_STALE`, `DCC_AUTO_DEFER_MS`,
`DCC_AUTO_DEFER_EPHEMERAL`, `DCC_STORE_FILE`, `DCC_APP_STORE_FILE`,
`DCC_ENABLE_CACHE`, and
`DCC_INFER_GUILD_FROM_CHANNEL`.
`DCC_INTENTS` accepts a decimal mask, a `0x...` mask, `all`, `none`,
`default`, `messages`, `message_content`, `privileged`, `unprivileged`, or a
comma/pipe/plus separated composition such as `guilds,message_content,voice`.
When only a channel ID is configured, enable `DCC_INFER_GUILD_FROM_CHANNEL`
and call `DCC_INFER_GUILD_FROM_CHANNEL(app, channel_id, on_guild, user_data)`.
DCC uses the channel cache first and falls back to Get Channel when the mapping
is missing.

When a handler needs small persistent state from `DCC_STORE_FILE`, prefer the
context-first helpers:

```c
DCC_CTX_REQUIRE_STORE(ctx, "App store is not configured.");
uint64_t count = DCC_CTX_STORE_U64_OR(ctx, "ping.count", 0U) + 1U;
DCC_CTX_STORE_SET_U64(ctx, "ping.count", count);
```

For feature-specific configuration, prefer typed env binding over manual
`getenv()` parsing:

```sh
dcc_new_app add config . log_channel channel LOG_CHANNEL
dcc_new_app add config . refresh_seconds u64 REFRESH_SECONDS
```

The generator updates `src/config.h`, the `DCC_CONFIG(...)` call, and
`.env.example`. Generated feature files include `config.h`, so a handler can read
`BOT_CONFIG(user_data)->log_channel` or `BOT_CTX_CONFIG(ctx)->log_channel`
without rebuilding the config struct. If you are not using generated markers,
write the same schema manually:

```c
typedef struct bot_config {
    dcc_snowflake_t log_channel;
    dcc_snowflake_t mod_role;
    uint8_t debug;
} bot_config_t;

bot_config_t config = {0};
dcc_status_t status =
    DCC_CONFIG(
        DCC_CONFIG_CHANNEL("LOG_CHANNEL", &config.log_channel),
        DCC_CONFIG_ROLE("MOD_ROLE", &config.mod_role),
        DCC_CONFIG_BOOL_DEFAULT("DEBUG", 0U, &config.debug)
    );
```

The typed helpers reject malformed IDs and numbers before the app starts doing
REST calls. Channel, role, and user config accepts either raw IDs or the
matching Discord mention copied from the client; guild and generic snowflake
config stays raw-ID only. For one-off reads, use `DCC_ENV_TOKEN`,
`DCC_ENV_CHANNEL`, `DCC_ENV_ROLE`, `DCC_ENV_USER`, `DCC_ENV_BOOL`,
`DCC_ENV_U64`, `DCC_ENV_U32_RANGE_OR`, `DCC_ENV_INTENTS_OR`, or their `_OR` /
named-token forms.

## Response State

`dcc_ctx_t` tracks the interaction response state, including app-level
auto-defer. This is useful when a handler may reply immediately, edit a deferred
original response, or send a followup depending on work that already happened.

```c
static void on_work(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    if (DCC_CTX_DEFERRED(ctx)) {
        DCC_EDIT(ctx, DCC_MESSAGE_TEXT("Finished after defer."));
        return;
    }

    if (!DCC_CTX_RESPONDED(ctx)) {
        DCC_REPLY_TEXT(ctx, "Finished immediately.");
        return;
    }

    if (DCC_CTX_CAN_FOLLOWUP(ctx)) {
        DCC_FOLLOWUP_TEXT(ctx, "Additional result.");
    }
}
```

Use `DCC_CTX_RESPONSE_STATE(ctx)` when you need the exact
`dcc_interaction_flow_state_t`, or `DCC_CTX_RESPONSE_STATE_NAME(ctx)` for logs.
Use `DCC_FLOW_*` when a standalone `dcc_interaction_flow_t` should follow the
same reply/defer/edit/followup pattern outside an app context.

```c
dcc_interaction_flow_t flow =
    DCC_FLOW_AUTO_DEFER_EPHEMERAL_STARTED(client, interaction, started_at_ms, DCC_MS(1500U));

DCC_FLOW_MAYBE_AUTO_DEFER(&flow, now_ms);
DCC_FLOW_REPLY_TEXT(&flow, "Done.");
```

## App State And Modules

Use app state for process-wide bot data that many handlers need. `dcc_ctx_t`
can read it directly, so route-specific `user_data` can stay focused on the
module that owns the handler.

```c
typedef struct bot_state {
    unsigned request_count;
} bot_state_t;

DCC_SLASH_FN(on_ping) {
    (void)user_data;

    bot_state_t *state = DCC_CTX_STATE(bot_state_t, ctx);
    if (state != NULL) {
        state->request_count++;
    }
    DCC_REPLY_DONE(ctx, "pong");
}

bot_state_t state = {0};
dcc_app_set_state(app, &state, NULL);
```

Modules are small setup functions that register a feature's listeners, events,
and scheduled tasks together. This gives C code a feature-module shape without
requiring a separate framework.

```c
DCC_DEFINE_LISTENERS(
    ping,
    DCC_LISTEN_SLASH("ping", "Reply with pong", on_ping)
)

dcc_app_module_t ping_module = DCC_LISTENERS_MODULE(ping, NULL);

dcc_app_module_register(app, &ping_module);
```

Use `DCC_APP_MODULE_WITH_CLEANUP()` when the module owns heap state that should
be released with the app.

For simpler feature files, use `DCC_DEFINE_FEATURE`. It generates the setup
function for you and registers a declarative extension inside that setup call.

```c
DCC_SLASH_FN(on_ping) {
    (void)user_data;
    DCC_REPLY_TEXT(ctx, "pong");
}

DCC_READY_FN(on_ready) {
    (void)app;
    (void)user_data;
    printf("ready shard=%u/%u\n", ready->shard_id, ready->shard_count);
}

DCC_DEFINE_FEATURE(
    ping,
    "ping",
    DCC_FEATURE_COMMANDS(DCC_COMMAND_DATA("ping", "Reply with pong", on_ping, user_data)),
    DCC_FEATURE_COMPONENTS(DCC_BUTTON_DATA("ping.refresh", on_ping, user_data)),
    DCC_FEATURE_EVENTS(DCC_READY_DATA(on_ready, user_data))
)

dcc_app_module_t ping_module = DCC_USE_FEATURE(ping, &state);
dcc_app_module_register(app, &ping_module);
```

Use `DCC_DEFINE_PUBLIC_FEATURE` plus `DCC_DECLARE_FEATURE` when a feature lives in a separate
translation unit from `main.c`. Use `DCC_LOAD_FEATURE_DATA(app, ping, &state)` when
you are manually wiring an already-created app instead of putting the feature in
`DCC_APP_USE(...)`.

Typed routes can be grouped into the same feature file:

```c
DCC_DEFINE_FEATURE(
    profile,
    "profile",
    DCC_FEATURE_COMMAND_ROUTES(profile_command),
    DCC_FEATURE_MODAL_ROUTES(profile_modal),
    DCC_FEATURE_BUTTON_ROUTES(profile_button),
    DCC_FEATURE_SELECT_ROUTES(profile_select)
)
```

For typed component routes, namespace function-name aliases such as
`DCC_BUTTON_NS_PARAMS_FN_DATA("profile", save, save_args_t, state, SAVE_PARAMS)`
keep the `custom_id` action tied to the handler name.
For typed slash commands and autocomplete routes, use
`DCC_COMMAND_ROUTE_FN_DATA(search, "Search members", search_args_t, state, SEARCH_PARAMS)`
and `DCC_AUTOCOMPLETE_PARAMS_FN_DATA(search, search_args_t, state, SEARCH_PARAMS)`
when the command name should match the handler name.
If you already have a `dcc_app_t *`, register the same route without a temporary
descriptor:

```c
DCC_APP_SLASH_ROUTE_FN_DATA(app, search, "Search members", search_args_t, state, SEARCH_PARAMS);
DCC_APP_AUTOCOMPLETE_PARAMS_FN_DATA(app, search, search_args_t, state, SEARCH_PARAMS);
DCC_APP_BUTTON_NS_PREFIX_PARAMS_FN_DATA_GUARDED(
    app,
    "profile",
    save,
    save_args_t,
    state,
    SAVE_PARAMS,
    DCC_ONLY_GUILD()
);
```

Use a plain module setup function when the feature needs custom allocation,
dependency wiring, or cleanup. Use a raw `dcc_app_extension_t` object when you
are composing generated route descriptors in the same block.

For app entrypoints, group extensions, typed routes, modules, app-wide
middleware, guards, auto-defer, default error replies, and command sync into a
single definition. `DCC_BOT(...)` is the short definition form for normal
executables because it includes `DCC_APP_DEV_MODE()`: command auto-sync,
ephemeral auto-defer after 1500ms, and default error replies. Use
`DCC_GUILD_BOT(name, guild_id, ...)` for the same handler-side defaults with
guild command sync, and use `DCC_REQUIRE_ENV(...)` when a bot should fail fast
on missing deployment variables. `DCC_APP(...)` remains available when you want
to choose every app option explicitly. `dcc_app_apply()` adds the definition to
an existing app, and `dcc_app_create_defined()` creates and applies it in one
call.

```c
dcc_app_definition_t bot =
    DCC_BOT(
        "mybot",
        DCC_APP_USE(DCC_USE_FEATURE(ping, &state), database_module),
        DCC_APP_COMMAND_ROUTES(profile_command),
        DCC_APP_MODAL_ROUTES(profile_modal),
        DCC_APP_BUTTON_ROUTES(page_button),
        DCC_APP_SELECT_ROUTES(role_select),
        DCC_APP_EVENTS(DCC_ON_READY_ONCE_DATA(on_ready, &state)),
        DCC_APP_MIDDLEWARE(DCC_EXTENSION_MIDDLEWARE(require_ready, &state)),
        DCC_APP_GUILD_ONLY(),
        DCC_APP_OWNERS(owner_user_id, backup_owner_user_id),
        DCC_APP_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_MESSAGES)
    );

dcc_app_t *app = NULL;
dcc_app_create_defined(&options, &bot, &app);
```

When configuring the runtime object directly, `DCC_APP_OPTIONS_DEV()` and
`DCC_APP_OPTIONS_DEV_GUILD(token, guild_id)` turn on cache-backed guild
inference, auto-defer, and command sync in the options layer:

```c
dcc_app_options_t options =
    DCC_APP_OPTIONS_DEV_GUILD(token, dev_guild_id);
```

Use `DCC_APP_EVENTS(...)` for app-wide Gateway callbacks that do not need a
separate feature file. Use `DCC_FEATURE_EVENTS(...)` when the event belongs to a feature
module.

For common Gateway payloads, use typed event sugar instead of manually calling
`dcc_event_member()`, `dcc_event_role()`, or similar accessors in every handler:

```c
DCC_MEMBER_JOIN_FN(on_member_join) {
    (void)app;
    (void)event;

    bot_state_t *state = user_data;
    DCC_APP_SENDF(
        state->app,
        state->log_channel,
        "Welcome %s",
        DCC_MENTION_USER(member->user.id)
    );
}

dcc_app_definition_t bot =
    DCC_APP(
        "bot",
        DCC_APP_EVENTS(DCC_MEMBER_JOIN_DATA(on_member_join, &state))
    );
```

Use `DCC_APP_DM_ONLY()` instead of `DCC_APP_GUILD_ONLY()` when the whole app
definition should only handle direct-message interactions.
Use `DCC_APP_NSFW_ONLY()` when every interaction should come from a cached
age-restricted channel.

For the common `.env` executable case, run the same definition directly:

```c
dcc_status_t status = DCC_RUN_APP(bot);
```

The arrays hidden by `DCC_APP_*` are C compound literals. Apply or create the app
in the same block where the definition is declared. Use explicit arrays and fill
`dcc_app_definition_t` fields manually when a definition needs static storage or
is generated dynamically.

## Routes

`DCC_APP_ON_SLASH()` registers both the local slash command definition and the
runtime handler on an already-created app. Component routes can match exact
custom IDs or prefixes.

```c
DCC_APP_ON_SLASH_DATA(app, "rank", "Show server rank", on_rank, state);
DCC_APP_ON_BUTTON_DATA(app, "settings.save", on_save, state);
DCC_APP_ON_BUTTON_PREFIX_DATA(app, "page:", on_page, state);
DCC_APP_ON_MODAL_DATA(app, "birthday.register", on_birthday_modal, state);
```

Subcommands can be routed directly. Use `group/subcommand` when the command has
a subcommand group.

```c
dcc_app_subcommand(app, "admin", "moderation/ban", on_ban, state);

static void on_ban(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    const char *reason = DCC_OPTION_STRING_OR(ctx, "reason", "No reason");
    char path[64];
    dcc_ctx_subcommand_path(ctx, path, sizeof(path));
    DCC_REPLY_EPHEMERAL(ctx, reason);
}
```

For struct-bound subcommands, use a typed subcommand route. The command builder
is optional; include it when this route should also register the parent Discord
command with the command registry.

```c
typedef struct ban_args {
    const char *reason;
    int64_t days;
} ban_args_t;

static void on_ban_typed(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)user_data;

    const ban_args_t *ban = args;
    (void)ban;
    DCC_REPLY_DONE(ctx, "Ban queued.");
}

dcc_application_command_builder_t admin_command =
    DCC_SLASH_GROUP(
        "admin",
        "Admin tools",
        DCC_CMD_GROUP(
            "moderation",
            "Moderation",
            DCC_CMD_SUB(
                "ban",
                "Ban a member",
                DCC_OPTIONAL_STRING("reason", "Reason"),
                DCC_OPTIONAL_INT_RANGE("days", "Delete message days", 0, 7)
            )
        )
    );

dcc_subcommand_route_t ban =
    DCC_TYPED_SUBCOMMAND_BUILDER(
        &admin_command,
        "moderation/ban",
        ban_args_t,
        on_ban_typed,
        state,
        DCC_ARGS(
            DCC_ARG_STRING(ban_args_t, reason, "reason", "No reason"),
            DCC_ARG_INT(ban_args_t, days, "days", 0)
        ),
        DCC_VALIDATORS(
            DCC_VALIDATE_INT_RANGE(ban_args_t, days, "days", 0, 7)
        )
    );

DCC_APP_REGISTER_SUBCOMMAND(app, ban);
```

Typed option binding can also resolve Discord objects directly:

```c
typedef struct profile_args {
    const dcc_user_t *user;
    const dcc_member_t *member;
} profile_args_t;

dcc_command_route_t profile =
    DCC_ON_TYPED_SLASH(
        "profile",
        "Show a profile",
        profile_args_t,
        on_profile,
        DCC_ARGS(
            DCC_ARG_REQUIRED_USER(profile_args_t, user, "user"),
            DCC_ARG_MEMBER(profile_args_t, member, "user")
        ),
        DCC_NO_VALIDATORS(),
        DCC_OPT_USER("user", "User", 1U)
    );
```

The object pointers are borrowed from the interaction resolved payload or the
enabled client cache.

For component-heavy messages, group routes with a view object:

```c
DCC_APP_USE_VIEW_DATA(
    app,
    state,
    DCC_VIEW_BUTTON("settings.save", on_save),
    DCC_VIEW_BUTTON_PREFIX("page:", on_page),
    DCC_VIEW_MODAL("settings.modal", on_settings_modal)
);
```

Declarative apps can register persistent static custom IDs without a setup
call:

```c
dcc_app_definition_t bot =
    DCC_APP(
        "settings-bot",
        DCC_APP_VIEWS(
            DCC_PERSISTENT_VIEW_DATA(
                state,
                DCC_VIEW_BUTTON("settings.save", on_save),
                DCC_VIEW_MODAL("settings.modal", on_settings_modal)
            )
        )
    );
```

When several custom IDs share one namespace, register them through a route
group. The group prefixes every item, so the handler keys stay local to that
feature module.

```c
dcc_app_route_group_t settings =
    DCC_GROUP_DATA(app, "settings:", state);

DCC_GROUP_BUTTON(&settings, "save", on_save);
DCC_GROUP_SELECT(&settings, "mode", on_mode_select);
DCC_GROUP_MODAL(&settings, "edit", on_edit_modal);

DCC_GROUP_USE_VIEW(
    &settings,
    DCC_VIEW_BUTTON("open", on_open),
    DCC_VIEW_BUTTON_PREFIX("page:", on_page)
);
DCC_GROUP_REQUIRED_PERMISSIONS(&settings, DCC_PERMISSION_MANAGE_GUILD);
```

For button-only panels, keep the UI and route registration on the same action
list:

```c
#define SETTINGS_ACTIONS \
    DCC_VIEW_ACTION_PRIMARY("Refresh", "settings.refresh", on_settings_button), \
    DCC_VIEW_ACTION_SECONDARY("Edit", "settings.edit", on_settings_button)

DCC_APP_USE_VIEW_ACTIONS(app, state, SETTINGS_ACTIONS);
/* Or: DCC_FEATURE_VIEWS(DCC_VIEW_ACTION_ROUTES_DATA(state, SETTINGS_ACTIONS)) */
DCC_RESPOND_UI(ctx, DCC_UI_CARD(DCC_UI_TEXT("## Settings"), DCC_VIEW_ACTION_ROW(SETTINGS_ACTIONS)));
```

`DCC_VIEW_DATA()` and `DCC_PERSISTENT_VIEW_DATA()` bind one default state pointer
for the whole View. An item-level third `DCC_VIEW_*` argument overrides that
default when it is non-NULL. Route groups use the item state first, then the View
state, then the group state. Call `DCC_GROUP_USE()`, `DCC_GROUP_OWNER()`, or
`DCC_GROUP_POLICY()` after registering the group routes when every handler in
the namespace should share the same policy.
Use `dcc_new_app add view . profile dashboard` to generate a `/dashboard`
Components v2 opener plus a reusable `*_ACTIONS` list that powers both the
button row and persistent View routes inside a Cog. The generated opener sends a ready-to-edit panel with Refresh and Edit
buttons, so the route and the UI custom IDs start in sync.

```c
dcc_app_route_policy_t settings_policy =
    DCC_ROUTE_POLICY(
        DCC_ROUTE_GUILD_ONLY(),
        DCC_ROUTE_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_GUILD),
        DCC_ROUTE_COOLDOWN_USER(3U, DCC_SECONDS(10U))
    );

DCC_GROUP_POLICY(&settings, settings_policy);
```

When a route needs its own middleware or guard, use the `_route` variant and
keep the returned route ID:

```c
dcc_app_route_id_t admin_route = DCC_APP_ROUTE_INVALID;

dcc_app_slash_route(
    app,
    "admin",
    "Admin-only tools",
    on_admin,
    state,
    &admin_route
);

dcc_app_route_use_required_permissions(
    app,
    admin_route,
    DCC_PERMISSION_MANAGE_GUILD
);
```

For detailed command builders, use `dcc_app_slash_builder()`:

```c
dcc_application_command_builder_t command =
    DCC_SLASH_OPTIONS(
        "search",
        "Search members",
        DCC_REQUIRED_STRING("name", "Member name"),
        DCC_OPTIONAL_STRING_CHOICES(
            "sort",
            "Sort mode",
            DCC_STRING_CHOICE("Members", "members"),
            DCC_STRING_CHOICE("Score", "score")
        ),
        DCC_OPTIONAL_INT_RANGE("limit", "Result count", 1, 25)
    );

dcc_app_slash_builder(app, &command, on_search, state);
```

When a slash command has several options, use a typed slash route. This keeps
the command schema, field binding, validation, and typed handler together.

```c
typedef struct search_args {
    const char *name;
    int64_t limit;
} search_args_t;

#define SEARCH_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, name, "name", "Member name") \
    PARAM(type, INT_RANGE, limit, "limit", "Result count", 0U, 10, 1, 25)

static void on_search_typed(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)ctx;
    (void)user_data;

    const search_args_t *search = args;
    /* search->name and search->limit are ready. */
}

dcc_command_route_t typed_search =
    DCC_COMMAND_ROUTE_DATA_VALIDATED(
        "search",
        "Search members",
        search_args_t,
        on_search_typed,
        state,
        SEARCH_PARAMS,
        DCC_VALIDATORS(
            DCC_VALIDATE_STRING_LENGTH(search_args_t, name, "name", 1U, 32U),
            DCC_VALIDATE_INT_RANGE(search_args_t, limit, "limit", 1, 25)
        )
    );

DCC_APP_REGISTER_COMMAND(app, typed_search);
```

The typed route owns copies of the binding and validator arrays after
registration. The `args` pointer is valid only for the handler call; copy values
into your own state when they need to outlive the interaction.

Component interactions can also be registered as typed routes. Button handlers
usually bind the action encoded in `custom_id`; select handlers often bind the
first value or the full selected value array:

```c
typedef struct component_args {
    const char *action;
    const char *first_value;
    const char **values;
    size_t value_count;
} component_args_t;

#define SETTINGS_BUTTON_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_CUSTOM_ID_SUFFIX, action, "settings:")

#define SETTINGS_SELECT_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_FIRST_VALUE, first_value) \
    PARAM(type, REQUIRED_VALUES, values, value_count)

dcc_button_route_t button =
    DCC_BUTTON_PREFIX_PARAMS_DATA_VALIDATED(
        "settings:",
        component_args_t,
        on_settings_button,
        state,
        SETTINGS_BUTTON_PARAMS,
        DCC_CTX_NO_FIELD_VALIDATORS()
    );

dcc_select_route_t select =
    DCC_SELECT_PARAMS_DATA_VALIDATED(
        "settings.mode",
        component_args_t,
        on_settings_select,
        state,
        SETTINGS_SELECT_PARAMS,
        DCC_CTX_NO_FIELD_VALIDATORS()
    );

DCC_APP_REGISTER_BUTTON(app, button);
DCC_APP_REGISTER_SELECT(app, select);
```

Autocomplete can be typed too. Use an autocomplete option in the command
builder, bind the focused value into a small struct, and answer with inline
choices.

```c
typedef struct search_auto_args {
    const char *query;
} search_auto_args_t;

#define SEARCH_AUTO_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, query, "query", "Search query")

static void on_search_autocomplete(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)user_data;

    const search_auto_args_t *auto_args = args;
    (void)auto_args; /* auto_args->query is the focused value. */

    DCC_REPLY_AUTOCOMPLETE_MATCHING(
        ctx,
        DCC_AUTOCOMPLETE_STRING_CHOICE("Feral", "feral"),
        DCC_AUTOCOMPLETE_STRING_CHOICE("Support", "support")
    );
}

dcc_application_command_builder_t search =
    DCC_SLASH_OPTIONS(
        "search",
        "Search members",
        DCC_REQUIRED_STRING_AUTOCOMPLETE("query", "Search query")
    );

dcc_autocomplete_route_t search_autocomplete =
    DCC_AUTOCOMPLETE_BUILDER_PARAMS_DATA(
        &search,
        search_auto_args_t,
        on_search_autocomplete,
        state,
        SEARCH_AUTO_PARAMS
    );

DCC_APP_REGISTER_AUTOCOMPLETE(app, search_autocomplete);
```

For subcommand autocomplete, use `DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS*`
with the same subcommand path that the command handler uses. DCC checks the
subcommand route first for autocomplete interactions and then falls back to the
parent command name.

Direct autocomplete handlers are still available when you do not want a typed
request struct. They route by command name. Use the focused option shortcuts
when the handler needs the current query, or let DCC filter static choices for
the focused string option.

```c
static void on_search_autocomplete(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    const char *query = DCC_CTX_FOCUSED_STRING(ctx, "");
    (void)query; /* Use this when choices come from a database. */

    DCC_REPLY_AUTOCOMPLETE_MATCHING(
        ctx,
        DCC_AUTOCOMPLETE_STRING_CHOICE("Feral", "feral"),
        DCC_AUTOCOMPLETE_STRING_CHOICE("Support", "support")
    );
}

dcc_app_autocomplete(app, "search", on_search_autocomplete, state);
```

General Gateway events can be registered through the app too. Use these for
startup logs, message listeners, and bot-local schedulers that should not reach
down into `dcc_client_on()` directly.

```c
DCC_READY_FN(on_ready) {
    (void)app;
    (void)user_data;
    printf("ready shard=%u/%u\n", ready->shard_id, ready->shard_count);
}

DCC_MESSAGE_COMMAND_FN(on_message) {
    (void)args;
    (void)event;
    (void)user_data;

    (void)DCC_APP_SEND_TEXT(app, message->channel_id, "pong");
}

DCC_APP_ON_READY_ONCE(app, on_ready);
DCC_APP_ON_MESSAGE_COMMAND(app, "!", "ping", on_message);
```

`DCC_APP_ON_MESSAGE_COMMAND()` matches `!ping` and `!ping args`, rejects
`!pingpong`, skips whitespace after the command name, and passes the remaining
arguments as a pointer into the original message content. Use
`DCC_APP_ON_MESSAGE_CREATE()` only when a feature needs raw `MESSAGE_CREATE`
events instead of command routing. The underlying `dcc_app_on_*` functions
remain available for generated code.

Schedulers can be registered directly or declared through `DCC_APP(...)`.
`DCC_TASK_LOOP_*` mirrors discord.py's `tasks.loop` naming while still mapping
to the same DCC scheduler underneath:

```c
static void refresh_cache(dcc_app_t *app, void *user_data) {
    (void)app;
    cache_state_t *state = user_data;
    cache_refresh(state);
}

dcc_app_every_minutes(app, 10U, refresh_cache, &state);

dcc_app_definition_t bot =
    DCC_APP(
        "scheduler-bot",
        DCC_APP_TASKS(
            DCC_TASK_LOOP_MINUTES_DATA(10U, refresh_cache, &state),
            DCC_TASK_DAILY_KST_DATA("00:00", refresh_cache, &state)
        )
    );
```

For richer channel messages, pass a normal message builder:

```c
dcc_message_builder_t message =
    DCC_MESSAGE_TEXT_EMBED(
        "deploy complete",
        DCC_EMBED_RICH(
            "Done",
            "The task finished.",
            0x57F287,
            "DCC",
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        )
    );

dcc_app_send(app, channel_id, &message, NULL, NULL);
```

For announcement text that contains user, role, or everyone-looking mentions but
should not ping accidentally, use typed allowed mentions:

```c
DCC_APP_SEND_SAFE(app, channel_id, "@everyone status updated");
DCC_APP_SEND(app, channel_id, DCC_MESSAGE_MENTION_USERS("hi <@123>", 123ULL));
DCC_APP_SEND(app, channel_id, DCC_MESSAGE_SUCCESS("Saved."));
```

When the next step needs the created message id, use the ID callback variants
instead of parsing the REST body in every handler:

```c
typedef struct ticket_post_state {
    dcc_snowflake_t channel_id;
} ticket_post_state_t;

static void posted(
    dcc_app_t *app,
    const dcc_rest_response_t *response,
    dcc_snowflake_t message_id,
    dcc_status_t status,
    void *user_data
) {
    ticket_post_state_t *state = user_data;
    if (status == DCC_OK) {
        DCC_MESSAGE_THREAD(app, state->channel_id, message_id, "support thread");
    }
    (void)response;
}

ticket_post_state_t *state = get_ticket_post_state();
state->channel_id = channel_id;
DCC_APP_SEND_TEXT_ID(app, channel_id, "ticket opened", posted, state);
DCC_CTX_SEND_UI_ID(ctx, posted, state, DCC_UI_TEXT("ticket opened"));
```

Format helpers cover Discord mention, custom emoji, and relative-time markup:

```c
char text[160];
snprintf(
    text,
    sizeof(text),
    "%s %s starts %s. See %s",
    DCC_MENTION_ROLE(role_id),
    DCC_ANIMATED_EMOJI("party", emoji_id),
    DCC_TIMESTAMP_RELATIVE(starts_at),
    DCC_MENTION_COMMAND("help", help_command_id)
);
DCC_APP_SEND(app, channel_id, DCC_MESSAGE_MENTION_ROLES(text, role_id));
```

Handlers also have context-first aliases for common current-interaction text:

```c
DCC_RESPONDF(
    ctx,
    "%s used this in %s: %s",
    DCC_CTX_USER_MENTION(ctx),
    DCC_CTX_CHANNEL_MENTION(ctx),
    DCC_CTX_MESSAGE_LINK(ctx)
);

DCC_RESPONDF(
    ctx,
    "Updating %s for %s",
    DCC_OPTION_ROLE_MENTION(ctx, "role"),
    DCC_CTX_TARGET_USER_MENTION(ctx)
);

DCC_UPDATEF(ctx, "Page %zu of %zu", page, page_count);
DCC_EDITF(ctx, "Finished in %ums.", elapsed_ms);
DCC_CTX_DONEF(ctx, "Saved %zu records.", record_count);
DCC_BAD_INPUTF(ctx, "Limit must be between %d and %d.", 1, 100);
DCC_APP_EDITF(app, status_channel, status_message, "Updated %zu records.", record_count);
```

Markdown helpers keep status messages readable without manual wrapper strings:

```c
char status[160];
snprintf(
    status,
    sizeof(status),
    "%s %s",
    DCC_MD_BOLD("Saved"),
    DCC_MD_CODE("profile.update")
);
DCC_APP_SEND(app, channel_id, DCC_MESSAGE_SUCCESS(status));
```

For embed images, avatar thumbnails, and custom emoji image URLs, use the CDN
helpers instead of manually assembling `cdn.discordapp.com` paths:

```c
dcc_embed_builder_t embed = DCC_EMBED_COLOR_THUMBNAIL(
    "Member",
    "Current guild avatar",
    DCC_COLOR_BLURPLE,
    DCC_CDN_MEMBER_AVATAR(guild_id, user_id, member_avatar_hash)
);
DCC_APP_SEND(app, channel_id, DCC_MESSAGE_EMBED(embed));
```

For user input that may be a raw id or a copied Discord mention, parse through
the id helpers:

```c
dcc_snowflake_t channel = 0;
if (DCC_PARSE_CHANNEL_MENTION(input, &channel) != DCC_OK) {
    DCC_CTX_BAD_INPUT(ctx, "Use a channel mention like <#123>.");
    return;
}
```

Snowflake IDs can be displayed as creation timestamps without extra parsing:

```c
char line[128];
snprintf(line, sizeof(line), "Created %s", DCC_SNOWFLAKE_RELATIVE(message_id));
DCC_APP_SEND_SAFE(app, channel_id, line);
```

For copied Discord message links, parse the whole URL instead of splitting it in
the handler:

```c
dcc_message_link_t link = {0};
if (DCC_PARSE_MESSAGE_LINK(input, &link) != DCC_OK) {
    DCC_CTX_BAD_INPUT(ctx, "Use a Discord message link.");
    return;
}

printf("jump to channel=%llu message=%llu\n",
    (unsigned long long)link.channel_id,
    (unsigned long long)link.message_id);
```

Use action helpers when a channel message is an embed with one or more button or
select rows:

```c
DCC_APP_SEND(
    app,
    channel_id,
    DCC_MESSAGE_TEXT_EMBED_ACTIONS(
        "ready",
        DCC_EMBED_COLOR("Runtime", "Gateway online", DCC_COLOR_SUCCESS),
        DCC_ACTION_ROW(DCC_BUTTON_PRIMARY_BUILDER("Refresh", "status.refresh"))
    )
);
```

Use presets for the button layouts every bot tends to re-create:

```c
DCC_REPLY(ctx, DCC_CONFIRM_MESSAGE("Run this action?", "job.run", "job.cancel"));
DCC_UPDATE(ctx, DCC_PAGINATOR_MESSAGE("Page 1/4", "page.prev", "page.next", 1U, 0U));

DCC_REPLY(ctx, DCC_CONFIRM_V2("Continue?", "flow.confirm", "flow.cancel"));
DCC_REPLY(ctx, DCC_SETTINGS_PANEL_V2("## Settings", "settings.edit", "settings.enable", "settings.disable", "settings.reset", enabled));
```

`dcc_new_app add preset ... announcement` generates a modal announcement
composer, stores the user draft in the app store, shows an ephemeral preview,
and sends the confirmed Components v2 announcement to the current channel.
`dcc_new_app add preset ... wizard` generates a slash command, start button,
typed modal submit route, and review buttons for setup-style flows.
`dcc_new_app add preset ... counter` generates a store-backed slash command
with `+1` and reset buttons, so persistent component state does not require
hand-written store plumbing.
`dcc_new_app add preset ... crud` generates a store-backed item list with add,
delete-last, and clear actions for simple admin panels.
`dcc_new_app add preset ... help` generates a paged help center with buttons.
`dcc_new_app add preset ... menu` generates a typed select-menu navigation panel.
`dcc_new_app add preset ... poll` generates a store-backed yes/no vote panel
with reset.
`dcc_new_app add preset ... profile` generates a user profile card with a refresh
button. Pass `--name profile_card` when the target feature is also named `profile`.
`dcc_new_app add preset ... roles` generates a guild-only self-role menu with a
typed select route, remove buttons, and role assignment helpers.
`dcc_new_app add preset ... ticket` generates an open-ticket button, typed modal,
ephemeral ticket card, and close button.
`dcc_new_app add preset ... welcome` generates a `GUILD_MEMBER_ADD` event route,
a slash preview command, and a channel-id macro for the welcome destination.
`dcc_new_app add preset ... flow` generates a slash-to-modal-to-review flow
with confirm, edit, and cancel buttons for common multi-step interactions.

For common bot operations, the app runtime exposes short wrappers over the
lower-level REST API. These keep handlers close to the discord.js/discord.py
shape while still using the same DCC request pipeline:

```c
DCC_CHANNEL_SEND_TEXT(app, channel_id, "posted from the app");
DCC_CHANNEL_SEND_UI(app, channel_id, DCC_UI_TEXT("posted as Components v2"));
DCC_CHANNEL_FETCH(app, channel_id);
DCC_CHANNEL_TYPING(app, channel_id);
DCC_CHANNEL_EDIT_PARAMS(app, &channel_params);
DCC_CHANNEL_DELETE(app, channel_id);
DCC_CHANNEL_INVITES_FETCH(app, channel_id);
DCC_CHANNEL_INVITE_CREATE_PARAMS(app, &invite_params);
DCC_CHANNEL_PERMISSION_EDIT_PARAMS(app, channel_id, &permission_overwrite);
DCC_CHANNEL_PERMISSION_DELETE(app, channel_id, role_id);
DCC_GUILD_CHANNELS_REORDER_PARAMS(app, &channel_positions_params);

DCC_GUILD_ROLES_FETCH(app, guild_id);
DCC_GUILD_ROLE_CREATE_PARAMS(app, &role_params);
DCC_ROLE_EDIT_PARAMS(app, &role_params);
DCC_ROLE_DELETE(app, guild_id, role_id);

DCC_MESSAGE_EDIT_TEXT(app, channel_id, message_id, "edited");
DCC_MESSAGE_EDIT_UI(app, channel_id, message_id, DCC_UI_TEXT("edited as Components v2"));
DCC_MESSAGE_DELETE(app, channel_id, message_id);
DCC_MESSAGE_PIN(app, channel_id, message_id);
DCC_MESSAGE_REACT(app, channel_id, message_id, "thumbsup");
DCC_MESSAGE_CROSSPOST(app, channel_id, message_id);

DCC_MESSAGE_THREAD(app, channel_id, message_id, "support thread");
DCC_THREAD_LOCK(app, thread_id);

DCC_APP_SEND_TEXT(app, channel_id, "posted from the app");
DCC_APP_SEND_V2(app, channel_id, DCC_V2_TEXT("posted as Components v2"));
DCC_APP_EDIT_TEXT(app, channel_id, message_id, "edited");
DCC_APP_EDIT_V2(app, channel_id, message_id, DCC_V2_TEXT("edited as Components v2"));
DCC_APP_DELETE_MESSAGE(app, channel_id, message_id);

DCC_APP_THREAD_FROM_MESSAGE(app, channel_id, message_id, "support thread");
DCC_APP_LOCK_THREAD(app, thread_id);

DCC_MEMBER_ADD_ROLE(app, guild_id, user_id, role_id);
DCC_MEMBER_REMOVE_ROLE(app, guild_id, user_id, role_id);
DCC_MEMBER_FETCH(app, guild_id, user_id);
DCC_MEMBER_TIMEOUT(app, guild_id, user_id, "2026-06-23T12:00:00Z");
DCC_MEMBER_CLEAR_TIMEOUT(app, guild_id, user_id);
DCC_MEMBER_KICK(app, guild_id, user_id);
DCC_MEMBER_BAN(app, guild_id, user_id);
DCC_MEMBER_UNBAN(app, guild_id, user_id);
DCC_ME_SET_NICK(app, guild_id, "new nick");

DCC_APP_ADD_MEMBER_ROLE(app, guild_id, user_id, role_id);
DCC_APP_REMOVE_MEMBER_ROLE(app, guild_id, user_id, role_id);
DCC_GUILD_BANS_FETCH(app, guild_id);
DCC_GUILD_PRUNE_COUNT(app, guild_id, 7U);
DCC_GUILD_AUDIT_LOG_FETCH_PAGE(app, guild_id, user_id, action_type, 0U, 0U, 50U);
DCC_GUILD_INTEGRATIONS_FETCH(app, guild_id);
DCC_GUILD_WIDGET_FETCH(app, guild_id);
DCC_GUILD_WIDGET_EDIT_PARAMS(app, guild_id, &widget_params);
DCC_GUILD_ONBOARDING_FETCH(app, guild_id);
DCC_GUILD_ONBOARDING_EDIT_PARAMS(app, &onboarding_params);
DCC_GUILD_WELCOME_SCREEN_FETCH(app, guild_id);
DCC_GUILD_WELCOME_SCREEN_EDIT_PARAMS(app, guild_id, &welcome_params);
DCC_AUTO_MOD_RULES_FETCH(app, guild_id);
DCC_AUTO_MOD_RULE_CREATE_PARAMS(app, guild_id, &auto_mod_rule_params);
DCC_GUILD_EMOJIS_FETCH(app, guild_id);
DCC_GUILD_EMOJI_CREATE_PARAMS(app, guild_id, &emoji_params);
DCC_GUILD_STICKERS_FETCH(app, guild_id);
DCC_GUILD_STICKER_CREATE_PARAMS(app, &sticker_params);
DCC_STICKER_PACKS_FETCH(app);
DCC_GUILD_MESSAGES_SEARCH_PARAMS(app, guild_id, &message_search_params);
DCC_GATEWAY_BOT_FETCH(app);
DCC_MY_VOICE_STATE_FETCH(app, guild_id);
DCC_USER_VOICE_STATE_EDIT_PARAMS(app, &voice_state_params);
DCC_GUILD_VOICE_REGIONS_FETCH(app, guild_id);
DCC_SKUS_FETCH(app, application_id);
DCC_ENTITLEMENTS_FETCH_PAGE(app, application_id, user_id, sku_ids, sku_count, 0U, 0U, 25U, guild_id, 1U);
DCC_TEST_ENTITLEMENT_CREATE_PARAMS(app, &test_entitlement_params);

DCC_GUILD_SCHEDULED_EVENTS_FETCH(app, guild_id);
DCC_SCHEDULED_EVENT_CREATE_PARAMS(app, &scheduled_event_params);
DCC_SCHEDULED_EVENT_EDIT_PARAMS(app, &scheduled_event_params);
DCC_SCHEDULED_EVENT_DELETE(app, guild_id, event_id);

DCC_CHANNEL_WEBHOOKS_FETCH(app, channel_id);
DCC_GUILD_WEBHOOKS_FETCH(app, guild_id);
DCC_WEBHOOK_CREATE_PARAMS(app, &webhook_params);
DCC_WEBHOOK_SEND_TEXT(app, webhook_id, webhook_token, "posted by webhook");
DCC_WEBHOOK_SEND(app, webhook_id, webhook_token, DCC_MESSAGE_TEXT("posted by webhook"));
DCC_WEBHOOK_SEND_UI(app, webhook_id, webhook_token, DCC_UI_TEXT("Components v2 webhook"));
DCC_WEBHOOK_MESSAGE_EDIT(app, webhook_id, webhook_token, message_id, DCC_MESSAGE_TEXT("edited"));
DCC_WEBHOOK_MESSAGE_DELETE(app, webhook_id, webhook_token, message_id);

DCC_ME_FETCH(app);
DCC_USER_FETCH(app, user_id);
DCC_USER_DM_SEND_TEXT(app, user_id, "hello in DM");
DCC_USER_DM_SEND_UI(app, user_id, DCC_UI_TEXT("Components v2 DM"));
DCC_GROUP_DM_RECIPIENT_ADD_PARAMS(app, &group_dm_recipient_params);
DCC_GROUP_DM_RECIPIENT_REMOVE(app, group_dm_channel_id, user_id);
DCC_GUILD_INVITES_FETCH(app, guild_id);
DCC_INVITE_FETCH_FULL(app, invite_code);

DCC_STAGE_CREATE_PARAMS(app, &stage_params);
DCC_STAGE_FETCH(app, stage_channel_id);
DCC_STAGE_DELETE(app, stage_channel_id);

DCC_TEMPLATE_FETCH(app, template_code);
DCC_GUILD_TEMPLATES_FETCH(app, guild_id);
DCC_GUILD_TEMPLATE_CREATE_PARAMS(app, &template_params);
DCC_GUILD_TEMPLATE_SYNC(app, guild_id, template_code);
```

For messages that should always stay at the bottom of a channel, use managed
latest publishing with the app-owned DCC store. DCC loads the previous saved
message reference, deletes it when present, posts the new message, parses the
created message ID, and saves the new reference.

```sh
dcc_new_app add config . status_channel channel STATUS_CHANNEL
dcc_new_app add latest-message . profile status status_channel
dcc_new_app add scheduled-latest . profile daily_status status_channel --daily-kst 09:00
```

```c
DCC_APP_PUBLISH_LATEST_STORE_TEXT(app, "status.latest", channel_id, "service online", NULL, NULL);
DCC_APP_PUBLISH_LATEST_STORE_UI(
    app,
    "status.panel",
    channel_id,
    NULL,
    NULL,
    DCC_UI_TEXT("**service online**")
);
```

Inside an interaction handler, `DCC_SEND_TEXT()` posts to the interaction
channel without spelling out the channel ID:

```c
DCC_SEND_TEXT(ctx, "visible in this channel");

/* Guild and author IDs come from the interaction context. */
DCC_AUTHOR_ADD_ROLE(ctx, verified_role_id);
```

For signed stateful UIs, register a component session directly on the app.
DCC owns the listener and the handler receives the verification result through
`dcc_ctx_t`.

```c
static void on_page_action(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    if (dcc_ctx_component_session_status(ctx) != DCC_COMPONENT_SESSION_VERIFY_OK) {
        DCC_REPLY_ERROR(ctx, "Expired", "This UI can no longer be used.");
        return;
    }

    const char *action = dcc_ctx_component_session_action(ctx);
    if (strcmp(action, "next") == 0) {
        DCC_UPDATE_V2(ctx, DCC_V2_TEXT("Next page"));
    }
}

dcc_app_component_session(app, &session, NULL, on_page_action, state);
```

The same listener can be declared on an app definition. DCC copies the session
or store during registration, so the route remains valid after setup:

```c
dcc_app_definition_t bot =
    DCC_APP(
        "pages",
        DCC_APP_COMPONENT_SESSIONS(
            DCC_COMPONENT_SESSION_ROUTE(&session, on_page_action, state),
            DCC_COMPONENT_SESSION_STORE_ROUTE(&session_store, on_page_action, state)
        )
    );
```

## Interaction Context

Handlers receive `dcc_ctx_t`. It exposes typed slash options, modal fields,
flow-aware replies, followups, and original-response edits.

```c
static void on_rank(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    typedef struct rank_args {
        const char *name;
        int64_t limit;
        uint8_t public_result;
    } rank_args_t;

    rank_args_t args = {0};

    DCC_CTX_BIND_OPTION_FIELDS_OR_REPLY(
        ctx,
        &args,
        DCC_ARG_REQUIRED_STRING(rank_args_t, name, "name"),
        DCC_ARG_INT(rank_args_t, limit, "limit", 10),
        DCC_ARG_BOOL(rank_args_t, public_result, "public", 0U)
    );

    DCC_CTX_VALIDATE_OR_REPLY(
        ctx,
        &args,
        DCC_VALIDATE_STRING_LENGTH(rank_args_t, name, "name", 1U, 32U),
        DCC_VALIDATE_INT_RANGE(rank_args_t, limit, "limit", 1, 100)
    );

    DCC_CTX_TRY(ctx, DCC_CTX_SEND_TEXT(ctx, "Rank lookup queued."));

    char text[128];
    snprintf(
        text,
        sizeof(text),
        "name=%s limit=%lld public=%u",
        args.name,
        (long long)args.limit,
        args.public_result
    );
    DCC_REPLY_EPHEMERAL(ctx, text);
}
```

For one-off reads, the individual getters are still available:

```c
const char *name = DCC_OPTION_STRING_OR(ctx, "name", "unknown");
int64_t limit = DCC_OPTION_INT_OR(ctx, "limit", 10);
```

Modal and Components v2 form submissions can be bound the same way:

```c
static void on_profile_modal(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    typedef struct profile_form {
        const char *birthday;
        uint8_t public_age;
    } profile_form_t;

    profile_form_t form = {0};

    DCC_CTX_BIND_FORM_FIELDS_OR_REPLY(
        ctx,
        &form,
        DCC_CTX_FORM_FIELD_REQUIRED_STRING(profile_form_t, birthday, "birthday"),
        DCC_CTX_FORM_FIELD_BOOL(profile_form_t, public_age, "public_age", 1U)
    );

    DCC_CTX_VALIDATE_OR_REPLY(
        ctx,
        &form,
        DCC_VALIDATE_DATE_MMDD_OR_YYYYMMDD(profile_form_t, birthday, "birthday")
    );

    dcc_ctx_parsed_date_t birthday;
    if (DCC_PARSE_BIRTHDAY(form.birthday, &birthday) != DCC_OK) {
        DCC_REPLY_BAD_INPUT(ctx, "Use a real birthday in MMDD or YYYYMMDD format.");
        return;
    }

    DCC_REPLY_DONE(ctx, "Profile updated.");
}
```

`DCC_VALIDATE_DATE_YYYYMMDD()` requires exactly eight digits and rejects short
values such as `2007`; `DCC_VALIDATE_DATE_MMDD()` requires exactly four digits.
Use `DCC_VALIDATE_BIRTHDAY()` when a form intentionally accepts either format.
`DCC_PARSE_DATE()`, `DCC_PARSE_DATE_MMDD()`, `DCC_PARSE_DATE_YYYYMMDD()`, and
`DCC_PARSE_BIRTHDAY()` return a `dcc_ctx_parsed_date_t` with `year`, `month`,
`day`, and `has_year` fields for application logic after validation.

For modal-heavy flows, prefer a typed modal flow over blocking inside a button
handler. The flow keeps the modal `custom_id`, builder, field binding, validators,
and submit route in one declaration. Registration copies the submit route; modal
display should use an inline builder or a flow object with static storage.

```c
typedef struct birthday_form {
    const char *birthday;
} birthday_form_t;

static void on_birthday_modal(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)args;
    (void)user_data;
    DCC_REPLY_DONE(ctx, "Birthday saved.");
}

dcc_modal_flow_t birthday_flow =
    DCC_MODAL_V2_FLOW(
        "birthday.register",
        "Birthday",
        birthday_form_t,
        on_birthday_modal,
        state,
        DCC_CTX_FORM_FIELD_BINDINGS(
            DCC_CTX_FORM_FIELD_REQUIRED_STRING(birthday_form_t, birthday, "birthday")
        ),
        DCC_CTX_FIELD_VALIDATORS(
            DCC_VALIDATE_DATE_YYYYMMDD(birthday_form_t, birthday, "birthday")
        ),
        DCC_MODAL_V2_FIELD_TEXT_PLACEHOLDER("birthday", "Birthday", "YYYYMMDD")
    );

DCC_APP_REGISTER_MODAL_FLOW(app, birthday_flow);
```

When the custom ID follows a namespace plus handler-name convention, use the
params `_FN` flow sugar and keep the action string out of the call site:

```c
#define BIRTHDAY_MODAL_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, birthday, "birthday")

dcc_modal_flow_t birthday_flow =
    DCC_MODAL_V2_FLOW_NS_PARAMS_FN_DATA(
        "birthday",
        "Birthday",
        birthday_form_t,
        on_birthday_modal,
        state,
        BIRTHDAY_MODAL_PARAMS,
        DCC_UI_LABEL("Birthday", DCC_UI_INPUT_NS("birthday", "birthday", "Birthday", 1U))
    );
```

`DCC_REPLY()` and the `DCC_REPLY_*()` macros are flow-aware because they call
the same `dcc_ctx_reply*()` functions underneath. If the handler already
deferred, the reply edits the original response. If it already replied, it
becomes a followup.

```c
DCC_DEFER_EPHEMERAL(ctx);
DCC_PUBLIC(ctx, "Done");
DCC_EPHEMERAL(ctx, "Only you can see this.");
DCC_ACK(ctx, "Saved.");
DCC_DENY(ctx, "Only moderators can use this.");
DCC_CTX_BAD_INPUT(ctx, "Use YYYYMMDD.");
DCC_NOT_FOUND(ctx, "No matching member was found.");
DCC_BUSY(ctx, "This action is already running.");
DCC_INTERNAL_ERROR(ctx);
DCC_SUCCESS(ctx, "Saved", "Your settings were updated.");
DCC_ERROR(ctx, "Action failed", "Try again later.");
DCC_FOLLOWUP_PUBLIC(ctx, "Extra detail");
DCC_REPLY_V2(ctx, DCC_V2_TEXT("Component-first reply"));
DCC_SHOW_MODAL_V2(
    ctx,
    "profile.modal",
    "Profile",
    DCC_V2_LABEL("Name", DCC_MODAL_V2_TEXT_INPUT("profile.name", "Name", 1U))
);
```

For the classic embed plus button-row layout, use `DCC_REPLY_*_ACTIONS()` so the
handler does not need temporary arrays:

```c
DCC_REPLY_TEXT_EMBED_ACTIONS(
    ctx,
    "ready",
    DCC_EMBED_COLOR("Runtime", "Gateway online", 0x57F287),
    DCC_ACTION_ROW(
        DCC_BUTTON_PRIMARY_BUILDER("Refresh", "status.refresh"),
        DCC_BUTTON_DANGER_BUILDER("Close", "status.close")
    )
);
```

The longer `DCC_REPLY_*()` forms are still available. Use them when you want the
operation spelled out, or use `DCC_REPLY_PUBLIC_EMBED()` /
`DCC_REPLY_EPHEMERAL_EMBED()` for single-embed replies without hand-building a
message.

Context also exposes the invoking user and resolved member permissions for
guild interactions:

```c
if (DCC_CTX_HAS_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_MESSAGES)) {
    DCC_REPLY_EPHEMERAL(ctx, "You can manage messages.");
}
```

Use `DCC_PERMISSIONS(...)` when the permission set is assembled inline, and
`DCC_CTX_CAN_ANY_PERMISSION()` when any one moderation capability should pass:

```c
dcc_permission_t cleanup_permissions =
    DCC_PERMISSIONS(
        DCC_PERMISSION_MANAGE_MESSAGES,
        DCC_PERMISSION_READ_MESSAGE_HISTORY
    );

if (DCC_CTX_CAN(ctx, cleanup_permissions) ||
    DCC_CTX_CAN_ANY_PERMISSION(ctx, DCC_PERMISSION_MODERATION_BASIC)) {
    DCC_REPLY_EPHEMERAL(ctx, "Allowed.");
}
```

When a missing permission should stop the handler immediately, use the matching
context-first require helpers:

```c
DCC_CTX_REQUIRE_CAN(ctx, cleanup_permissions, "Missing cleanup permission.");
DCC_CTX_REQUIRE_CAN_ANY(ctx, DCC_PERMISSION_MODERATION_BASIC, "Missing moderation permission.");
```

Use context shortcut macros when handler code should read closer to
discord.py's `ctx.author`, `ctx.guild`, and `ctx.channel` style:

```c
dcc_snowflake_t author_id = DCC_CTX_AUTHOR_ID(ctx);
dcc_snowflake_t channel_id = DCC_CTX_CHANNEL_ID(ctx);
const dcc_user_t *author = DCC_CTX_AUTHOR(ctx);
const dcc_guild_t *guild = DCC_CTX_GUILD(ctx);
const dcc_channel_t *channel = DCC_CTX_CHANNEL(ctx);

if (DCC_CTX_IS_GUILD(ctx) && DCC_CTX_IS_AUTHOR(ctx, owner_id)) {
    DCC_SEND_TEXT(ctx, "Owner command received.");
}
```

Object helpers first use the interaction's resolved payload, then fall back to
the enabled client cache. They return borrowed views. This keeps handlers short
without adding hidden REST calls:

```c
const dcc_user_t *user = DCC_CTX_OPT_USER(ctx, "user");
const dcc_member_t *member = DCC_CTX_OPT_MEMBER(ctx, "user");
const dcc_role_t *role = DCC_CTX_OPT_ROLE(ctx, "role");
const dcc_channel_t *target = DCC_CTX_OPT_CHANNEL(ctx, "channel");

if (user == NULL || member == NULL) {
    DCC_EPHEMERAL(ctx, "That member is not available in the interaction payload or cache.");
    return;
}
```

Small handlers can combine required option validation with local declarations.
These helpers reply with the standard bad-input message and return from the
current void handler when the option is missing or cannot be resolved:

```c
DCC_REQUIRED_GUILD(ctx);
DCC_REQUIRED_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_GUILD);
DCC_REQUIRED_STRING_OPTION(name, ctx, "name");
DCC_REQUIRED_INT_OPTION(limit, ctx, "limit");
DCC_REQUIRED_USER_OPTION(user, ctx, "user");

DCC_REPLY_TEXT(ctx, name);
```

Modal submit handlers have the same declaration style for fields:

```c
DCC_REQUIRED_TEXT_FIELD(birthday, ctx, "birthday");
DCC_REQUIRED_CHECKBOX_FIELD(public_age, ctx, "public_age");
DCC_REQUIRED_FIRST_VALUE_FIELD(color, ctx, "color");
```

Select handlers can use the same shape for submitted values:

```c
DCC_REQUIRED_SELECT_FIRST(role_id, ctx);
DCC_REQUIRED_SELECT_VALUES(role_ids, role_count, ctx);
```

For component routes, `DCC_CTX_ACTION()` returns the suffix after a known
`custom_id` prefix:

```c
DCC_REQUIRED_ACTION(action, ctx, "settings:");
if (strcmp(action, "save") == 0) {
    DCC_REPLY_DONE(ctx, "Saved.");
}
```

Namespace-style component IDs can be handled the same way:

```c
DCC_REQUIRED_COMPONENT_ACTION(action, ctx, "birthday");
DCC_REQUIRED_COMPONENT_IS(ctx, "birthday", "register");
```

Session handlers can inspect the signed component action without parsing the
`custom_id` string:

```c
const char *action = dcc_ctx_component_session_action(ctx);
```

Route declarations can use the short names when a feature should read like a
discord.js router:

```c
dcc_app_extension_t controls =
    DCC_FEATURE(
        "controls",
        DCC_FEATURE_COMMANDS(DCC_ON_SLASH("ping", "Reply with pong", on_ping)),
        DCC_FEATURE_CONTEXT_MENUS(
            DCC_ON_USER_CONTEXT_MENU("Inspect user", on_inspect_user),
            DCC_ON_MESSAGE_CONTEXT_MENU("Quote message", on_quote_message)
        ),
        DCC_FEATURE_COMPONENTS(
            DCC_BUTTON("settings.open", on_settings_open),
            DCC_SELECT("settings.mode", on_settings_mode),
            DCC_MODAL("settings.modal", on_settings_submit)
        )
    );
```

When the C handler name should also be the Discord command name or component
`custom_id`, use the function-name aliases to avoid repeating the same string:

```c
dcc_app_extension_t core =
    DCC_FEATURE(
        "core",
        DCC_FEATURE_COMMANDS(DCC_ON_SLASH_FN(ping, "Reply with pong")),
        DCC_FEATURE_COMPONENTS(
            DCC_BUTTON_NS_FN("status", refresh),
            DCC_SELECT_NS_FN("roles", choose),
            DCC_MODAL_NS_FN("profile", edit)
        )
    );
```

Context menu handlers receive the same `dcc_ctx_t` as slash commands. Use
`DCC_CTX_TARGET_ID(ctx)`, `DCC_CTX_TARGET_USER(ctx)`, and
`DCC_CTX_TARGET_MESSAGE(ctx)` to inspect the selected user or message. Direct
registration is available with `DCC_APP_ON_USER_CONTEXT_MENU()` and
`DCC_APP_ON_MESSAGE_CONTEXT_MENU()` when you are wiring an app object manually.
The generator can add the same route shape:

```sh
dcc_new_app add context-menu . profile inspect_user user --guild-only
dcc_new_app add context-menu . profile quote_message message
```

When a typed route struct would be too much, read slash parameters and modal
fields directly with the shorter aliases:

```c
static void on_search(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    const char *name = DCC_PARAM_STRING_OR(ctx, "name", "");
    int64_t limit = DCC_PARAM_INT_OR(ctx, "limit", 10);
    const dcc_user_t *user = DCC_PARAM_USER(ctx, "user");

    (void)name;
    (void)limit;
    (void)user;
    DCC_REPLY_DONE(ctx, "Search queued.");
}

static void on_profile_submit(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    const char *display_name = DCC_FIELD_TEXT_OR(ctx, "display_name", "");
    uint8_t visible = DCC_FIELD_CHECKED_OR(ctx, "visible", 1U);

    (void)display_name;
    (void)visible;
    DCC_REPLY_DONE(ctx, "Profile saved.");
}
```

For larger handlers, bind directly into a request struct and let DCC handle the
bad-input reply:

```c
typedef struct profile_args {
    const char *display_name;
    uint8_t visible;
} profile_args_t;

static void on_profile_submit(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    profile_args_t args = {0};
    DCC_CTX_BIND_FORM_FIELDS_OR_REPLY(
        ctx,
        &args,
        DCC_CTX_FORM_FIELD_REQUIRED_STRING(profile_args_t, display_name, "display_name"),
        DCC_CTX_FORM_FIELD_BOOL(profile_args_t, visible, "visible", 1U)
    );

    DCC_REPLY_DONE(ctx, "Profile saved.");
}
```

The matching option helpers are `DCC_CTX_BIND_OPTIONS_OR_REPLY()` for local
variables and `DCC_CTX_BIND_OPTION_FIELDS_OR_REPLY()` for structs.

For simple handler-local checks, `DCC_CTX_REQUIRE_*` sends an ephemeral message
and returns from the current void handler. `DCC_REQUIRE_*` remains available as
the shorter alias:

```c
static void on_admin_reload(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    DCC_CTX_REQUIRE_GUILD(ctx, "This command can only be used in a server.");
    DCC_CTX_REQUIRE_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_GUILD, "Missing Manage Server.");
    DCC_CTX_REQUIRE_ANY_ROLE_IDS(ctx, "Missing staff role.", 123456789012345678ULL, 223456789012345678ULL);

    DCC_REPLY_DONE(ctx, "Reload queued.");
}
```

## Middleware

Use middleware for checks, shared state validation, permissions, or telemetry.
Return `DCC_OK` to continue. Any other status stops the route handler.

```c
static dcc_status_t require_ready(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    const int *ready = user_data;
    return ready != NULL && *ready ? DCC_OK : DCC_ERR_STATE;
}

dcc_app_use(app, require_ready, &ready);
```

Middleware can also call `dcc_event_cancel(dcc_ctx_event(ctx))` when it wants to
stop later listeners that share the same event bus.

Generated feature files can start with a middleware skeleton:

```sh
dcc_new_app add middleware . profile require_config config
dcc_new_app add middleware . profile guild_only guild
```

`config` checks that generated `BOT_CONFIG(user_data)` is available. `guild`
and `dm` are small editable examples for request-shape checks; route-specific
guards such as `--guild-only` remain the better fit when only one command needs
the rule.

For common yes/no route predicates, use a check instead of writing a full
middleware. The check returns `DCC_OK` to allow the handler and any other status
to send the configured error response.

```c
static dcc_status_t is_setup_complete(dcc_ctx_t *ctx, void *user_data) {
    (void)ctx;
    const app_state_t *state = user_data;
    return state != NULL && state->setup_complete ? DCC_OK : DCC_ERR_STATE;
}
```

Common guards are built in. They reply with an ephemeral error message and stop
the route handler when the check fails.

```c
dcc_app_use_guild_only(app);
/* or */
dcc_app_use_dm_only(app);
dcc_app_use_nsfw_only(app);
dcc_app_use_channel_types(app, (uint32_t[]){DCC_CHANNEL_TEXT}, 1U);
dcc_app_use_owner(app, owner_user_id);
dcc_app_use_required_permissions(app, DCC_PERMISSION_MANAGE_MESSAGES);
dcc_app_use_required_roles(app, (dcc_snowflake_t[]){moderator_role_id}, 1U);
dcc_app_use_any_role(app, (dcc_snowflake_t[]){admin_role_id, moderator_role_id}, 2U);
```

When using `DCC_APP(...)`, the same defaults can be declared without setup
calls:

```c
dcc_app_definition_t bot =
    DCC_APP(
        "admin-bot",
        DCC_APP_GUILD_ONLY(),
        DCC_APP_CHANNEL_TYPES(DCC_CHANNEL_TEXT),
        DCC_APP_OWNER(owner_user_id),
        DCC_APP_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_MESSAGES),
        DCC_APP_CHECK_MESSAGE(is_setup_complete, &state, "Setup required", "Finish setup first."),
        DCC_APP_COOLDOWN_USER(3U, DCC_SECONDS(10U)),
        DCC_APP_EXTENSIONS(admin_cog)
    );
```

For route-local checks, prefer a declarative route policy when the route is
declared through sugar:

```c
dcc_app_extension_t admin =
    DCC_FEATURE(
        "admin",
        DCC_FEATURE_COMMANDS(
            DCC_ON_SLASH_GUARDED(
                "reload",
                "Reload config",
                on_reload,
                DCC_USE(DCC_MIDDLEWARE(require_ready, &ready)),
                DCC_ONLY_GUILD(),
                DCC_IN_CHANNEL_TYPES(DCC_CHANNEL_TEXT, DCC_CHANNEL_PUBLIC_THREAD),
                DCC_OWNER_ONLY(owner_user_id),
                DCC_CAN(DCC_PERMISSION_MANAGE_MESSAGES),
                DCC_HAS_ANY_ROLE(admin_role_id, moderator_role_id),
                DCC_WITH_CHECK_MESSAGE(
                    is_setup_complete,
                    &state,
                    "Setup required",
                    "Finish setup before using this action."
                ),
                DCC_RATE_LIMIT_USER(1U, DCC_SECONDS(5U))
            )
        )
    );
```

The same policy shape is available when you register routes directly on an
already-created app:

```c
DCC_APP_ON_SLASH_GUARDED(
    app,
    "reload",
    "Reload config",
    on_reload,
    DCC_ONLY_GUILD(),
    DCC_CAN(DCC_PERMISSION_MANAGE_MESSAGES),
    DCC_RATE_LIMIT_USER(1U, DCC_SECONDS(5U))
);
```

Use `DCC_ONLY_DM()` instead of `DCC_ONLY_GUILD()` for routes that should only
work in DMs. Use `DCC_ONLY_NSFW()` for routes that should only work in cached
age-restricted channels. Use `DCC_IN_CHANNEL_TYPES(...)`, `DCC_HAS_ROLE(...)`,
`DCC_HAS_ROLES(...)`, and `DCC_HAS_ANY_ROLE(...)` for channel and role checks
without writing custom middleware. Channel checks use the DCC cache; role checks
use interaction resolved members first and fall back to the member cache. The
longer `DCC_GUARD_*` names remain available if you prefer explicit names.

Use a global error handler to centralize guard or middleware failures:

```c
DCC_FRIENDLY_ERROR_FN(on_error)

dcc_app_on_error(app, on_error, NULL);
```

Generated apps can wire this through the `DCC_APP(...)` definition:

```sh
dcc_new_app add error-handler . bot_error friendly
```

The generated handler uses `DCC_FRIENDLY_ERROR_FN(...)` and
`DCC_APP_ERROR_HANDLER(on_bot_error, &config)`. Use `simple` for a generic
user-facing message, or `verbose` while debugging local development.

Inside void handlers, use `DCC_CTX_TRY(ctx, expression)` for calls that return
`dcc_status_t`. Failed statuses are routed through the app error handler above,
or through DCC's default error response if no custom handler is installed:

```c
DCC_CTX_TRY(ctx, DCC_CTX_SEND_TEXT(ctx, "Queued."));
DCC_CTX_TRY_MSG(ctx, DCC_CTX_ADD_AUTHOR_ROLE(ctx, role_id), "Could not add that role.");
```

The same guards can be attached to a single route:

```c
dcc_app_route_id_t save_route = DCC_APP_ROUTE_INVALID;
dcc_app_button_route(app, "settings.save", on_save, state, &save_route);
dcc_app_route_use_owner(app, save_route, owner_user_id);

dcc_app_check_t setup_check =
    DCC_CHECK_MESSAGE(is_setup_complete, state, "Setup required", "Finish setup first.");
dcc_app_route_use_check(app, save_route, &setup_check);
```

Cooldowns are route middleware too. Buckets can be global, per user, per
channel, or per guild:

```c
dcc_app_cooldown_options_t cooldown = DCC_COOLDOWN_USER(3U, DCC_SECONDS(10U));
dcc_app_route_use_cooldown(app, save_route, &cooldown);
```

Use an allowlist when multiple users can administer the bot:

```c
const dcc_snowflake_t admins[] = {owner_user_id, maintainer_user_id};
dcc_app_use_user_allowlist(app, admins, sizeof(admins) / sizeof(admins[0]));
```

## Auto Defer

Use app-level auto-defer when handlers may run near Discord's initial response
deadline. If the handler has not claimed the initial response before the
threshold, DCC sends a deferred response. Later `dcc_ctx_reply*()` calls edit the
original response instead of trying to send a second initial response.

```c
dcc_app_options_t options =
    DCC_APP_OPTIONS_AUTO_DEFER_PRIVATE(token, DCC_INTENTS_DEFAULT, DCC_MS(1500U));
```

You can also configure it after creating the app:

```c
dcc_app_auto_defer_ephemeral(app, DCC_MS(1500U));
```

## Scheduled Tasks

Tasks run on DCC's LLAM-backed task group after `dcc_app_start()`.

```c
static void refresh_cache(dcc_app_t *app, void *user_data) {
    (void)app;
    refresh_state(user_data);
}

dcc_app_every_seconds(app, 60, refresh_cache, state);
dcc_app_every_minutes(app, 5, refresh_cache, state);
dcc_app_every_kst(app, "00:00", refresh_cache, state);
```

## Waiters

Inside a handler, use context wait helpers when a short flow needs the next
button, select, modal submit, or message from the same user, channel, and guild:

```c
static void on_confirm(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    dcc_app_wait_result_t result = {0};
    if (DCC_WAIT_FOR_BUTTON(ctx, "confirm.yes", (uint32_t)DCC_SECONDS(30U), &result) == DCC_OK) {
        DCC_REPLY_DONE(ctx, "Confirmed.");
    }
    DCC_WAIT_RESULT_DEINIT(&result);
}
```

Use the prefix forms for pagers and stateful component IDs:

```c
dcc_app_wait_result_t result = {0};
if (DCC_WAIT_FOR_BUTTON_PREFIX(ctx, "page:", (uint32_t)DCC_SECONDS(30U), &result) == DCC_OK) {
    const char *custom_id = DCC_WAIT_CUSTOM_ID(&result);
    /* route page:next, page:prev, ... */
}
DCC_WAIT_RESULT_DEINIT(&result);
```

Wait result sugar keeps collector-style code away from owned object internals:

```c
dcc_app_wait_result_t result = {0};
if (DCC_WAIT_FOR_SELECT(ctx, "settings.role", (uint32_t)DCC_SECONDS(30U), &result) == DCC_OK) {
    const char *role_id = DCC_WAIT_VALUE(&result, 0);
    DCC_REPLY_EPHEMERAL(ctx, role_id != NULL ? role_id : "No role selected.");
}
DCC_WAIT_RESULT_DEINIT(&result);
```

Supervisor code can also wait for the next app-level interaction or message
through owned result helpers:

```c
dcc_app_wait_result_t result = {0};
if (DCC_APP_WAIT_FOR_BUTTON(app, (uint32_t)DCC_SECONDS(30U), &result) == DCC_OK) {
    printf("button=%s\n", DCC_WAIT_CUSTOM_ID(&result));
}
DCC_WAIT_RESULT_DEINIT(&result);
```

For explicit app-level filters, initialize `dcc_app_wait_filter_t` or use sugar
such as `DCC_WAIT_FILTER_USER(user_id)` and
`DCC_WAIT_FILTER_COMPONENT_PREFIX("settings:")`, then call
`DCC_APP_WAIT_FOR_COMPONENT_FILTERED()` or
`DCC_APP_WAIT_FOR_MESSAGE_FILTERED()`. Wait results own cloned
interaction/message objects and must be released with
`dcc_app_wait_result_deinit()` or `DCC_WAIT_RESULT_DEINIT()`.

Prefer route APIs and component sessions for long-lived bot UI. Waiters are best
for tests, setup flows, short confirmation flows, and supervisor/admission code.

## Command Sync Helpers

The app stores the command registry options from `dcc_app_options_t`.
Default scope is global; guild scope must be explicit.

```c
dcc_app_options_t options =
    DCC_APP_OPTIONS_GUILD(token, DCC_INTENTS_DEFAULT, guild_id);
```

For the easiest development loop, let the app reconcile commands on READY. The
sugar auto-sync forms infer the application id from the READY event and inherit
the command scope from the app options:

```c
dcc_app_options_t options =
    DCC_APP_OPTIONS_WITH_AUTO_SYNC(token, DCC_INTENTS_DEFAULT);

dcc_app_definition_t app =
    DCC_APP("bot", DCC_APP_EXTENSIONS(commands), DCC_APP_AUTO_SYNC());
```

Use `DCC_APP_AUTO_SYNC_GLOBAL()` or `DCC_APP_AUTO_SYNC_GUILD(guild_id)` when a
definition should force a scope instead of inheriting `DCC_COMMAND_GUILD_ID` or
the scope stored in `dcc_app_options_t`.

Environment-based sync is still explicit so production deploys can fail early
when the application id is missing:

```env
DCC_TOKEN=...
DCC_APPLICATION_ID=...
DCC_COMMAND_SYNC=apply
# DCC_COMMAND_GUILD_ID=123456789012345678
```

`DCC_COMMAND_SYNC=plan` or `DCC_COMMAND_SYNC=dry-run` fetches the remote command
snapshot and logs the plan without writing. `DCC_COMMAND_SYNC_DELETE_STALE=true`
allows stale remote commands to be removed when applying.

The same behavior is available explicitly from C:

```c
dcc_app_command_sync_options_t sync =
    DCC_APP_COMMAND_SYNC_GUILD(application_id, guild_id);

dcc_app_sync_commands_on_ready(app, &sync);
```

When you already have a remote command JSON snapshot, build and apply a plan in
one call:

```c
dcc_command_registry_plan_t plan;
dcc_app_sync_commands_from_json(
    app,
    application_id,
    NULL,
    remote_json,
    remote_json_len,
    &plan,
    NULL,
    NULL
);
dcc_command_registry_plan_deinit(&plan);
```

For live bots, fetch the remote snapshot through `dcc_command_registry_fetch_remote()`
and pass the returned body to `dcc_app_sync_commands_from_json()`.
