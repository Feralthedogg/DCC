# Sugar-First API

DCC documents common construction with `<dcc/sugar.h>` first. The sugar layer is
just C compound literals and defaults over the public structs. It keeps short,
static values readable while leaving the explicit builder and setter APIs
available for generated, conditional, or long-lived data.

```c
#include <dcc/sugar.h>
```

Use focused headers when you are writing a narrow library module, but application
code can normally start with `sugar.h`.

## Handler Declarations

Use handler declaration sugar when you want application code to read closer to a
discord.py callback without remembering every C function signature:

```c
DCC_SLASH_FN(ping) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_BUTTON_FN(on_refresh) {
    (void)user_data;
    (void)DCC_UPDATEF(ctx, "Refreshed by %s", DCC_CTX_USER_MENTION(ctx));
}

DCC_MODAL_FN(on_profile_submit) {
    (void)user_data;
    (void)DCC_CTX_DONE(ctx, "Saved.");
}

DCC_READY_FN(on_ready) {
    (void)app;
    printf("logged in with session %s\n", ready->session_id);
}

DCC_TASK_FN(on_tick) {
    (void)user_data;
    dcc_app_send_text(app, 123456789012345678ULL, "scheduled tick", NULL, NULL);
}
```

`DCC_HANDLER()` remains the generic callback form. `DCC_SLASH_FN()`,
`DCC_BUTTON_FN()`, `DCC_SELECT_FN()`, and `DCC_MODAL_FN()` use the same
signature but make feature files easier to scan. For typed routes, prefer the
role-specific `DCC_COMMAND_ARGS_IMPL()`, `DCC_BUTTON_ARGS_IMPL()`,
`DCC_SELECT_ARGS_IMPL()`, `DCC_MODAL_ARGS_IMPL()`, and
`DCC_AUTOCOMPLETE_ARGS_IMPL()` helpers. DCC keeps the route ABI as `void *args`,
generates the wrapper, and lets your handler body start with a typed pointer.

```c
DCC_COMMAND_ARGS_IMPL(ban, ban_args_t, command) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, command->reason);
}
```

Mutable variants such as `DCC_BUTTON_ARGS_MUT_IMPL()` pass a non-const typed
pointer when a binding object must be edited before returning. The compatible
`DCC_TYPED_*_IMPL()` names and the lower-level `DCC_TYPED_HANDLER()` plus
`DCC_TYPED_ARGS()` pattern remain available for generated or unusual handlers
that need to inspect the raw `void *args` value.

For the common case where the C function name is also the command name, use the
function-name route helpers. The `_FN` helpers use `#handler` as the command
name, custom ID, or context-menu label and keep the handler pointer in sync:

```c
dcc_app_extension_t cog =
    DCC_FEATURE(
        "core",
        DCC_FEATURE_COMMANDS(DCC_ON_SLASH_FN(ping, "Reply with pong")),
        DCC_FEATURE_AUTOCOMPLETES(DCC_AUTOCOMPLETE_FN(ping)),
        DCC_FEATURE_CONTEXT_MENUS(DCC_ON_USER_CONTEXT_MENU_FN(inspect_user)),
        DCC_FEATURE_COMPONENTS(
            DCC_BUTTON_NS_FN("status", refresh),
            DCC_SELECT_NS_FN("roles", choose),
            DCC_MODAL_NS_FN("profile", edit)
        ),
        DCC_FEATURE_EVENTS(DCC_ON_READY_ONCE(on_ready)),
        DCC_FEATURE_TASKS(DCC_TASK_DAILY_KST("09:00", on_tick))
    );
```

Use the explicit route helpers such as `DCC_ON_SLASH("ping", ..., on_ping)`
when the public Discord command name or component `custom_id` should differ
from the C function name. Direct app wiring has the same shape:

```c
DCC_APP_ON_SLASH_FN(app, ping, "Reply with pong");
DCC_APP_ON_BUTTON_NS_FN(app, "status", refresh);
DCC_APP_ON_MODAL_NS_FN(app, "profile", edit);
```

When a command needs builder-only metadata such as DM availability, default
permissions, or NSFW flags, use the builder-value listener form. It accepts the
normal command builder by value, so application code does not need to spell out
compound-literal pointer lifetimes:

```c
DCC_SLASH_FN(play_tictactoe) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "Starting a game.");
}

DCC_SIMPLE_BOT_MAIN(
    "games",
    DCC_LISTEN_COMMAND_BUILDER_VALUE(
        DCC_SLASH_COMMAND_DM("tictactoe", "Start a Tic Tac Toe game", 1U),
        play_tictactoe
    )
)
```

When you want the route metadata to live directly beside the handler, use the
decorator-style helpers. They create a static route descriptor and then declare
the handler with the right signature. Add that descriptor to a feature with
`DCC_ROUTE(handler_name)`.

```c
DCC_DECORATE_SLASH(ping, "Reply with pong") {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_DECORATE_BUTTON_NS(refresh, "status", "refresh") {
    (void)user_data;
    (void)DCC_UPDATEF(ctx, "Refreshed by %s", DCC_CTX_USER_MENTION(ctx));
}

DCC_DECORATE_READY_ONCE(on_ready) {
    (void)app;
    printf("logged in with session %s\n", ready->session_id);
}

dcc_app_extension_t core =
    DCC_FEATURE(
        "core",
        DCC_FEATURE_COMMANDS(DCC_ROUTE(ping)),
        DCC_FEATURE_COMPONENTS(DCC_ROUTE(refresh)),
        DCC_FEATURE_EVENTS(DCC_ROUTE(on_ready))
    );
```

Use `DCC_DECORATE_SLASH_AS()` when the Discord command name should differ from
the C function name, and use the component variants such as
`DCC_DECORATE_BUTTON()`, `DCC_DECORATE_SELECT_NS_PREFIX()`, and
`DCC_DECORATE_MODAL_NS()` for buttons, selects, and modals. These descriptors are
static C objects, so route names, descriptions, custom IDs, prefixes, and KST
task times should be string literals or other static-storage strings.

Decorated routes also have `_DATA`, `_POLICY`, and `_GUARDED` forms for the
routes that support policies. This keeps operational rules beside the handler
instead of splitting them across setup code:

```c
DCC_DECORATE_SLASH_DATA_GUARDED(
    prune,
    "Delete old messages",
    &moderation_state,
    DCC_ROUTE_GUILD_ONLY(),
    DCC_ROUTE_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_MESSAGES),
    DCC_ROUTE_COOLDOWN_USER(1U, DCC_SECONDS(10U))
) {
    moderation_state_t *state = user_data;
    (void)state;
    (void)DCC_RESPOND_EPHEMERAL(ctx, "Pruning...");
}

DCC_DECORATE_BUTTON_NS_DATA_GUARDED(
    approve_ticket,
    "ticket",
    "approve",
    &ticket_state,
    DCC_ROUTE_OWNER(803177060956766231ULL)
) {
    ticket_state_t *state = user_data;
    (void)state;
    (void)DCC_UPDATE(ctx, DCC_MESSAGE_TEXT("Approved."));
}
```

For typed slash commands, use a params decorator. The params table generates the
Discord command options and the C struct binding at the same time:

```c
typedef struct echo_args {
    const char *text;
    int64_t count;
} echo_args_t;

#define ECHO_PARAMS(X, T) \
    X(T, REQUIRED_STRING, text, "text", "Text to echo") \
    X(T, INT, count, "count", "Repeat count", 0U, 1)

DCC_DECORATE_SLASH_PARAMS_DATA_GUARDED(
    echo,
    "Echo text",
    echo_args_t,
    args,
    &state,
    ECHO_PARAMS,
    DCC_ROUTE_GUILD_ONLY()
) {
    (void)args->count;
    (void)DCC_RESPOND_TEXT(ctx, args->text);
}

dcc_app_extension_t utility =
    DCC_FEATURE(
        "utility",
        DCC_FEATURE_COMMAND_ROUTES(DCC_TYPED_ROUTE(echo))
    );
```

Modal, button, and select routes use the same typed decorator shape. This keeps
the Discord `custom_id` convention and the parsed C struct beside the handler:

```c
typedef struct profile_args {
    const char *display_name;
    uint8_t public_profile;
} profile_args_t;

typedef struct page_button_args {
    const char *action;
} page_button_args_t;

typedef struct role_select_args {
    const char *role_id;
} role_select_args_t;

#define PROFILE_FORM(X, T) \
    X(T, REQUIRED_STRING, display_name, "display_name") \
    X(T, BOOL, public_profile, "public_profile", 1U)

#define PAGE_BUTTON(X, T) \
    X(T, REQUIRED_CUSTOM_ID_SUFFIX, action, "page.")

#define ROLE_SELECT(X, T) \
    X(T, REQUIRED_FIRST_VALUE, role_id)

DCC_DECORATE_MODAL_NS_PARAMS_DATA_GUARDED(
    save_profile,
    "profile",
    "edit",
    profile_args_t,
    form,
    &state,
    PROFILE_FORM,
    DCC_ROUTE_GUILD_ONLY()
) {
    (void)form->public_profile;
    (void)DCC_RESPOND_TEXT(ctx, form->display_name);
}

DCC_DECORATE_BUTTON_PREFIX_PARAMS(next_page, "page.", page_button_args_t, button, PAGE_BUTTON) {
    (void)DCC_UPDATEF(ctx, "action=%s", button->action);
}

DCC_DECORATE_SELECT_NS_PARAMS(choose_role, "roles", "choose", role_select_args_t, select, ROLE_SELECT) {
    (void)DCC_RESPOND_EPHEMERALF(ctx, "role=%s", select->role_id);
}

dcc_app_extension_t profile =
    DCC_FEATURE(
        "profile",
        DCC_FEATURE_MODAL_ROUTES(DCC_TYPED_ROUTE(save_profile)),
        DCC_FEATURE_BUTTON_ROUTES(DCC_TYPED_ROUTE(next_page)),
        DCC_FEATURE_SELECT_ROUTES(DCC_TYPED_ROUTE(choose_role))
    );
```

Gateway events can be raw or typed. `DCC_DECORATE_EVENT()` passes the full
`dcc_event_t`, while typed variants unwrap the event payload before calling your
handler:

```c
DCC_DECORATE_MEMBER_JOIN(on_member_join) {
    (void)app;
    (void)member;
    (void)event;
    (void)user_data;
    /* update local state or queue a welcome action */
}

dcc_app_extension_t welcome =
    DCC_FEATURE(
        "welcome",
        DCC_FEATURE_EVENTS(DCC_ROUTE(on_member_join))
    );
```

If you prefer a discord.js-like registration list over COG arrays, use listener
sugar. A listener list registers routes directly into the app while still using
the same descriptors and typed routes:

```c
DCC_SLASH_FN(ping) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_BUTTON_FN(refresh) {
    (void)user_data;
    (void)DCC_UPDATE(ctx, DCC_MESSAGE_TEXT("Refreshed."));
}

DCC_READY_FN(on_ready) {
    (void)user_data;
    printf("ready: %s\n", ready->session_id);
}

DCC_MEMBER_JOIN_FN(on_member_join) {
    (void)app;
    (void)member;
    (void)event;
    (void)user_data;
    /* update local state or queue a welcome action */
}

DCC_TASK_FN(refresh_cache) {
    (void)user_data;
    /* update local state */
}

dcc_app_definition_t bot =
    DCC_SIMPLE_BOT(
        "mybot",
        DCC_LISTEN_SLASH("ping", "Reply with pong", ping),
        DCC_LISTEN_BUTTON_NS("status", "refresh", refresh),
        DCC_LISTEN_READY_ONCE(on_ready),
        DCC_LISTEN_MEMBER_JOIN(on_member_join),
        DCC_LISTEN_TASK_DAILY_KST("09:00", refresh_cache)
    );
```

`DCC_SIMPLE_BOT()` is the listener-list-first form for one-file bots. It is
`DCC_BOT()` plus `DCC_APP_LISTENERS(...)`, so it includes command auto-sync,
ephemeral auto-defer after 1500ms, and default error replies. Use
`DCC_LISTENERS(...)` when you need a standalone listener list, or
`DCC_LISTENERS_REGISTER(app, ...)` for one-off setup code. The older
`DCC_BOT_ROUTES()`, `DCC_ROUTE_*`, and `DCC_ROUTES(...)` names remain
compatible aliases for descriptor-heavy code.

For reusable feature files, `DCC_DEFINE_LISTENERS()` plus
`DCC_LISTENERS_MODULE()` registers a listener table. Typed routes created by
decorators can be registered in the same list with
`DCC_LISTEN_DECORATED_TYPED_COMMAND(handler)`,
`DCC_LISTEN_DECORATED_TYPED_MODAL(handler)`,
`DCC_LISTEN_DECORATED_TYPED_BUTTON(handler)`, and
`DCC_LISTEN_DECORATED_TYPED_SELECT(handler)`. For single-source params routes,
prefer listener-first forms such as `DCC_LISTEN_COMMAND_PARAMS()`,
`DCC_LISTEN_MODAL_PARAMS()`, `DCC_LISTEN_BUTTON_NS_PREFIX_PARAMS()`, and
`DCC_LISTEN_SELECT_NS_PARAMS()`. Typed Gateway handlers created with
`DCC_MEMBER_JOIN_FN()`, `DCC_MESSAGE_CREATE_FN()`, `DCC_ROLE_UPDATE_FN()`, and
the other event handler macros can be registered with matching listener aliases
such as `DCC_LISTEN_MEMBER_JOIN()`.

Decorated handlers can also be registered without repeating their metadata:

```c
DCC_DECORATE_SLASH(ping, "Reply with pong") {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_DECORATE_BUTTON_NS(refresh, "status", "refresh") {
    (void)user_data;
    (void)DCC_UPDATE(ctx, DCC_MESSAGE_TEXT("Refreshed."));
}

DCC_SIMPLE_BOT_MAIN(
    "mybot",
    DCC_LISTEN_DECORATED_COMMAND(ping),
    DCC_LISTEN_DECORATED_BUTTON(refresh)
)
```

Use the matching decorated listener aliases for the route kind:
`DCC_LISTEN_DECORATED_COMMAND()`, `DCC_LISTEN_DECORATED_COMPONENT()`,
`DCC_LISTEN_DECORATED_EVENT()`, `DCC_LISTEN_DECORATED_PREFIX_COMMAND()`,
`DCC_LISTEN_DECORATED_TASK()`, and typed forms such as
`DCC_LISTEN_DECORATED_TYPED_COMMAND()`. The older `DCC_ROUTE_DECORATED_*`
names are equivalent.

For a one-file bot, let DCC generate `main()` from the listener list directly.
The default form loads `.env` and reads `DCC_TOKEN`, `BOT_TOKEN`, or
`DISCORD_TOKEN`. The
`_TOKEN` variants load `.env` and read the token environment variable you name;
the `_ENV` variants skip `.env` loading and read only the current process
environment:

```c
DCC_SIMPLE_BOT_MAIN(
    "mybot",
    DCC_LISTEN_SLASH("ping", "Reply with pong", ping),
    DCC_LISTEN_READY_ONCE(on_ready)
)

DCC_SIMPLE_BOT_MAIN_TOKEN(
    "MY_BOT_TOKEN",
    "mybot",
    DCC_LISTEN_SLASH("ping", "Reply with pong", ping),
    DCC_LISTEN_READY_ONCE(on_ready)
)

DCC_SIMPLE_BOT_MAIN_ENV(
    "MY_BOT_TOKEN",
    "mybot",
    DCC_LISTEN_SLASH("ping", "Reply with pong", ping),
    DCC_LISTEN_READY_ONCE(on_ready)
)
```

Use `DCC_RUN_SIMPLE_BOT()` instead when you want to keep your own `main()`.
For descriptor-heavy apps, `DCC_BOT_MAIN()` mirrors `DCC_RUN_BOT()` and accepts
`DCC_APP_*` fields instead of listener descriptors.

## Client Options

`DCC_SIMPLE_BOT_MAIN()` and `DCC_RUN_BOT()` already load `.env` and read
`DCC_TOKEN`, `BOT_TOKEN`, or `DISCORD_TOKEN`. Use explicit option values when
you are building a custom `main()` or embedding DCC inside another runtime.

```c
dcc_client_options_t options =
    DCC_CLIENT_OPTIONS(token, DCC_INTENTS_DEFAULT);
```

Use every currently supported Gateway intent with `DCC_INTENTS_ALL`. Discord
still requires privileged intents to be enabled in the Developer Portal. For
the common "turn everything on" case, use the `_ALL` aliases:

```c
dcc_client_options_t options =
    DCC_CLIENT_OPTIONS_ALL(token);
```

For sharded clients:

```c
dcc_client_options_t options =
    DCC_CLIENT_SHARDED_OPTIONS(token, DCC_INTENTS_MESSAGES, shard_id, shard_count);

dcc_client_options_t all_intents =
    DCC_CLIENT_SHARDED_OPTIONS_ALL(token, shard_id, shard_count);
```

If you want voice helpers to infer `guild_id` from a cached channel ID, use the
guild-inference client options. This enables the cache and the inference flag.

```c
dcc_client_options_t options =
    DCC_CLIENT_OPTIONS_WITH_GUILD_INFERENCE(token, DCC_INTENTS_ALL);

dcc_client_options_t all_intents =
    DCC_CLIENT_OPTIONS_WITH_GUILD_INFERENCE_ALL(token);

dcc_voice_client_connect(voice, 0, voice_channel_id, 0, 0, 1);
```

For application code, the shortest executable shape is `DCC_RUN_BOT(...)`.
It reads `.env` when present, reads `DCC_TOKEN`, `BOT_TOKEN`, or `DISCORD_TOKEN`, applies
runtime options such as `DCC_STORE_FILE`, runs the app, and destroys it before
returning. `DCC_BOT(...)` includes `DCC_APP_DEV_MODE()` by default.

```c
dcc_status_t status =
    DCC_RUN_BOT(
        "mybot",
        DCC_APP_EXTENSIONS(
            DCC_FEATURE(
                "ping",
                DCC_FEATURE_COMMANDS(DCC_ON_SLASH("ping", "Reply with pong", on_ping))
            )
        ),
        DCC_APP_EVENTS(DCC_ON_READY_ONCE(on_ready)),
        DCC_APP_GUILD_ONLY()
    );
```

`DCC_APP_DEV_MODE()` bundles command auto-sync, ephemeral auto-defer after
1500ms, and default error replies. Use `DCC_APP_DEV_MODE_PLAN()` when you want
the same development defaults but only want to print the command sync plan.
Use `DCC_APP_PRESET_DEV_GUILD(guild_id)` when local development should sync to
one guild instead of global commands.

Declare required deployment variables next to the app definition when startup
should fail early with a clear missing-env diagnostic:

```c
dcc_app_definition_t bot =
    DCC_APP(
        "mybot",
        DCC_REQUIRE_ENV(
            DCC_ENV_DESCRIBED("DATABASE_URL", "database connection string"),
            DCC_ENV("BIRTHDAY_CHANNEL")
        ),
        DCC_APP_PRESET_BOT()
    );
```

When application code needs the typed values, bind the environment once near
startup instead of scattering `getenv()` and `strtoull()` calls through handlers:

```sh
dcc_new_app add config . birthday_channel channel BIRTHDAY_CHANNEL
dcc_new_app add config . birthday_role role TODAY_BIRTHDAY
dcc_new_app add config . refresh_seconds u64 REFRESH_SECONDS
dcc_new_app add error-handler . bot_error friendly
```

Generated apps insert those fields into `src/config.h`, add the matching
`DCC_CONFIG_*` bindings, and update `.env.example`. Feature handlers can access
them through `BOT_CONFIG(user_data)` or `BOT_CTX_CONFIG(ctx)`. For hand-written
apps, the same schema is just C:

```c
typedef struct bot_config {
    const char *database_url;
    dcc_snowflake_t birthday_channel;
    dcc_snowflake_t birthday_role;
    uint8_t debug;
    uint64_t refresh_seconds;
} bot_config_t;

bot_config_t config = {0};
dcc_status_t status =
    DCC_CONFIG(
        DCC_CONFIG_STRING_DESCRIBED(
            "DATABASE_URL",
            "database connection string",
            &config.database_url
        ),
        DCC_CONFIG_CHANNEL("BIRTHDAY_CHANNEL", &config.birthday_channel),
        DCC_CONFIG_ROLE("TODAY_BIRTHDAY", &config.birthday_role),
        DCC_CONFIG_BOOL_DEFAULT("DEBUG", 0U, &config.debug),
        DCC_CONFIG_U64_DEFAULT("REFRESH_SECONDS", 3600U, &config.refresh_seconds)
    );
```

`DCC_CONFIG_CHANNEL`, `DCC_CONFIG_ROLE`, and `DCC_CONFIG_USER` accept either a
raw Discord ID or the matching copied Discord mention (`<#channel>`,
`<@&role>`, `<@user>`). Mismatched mention types are rejected, so putting a role
mention in a channel setting still fails early. `DCC_CONFIG_GUILD` and
`DCC_CONFIG_SNOWFLAKE` stay raw-ID only because Discord has no guild mention
syntax. Use `DCC_ENV_TOKEN(&token)` for DCC's standard token fallback
(`DCC_TOKEN`, `BOT_TOKEN`, then `DISCORD_TOKEN`), or
`DCC_ENV_TOKEN_NAMED("CUSTOM_TOKEN", &token)` when a deployment uses a custom
token variable. Use `DCC_ENV_CHANNEL("NAME", &id)`, `DCC_ENV_ROLE(...)`,
`DCC_ENV_USER(...)`, `DCC_ENV_BOOL(...)`, `DCC_ENV_U64(...)`,
`DCC_ENV_U32_RANGE_OR(...)`, `DCC_ENV_INTENTS(...)`, and the matching `_OR`
forms when a one-off read is cleaner than a schema. Intent env values accept
numeric masks, `0x...` masks, presets such as `all`, `none`, `default`,
`messages`, `message_content`, `privileged`, and `unprivileged`, or small
compositions such as `guilds,message_content,voice`.

```c
(void)dcc_app_load_dotenv();

const char *token = NULL;
if (DCC_ENV_TOKEN(&token) != DCC_OK) {
    fprintf(stderr, "set DCC_TOKEN, BOT_TOKEN, or DISCORD_TOKEN\n");
    return 1;
}

uint32_t shard_count = 1U;
if (DCC_ENV_U32_RANGE_OR("DCC_SHARDS", 1U, 1U, 1024U, &shard_count) != DCC_OK) {
    fprintf(stderr, "DCC_SHARDS must be between 1 and 1024\n");
    return 1;
}

dcc_intents_t intents = DCC_INTENTS_DEFAULT;
if (DCC_ENV_INTENTS_OR("DCC_INTENTS", DCC_INTENTS_DEFAULT, &intents) != DCC_OK) {
    fprintf(stderr, "DCC_INTENTS must be a mask or preset list\n");
    return 1;
}
```

If you prefer configuring a runtime object directly, use the `DCC_APP_ON_*`
aliases. They call the same `dcc_app_*` registration functions but read closer
to discord.js event routing:

```c
dcc_app_t *app = NULL;
dcc_app_create_from_env(NULL, &app);

DCC_APP_ON_READY_ONCE_FN(app, on_ready);
DCC_APP_ON_MESSAGE_CREATE_FN(app, on_message);
DCC_APP_ON_MESSAGE_UPDATE_FN(app, on_message_update);
DCC_APP_ON_PREFIX_COMMAND(app, "!", "ping", on_ping_text);
DCC_APP_ON_SLASH_FN(app, on_ping, "Reply with pong");
DCC_APP_ON_USER_CONTEXT_MENU_FN(app, inspect_user);
DCC_APP_ON_MESSAGE_CONTEXT_MENU_FN(app, inspect_message);
DCC_APP_ON_BUTTON_NS_FN(app, "ping", refresh);
DCC_APP_ON_SELECT_NS_FN(app, "roles", choose);
DCC_APP_ON_MODAL_NS_FN(app, "profile", edit);
```

Narrow modules can include `<dcc/sugar/app_registration.h>` for these direct
registration aliases without pulling in the whole sugar header. The focused
header includes slash/subcommand/autocomplete, context menu, event/message,
prefix command, and component/modal direct registration aliases, including
`_DATA`, `_FN`, namespace, `_POLICY`, and `_GUARDED` variants. Use the explicit
forms when the Discord command name or component ID should differ from the C
handler name.

Route-local policy works the same way for direct app registration, so admin
routes can stay one line instead of registering a route id and then attaching
guards manually:

```c
DCC_APP_ON_SLASH_GUARDED(
    app,
    "reload",
    "Reload config",
    on_reload,
    DCC_ONLY_GUILD(),
    DCC_CAN(DCC_PERMISSION_MANAGE_GUILD),
    DCC_RATE_LIMIT_USER(1U, DCC_SECONDS(5U))
);
```

Common app-wide policy can stay in the same definition:

```c
dcc_app_definition_t bot =
    DCC_APP(
        "adminbot",
        DCC_APP_EXTENSIONS(admin_cog),
        DCC_APP_EVENTS(DCC_READY_DATA(on_ready, &state)),
        DCC_APP_MIDDLEWARE(DCC_EXTENSION_MIDDLEWARE(require_ready, &state)),
        DCC_APP_GUILD_ONLY(),
        DCC_APP_OWNERS(803177060956766231ULL, 123456789ULL),
        DCC_APP_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_MESSAGES),
        DCC_APP_DEFAULT_ERRORS(),
        DCC_APP_ERROR_HANDLER(on_error, &state)
    );
```

`DCC_APP_DEFAULT_ERRORS()` gives you the built-in friendly replies. Add
`DCC_APP_ERROR_HANDLER(...)` when you want one central friendly handler like
discord.py's command error hook:

```c
DCC_FRIENDLY_ERROR_FN(on_error)
```

The generator can wire the same pattern into `src/main.c`:

```sh
dcc_new_app add error-handler . bot_error friendly
```

When you want explicit options instead of environment loading:

```c
dcc_app_options_t options = DCC_APP_OPTIONS_DEV(token);
```

Use `DCC_APP_OPTIONS_DEV_ALL(token)` when a development bot needs every Gateway
intent, or the lower-level `DCC_APP_OPTIONS_ALL(token)` when you do not want
the dev preset's auto-defer and command-sync defaults.

If the app needs small persistent state, let the app own a file-backed store:

```c
dcc_app_options_t options =
    DCC_APP_OPTIONS_WITH_STORE(token, DCC_INTENTS_DEFAULT, "./bot-state.kv");
```

Handlers can read and write that store directly from `ctx`:

```c
DCC_CTX_REQUIRE_STORE(ctx, "App store is not configured.");
uint64_t count = DCC_CTX_STORE_U64_OR(ctx, "ping.count", 0U) + 1U;
DCC_CTX_STORE_SET_U64(ctx, "ping.count", count);
```

Use guild command scope explicitly for fast development syncs:

```c
dcc_app_options_t options =
    DCC_APP_OPTIONS_GUILD(token, DCC_INTENTS_DEFAULT, guild_id);

dcc_app_options_t dev_options =
    DCC_APP_OPTIONS_DEV_GUILD(token, guild_id);

dcc_app_options_t all_intents =
    DCC_APP_OPTIONS_DEV_GUILD_ALL(token, guild_id);
```

Add app-level auto-defer when handlers may take longer than the interaction
deadline. Response-state shortcuts such as `DCC_CTX_DEFERRED(ctx)`,
`DCC_CTX_RESPONDED(ctx)`, and `DCC_CTX_CAN_FOLLOWUP(ctx)` include this automatic
defer state.

```c
dcc_app_options_t options =
    DCC_APP_OPTIONS_AUTO_DEFER_PRIVATE_DEFAULT(token, DCC_INTENTS_DEFAULT);
```

Use duration helpers when an API expects milliseconds. They keep route
cooldowns, waiter timeouts, auto-defer thresholds, and session TTLs readable:

```c
DCC_APP_OPTIONS_AUTO_DEFER_PRIVATE(token, DCC_INTENTS_DEFAULT, DCC_MS(1500U));
DCC_COOLDOWN_USER(3U, DCC_SECONDS(10U));
DCC_WAIT_BUTTON(ctx, "confirm", (uint32_t)DCC_SECONDS(30U), &result);

uint32_t timeout_ms = 0U;
if (DCC_TIMEOUT_MINUTES_CHECKED(5U, &timeout_ms) != DCC_OK) {
    DCC_CTX_BAD_INPUT(ctx, "Timeout is too large.");
    return;
}
```

The unchecked macros are intended for small literals and compile-time constants:
`DCC_MS`, `DCC_SECONDS`, `DCC_MINUTES`, `DCC_HOURS`, and `DCC_DAYS`. Use the
`*_CHECKED` helpers for values loaded from env files, command options, or other
user input.

Enable command reconciliation on READY when you want the app to sync slash
commands as it starts. The auto-sync forms infer the application id from READY
and inherit the app's configured command scope, which is global unless you use a
guild app option:

```c
dcc_app_options_t options =
    DCC_APP_OPTIONS_WITH_AUTO_SYNC(token, DCC_INTENTS_DEFAULT);

dcc_app_options_t guild_options =
    DCC_APP_OPTIONS_GUILD_WITH_AUTO_SYNC(token, DCC_INTENTS_DEFAULT, guild_id);
```

The auto-sync forms infer the application id from the READY event. Use the
explicit forms when a deployment wants to pin an application id without waiting
for gateway identity:

```c
dcc_app_options_t options =
    DCC_APP_OPTIONS_WITH_COMMAND_SYNC(token, DCC_INTENTS_DEFAULT, application_id);

dcc_app_options_t guild_options =
    DCC_APP_OPTIONS_GUILD_WITH_COMMAND_SYNC(
        token,
        DCC_INTENTS_DEFAULT,
        application_id,
        guild_id
    );
```

Use `DCC_APP_COMMAND_SYNC_PLAN()` or `DCC_APP_COMMAND_SYNC_GUILD_PLAN()` when
you want to fetch and log the reconciliation plan without writing to Discord.
The matching `DCC_APP_COMMAND_SYNC_AUTO_PLAN()` and
`DCC_APP_COMMAND_SYNC_GUILD_AUTO_PLAN()` forms keep application id inference.
Use `DCC_APP_AUTO_SYNC_GLOBAL()` or `DCC_APP_AUTO_SYNC_GUILD(guild_id)` when a
definition should ignore the app options and force a specific command scope.

Share process-wide bot state without threading it through every route:

```c
typedef struct bot_state {
    unsigned requests;
} bot_state_t;

bot_state_t state = {0};
dcc_app_set_state(app, &state, NULL);

static void on_ping(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    bot_state_t *state = DCC_CTX_STATE(bot_state_t, ctx);
    if (state != NULL) {
        state->requests++;
    }
    DCC_REPLY_DONE(ctx, "pong");
}
```

Group a feature into a feature setup function when you want one file to own
its routes, events, and tasks. The setup function registers a temporary
extension immediately, so the inline arrays stay scoped to that setup call.
The example assumes normal app handlers named `on_ping`, `on_ready`, and
`refresh_cache` already exist:

```c
DCC_DEFINE_FEATURE(
    ping,
    "ping",
    DCC_FEATURE_COMMANDS(DCC_COMMAND_DATA("ping", "Reply with pong", on_ping, user_data)),
    DCC_FEATURE_MESSAGE_COMMANDS(DCC_MESSAGE_COMMAND_DATA("!", "ping", on_message_ping, user_data)),
    DCC_FEATURE_CONTEXT_MENUS(
        DCC_USER_CONTEXT_MENU_DATA("Inspect user", on_inspect_user, user_data),
        DCC_MESSAGE_CONTEXT_MENU_DATA("Quote message", on_quote_message, user_data)
    ),
    DCC_FEATURE_COMPONENTS(DCC_BUTTON_DATA("ping.refresh", on_ping, user_data)),
    DCC_FEATURE_EVENTS(DCC_ON_READY_ONCE_DATA(on_ready, user_data)),
    DCC_FEATURE_TASKS(DCC_TASK_DAILY_KST_DATA("00:00", refresh_cache, user_data))
)

dcc_app_module_t ping_module = DCC_USE_FEATURE(ping, &state);
dcc_app_module_register(app, &ping_module);
```

Use `DCC_APP_EVENTS(...)` when the callback is app-wide, or
`DCC_FEATURE_EVENTS(...)` when it belongs to one feature file. The short forms such
as `DCC_ON_SLASH`, `DCC_ON_READY`, `DCC_ON_READY_ONCE`, `DCC_BUTTON`, and
`DCC_TASK_LOOP_SECONDS` use `NULL` route data. Use the matching `_DATA` form when
a handler needs module state. Use `DCC_ON_READY_ONCE` for setup that must not run
again after a reconnect. The older `DCC_EVERY_*` names are still available;
`DCC_TASK_LOOP_*` and `DCC_TASK_DAILY_KST` read closer to discord.py's
`tasks.loop`.

User and message context menus use the same feature route shape. DCC registers the
Discord application command and routes the interaction by command name:

```c
static void on_inspect_user(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    const dcc_user_t *target = DCC_CTX_TARGET_USER(ctx);
    DCC_REPLY_PRIVATEF(ctx, "User id: %llu", (unsigned long long)DCC_CTX_TARGET_ID(ctx));
    (void)target;
}

static void on_quote_message(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    const dcc_message_t *message = DCC_CTX_TARGET_MESSAGE(ctx);
    DCC_REPLY_PRIVATE(ctx, message != NULL ? message->content : "Message not cached in interaction payload.");
}

DCC_DEFINE_COG(
    inspect,
    "inspect",
    DCC_FEATURE_CONTEXT_MENUS(
        DCC_ON_USER_CONTEXT_MENU("Inspect user", on_inspect_user),
        DCC_ON_MESSAGE_CONTEXT_MENU("Quote message", on_quote_message)
    )
)
```

When you are wiring an app manually, use `DCC_APP_ON_USER_CONTEXT_MENU()` and
`DCC_APP_ON_MESSAGE_CONTEXT_MENU()`. The `_DATA`, `_POLICY`, and `_GUARDED`
variants match slash commands and component routes.

The generator can create the boilerplate and update `commands.json` for both
types:

```sh
dcc_new_app add context-menu . profile inspect_user user --guild-only
dcc_new_app add context-menu . profile quote_message message
```

Typed Gateway event handlers remove the repeated raw event accessor step for
common payloads:

```c
DCC_MEMBER_JOIN_FN(on_member_join) {
    bot_state_t *state = user_data;
    DCC_APP_SENDF(
        state->app,
        state->log_channel,
        "Welcome %s",
        DCC_MENTION_USER(member->user.id)
    );
}

DCC_ROLE_UPDATE_FN(on_role_update) {
    bot_state_t *state = user_data;
    DCC_APP_SENDF(app, state->log_channel, "Role updated: %s", role->name);
}

DCC_DEFINE_COG(
    welcome,
    "welcome",
    DCC_FEATURE_EVENTS(
        DCC_MEMBER_JOIN_DATA(on_member_join, user_data),
        DCC_ON_ROLE_UPDATE_DATA(on_role_update, user_data)
    )
)
```

The handler still receives the original `dcc_event_t *event` as the third
argument when lower-level details are needed. Direct registration is also
available with helpers such as `DCC_APP_ON_MEMBER_JOIN_DATA(app, on_member_join,
state)`.

For multi-file apps, expose a public feature setup from the feature `.c` file and
load it from the app entrypoint. The `DCC_FEATURE_*` names are the preferred
sugar-first spelling; the older `DCC_COG_*` names remain compatible.

```c
/* birthday.h */
DCC_DECLARE_FEATURE(birthday);

/* birthday.c */
DCC_DEFINE_PUBLIC_FEATURE(
    birthday,
    "birthday",
    DCC_FEATURE_COMMANDS(DCC_COMMAND_DATA("birthday", "Show birthday help", on_birthday, user_data)),
    DCC_FEATURE_COMPONENTS(DCC_BUTTON_PREFIX_DATA("birthday.", on_birthday_button, user_data))
)

/* main.c */
dcc_app_definition_t bot =
    DCC_APP(
        "mybot",
        DCC_APP_USE(DCC_USE_FEATURE(birthday, &state)),
        DCC_APP_DEFAULT_ERRORS()
    );
```

`DCC_LOAD_FEATURE(app, ping)` and `DCC_LOAD_FEATURE_DATA(app, ping, &state)` are useful
when you are building an app object manually and want to register a feature directly
instead of wrapping it as a module.

Message prefix commands use a message-command handler, not an interaction
`dcc_ctx_t` handler. Prefer the `DCC_PREFIX_COMMAND*` aliases in new code; the
older `DCC_MESSAGE_COMMAND*` names are kept for compatibility.

```c
DCC_PREFIX_COMMAND_FN(on_message_ping) {
    (void)args;
    (void)event;
    (void)user_data;
    dcc_app_send_text(app, message->channel_id, "pong", NULL, NULL);
}

dcc_app_extension_t messages =
    DCC_FEATURE(
        "messages",
        DCC_FEATURE_MESSAGE_COMMANDS(
            DCC_PREFIX_COMMAND("!", "ping", on_message_ping)
        )
    );

DCC_APP_ON_PREFIX_COMMAND(app, "!", "ping", on_message_ping);
```

Guards, checks, cooldowns, and middleware can stay declarative too. Put defaults
on the app when every interaction should share them:

```c
dcc_app_definition_t bot =
    DCC_APP(
        "admin-bot",
        DCC_APP_GUILD_ONLY(),
        DCC_APP_CHANNEL_TYPES(DCC_CHANNEL_TEXT),
        DCC_APP_OWNERS(owner_user_id, maintainer_user_id),
        DCC_APP_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_MESSAGES),
        DCC_APP_REQUIRE_ANY_ROLE(admin_role_id, moderator_role_id),
        DCC_APP_CHECK_MESSAGE(require_ready, &state, "Not ready", "Try again after setup finishes."),
        DCC_APP_COOLDOWN_USER(3U, DCC_SECONDS(10U)),
        DCC_APP_TASKS(DCC_TASK_LOOP_MINUTES_DATA(10U, refresh_cache, &state)),
        DCC_APP_EXTENSIONS(admin_cog)
    );
```

Use a route policy when one command or component needs stricter access than the
app default:

```c
dcc_app_extension_t admin =
    DCC_FEATURE(
        "admin",
        DCC_FEATURE_COMMANDS(
            DCC_COMMAND_DATA_GUARDED(
                "reload",
                "Reload config",
                on_reload,
                &state,
                DCC_USE(DCC_MIDDLEWARE(require_ready, &state)),
                DCC_ONLY_GUILD(),
                DCC_IN_CHANNEL_TYPES(DCC_CHANNEL_TEXT),
                DCC_OWNERS_ONLY(owner_user_id, maintainer_user_id),
                DCC_CAN(DCC_PERMISSION_MANAGE_MESSAGES),
                DCC_HAS_ANY_ROLE(admin_role_id, moderator_role_id),
                DCC_WITH_CHECK_MESSAGE(
                    require_ready,
                    &state,
                    "Not ready",
                    "Try again after setup finishes."
                ),
                DCC_RATE_LIMIT_USER(1U, DCC_SECONDS(5U))
            )
        ),
        DCC_FEATURE_COMPONENTS(
            DCC_BUTTON_DATA_GUARDED(
                "admin.confirm",
                on_confirm,
                &state,
                DCC_ONLY_GUILD(),
                DCC_CAN(DCC_PERMISSION_MANAGE_MESSAGES)
            )
        )
    );
```

The app generator can create a feature middleware skeleton when the check belongs in
one feature file:

```sh
dcc_new_app add middleware . profile require_config config
dcc_new_app add middleware . profile guild_only guild
```

The generated middleware uses `DCC_MIDDLEWARE_FN(...)` and registers through
`DCC_FEATURE_MIDDLEWARE(DCC_MIDDLEWARE_DATA(...))`, so it is easy to edit into a
state check, telemetry hook, or shared request validator.

If you prefer an explicit reusable policy, keep using `DCC_POLICY(...)` or
`DCC_GUARDS(...)` and pass it to the matching `*_POLICY` alias:

```c
dcc_app_route_policy_t admin_policy =
    DCC_POLICY(
        DCC_ONLY_GUILD(),
        DCC_CAN(DCC_PERMISSION_MANAGE_MESSAGES),
        DCC_RATE_LIMIT_USER(1U, DCC_SECONDS(5U))
    );

DCC_MODAL_PREFIX_DATA_POLICY("admin.modal:", on_modal, &state, admin_policy);
DCC_APP_ON_BUTTON_DATA_POLICY(app, "admin.confirm", on_confirm, &state, admin_policy);
```

For persistent buttons, selects, and modals, keep `custom_id` values stable and
group them by a string-literal namespace. The `*_NS` aliases concatenate the
namespace and action at compile time, so `"birthday"` and `"register"` become
`"birthday.register"` without a runtime allocation:

```c
dcc_app_extension_t birthday =
    DCC_FEATURE(
        "birthday",
        DCC_FEATURE_COMPONENTS(
            DCC_BUTTON_NS_DATA("birthday", "register", on_register, &state),
            DCC_SELECT_NS_PREFIX_DATA("birthday", "schedule:", on_schedule_select, &state),
            DCC_MODAL_NS_DATA("birthday", "submit", on_submit, &state)
        )
    );
```

The same pattern works when registering routes on an already-created app:

```c
DCC_APP_ON_BUTTON_NS(app, "birthday", "register", on_register);
DCC_APP_ON_MODAL_NS_PREFIX_DATA(app, "birthday", "edit:", on_edit_modal, &state);
DCC_APP_ON_BUTTON_NS_DATA_GUARDED(
    app,
    "admin",
    "confirm",
    on_admin_confirm,
    &state,
    DCC_ONLY_GUILD(),
    DCC_CAN(DCC_PERMISSION_MANAGE_MESSAGES)
);
```

Use the matching builder aliases when creating the message or modal that emits
those IDs:

```c
dcc_message_builder_t registration =
    DCC_UI_MESSAGE(
        DCC_UI_CARD_ACCENT(
            DCC_COLOR_BLURPLE,
            DCC_UI_TEXT("## Birthday registration"),
            DCC_UI_ROW(DCC_UI_PRIMARY_NS("Register", "birthday", "register")),
            DCC_UI_STRING_SELECT_NS(
                "birthday",
                "schedule",
                DCC_SELECT_OPTION("This month", "month")
            )
        )
    );

dcc_modal_builder_t birthday_modal =
    DCC_MODAL(
        DCC_COMPONENT_ID("birthday", "submit"),
        "Birthday",
        DCC_MODAL_FIELD_TEXT_PLACEHOLDER(
            DCC_COMPONENT_ID("birthday", "date"),
            "Birthday",
            "YYYYMMDD"
        )
    );
```

Use the lower-level `DCC_BUTTON_PREFIX()` or `DCC_APP_ON_BUTTON_PREFIX()`
forms when the namespace or action is not a C string literal.

Typed modal/button/select routes can use the same namespace split. That keeps
the button builder, modal builder, and route declaration on one ID convention
without duplicating string literals:

```c
typedef struct birthday_form {
    const char *date;
    uint8_t show_age;
} birthday_form_t;

#define BIRTHDAY_MODAL_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, date, "birthday.date") \
    PARAM(type, BOOL, show_age, "birthday.show_age", 1U)

#define BIRTHDAY_BUTTON_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_CUSTOM_ID_SUFFIX, date, "birthday.schedule:")

dcc_modal_route_t birthday_submit =
    DCC_MODAL_NS_PARAMS_DATA(
        "birthday",
        "submit",
        birthday_form_t,
        on_birthday_submit,
        state,
        BIRTHDAY_MODAL_PARAMS
    );

dcc_button_route_t birthday_schedule =
    DCC_BUTTON_NS_PREFIX_PARAMS_DATA(
        "birthday",
        "schedule:",
        birthday_form_t,
        on_birthday_schedule,
        state,
        BIRTHDAY_BUTTON_PARAMS
    );
```

When the action is the same as the handler name, use the `_FN` aliases and keep
the namespace as the only string literal:

```c
dcc_modal_route_t birthday_submit =
    DCC_MODAL_NS_PARAMS_FN_DATA(
        "birthday",
        submit,
        birthday_form_t,
        state,
        BIRTHDAY_MODAL_PARAMS
    );

dcc_button_route_t birthday_schedule =
    DCC_BUTTON_NS_PREFIX_PARAMS_FN_DATA(
        "birthday",
        schedule,
        birthday_form_t,
        state,
        BIRTHDAY_BUTTON_PARAMS
    );
```

```c
dcc_modal_flow_t birthday_flow =
    DCC_MODAL_V2_FLOW_NS_PARAMS_DATA(
        "birthday",
        "submit",
        "Birthday",
        birthday_form_t,
        on_birthday_submit,
        state,
        BIRTHDAY_MODAL_PARAMS,
        DCC_UI_LABEL("Birthday", DCC_UI_INPUT_NS("birthday", "date", "Birthday", 1U)),
        DCC_UI_LABEL("Show age", DCC_UI_CHECKBOX_NS("birthday", "show_age", "Show age", 1U))
    );
```

The modal flow also has `_FN` forms when the route action should match the
handler name:

```c
dcc_modal_flow_t birthday_flow =
    DCC_MODAL_V2_FLOW_NS_PARAMS_FN_DATA(
        "birthday",
        "Birthday",
        birthday_form_t,
        submit,
        state,
        BIRTHDAY_MODAL_PARAMS,
        DCC_UI_LABEL("Birthday", DCC_UI_INPUT_NS("birthday", "date", "Birthday", 1U)),
        DCC_UI_LABEL("Show age", DCC_UI_CHECKBOX_NS("birthday", "show_age", "Show age", 1U))
    );
```

For DM-only interactions, use `DCC_ONLY_DM()` in the same policy position where
a guild route would use `DCC_ONLY_GUILD()`. For channel-restricted commands,
use `DCC_ONLY_NSFW()` to require the cached channel to be age-restricted.
Use `DCC_IN_CHANNEL_TYPES(...)` to restrict a handler to channel types, and
`DCC_HAS_ROLE(...)`, `DCC_HAS_ROLES(...)`, or `DCC_HAS_ANY_ROLE(...)` for role
checks. These macros hide the role and channel-type arrays and fill the
matching counts. The longer `DCC_GUARD_*` names remain available for codebases
that prefer explicit naming.

Permission sugar keeps common permission sets readable when a route needs more
than one bit:

```c
DCC_APP_ON_SLASH_POLICY(
    app,
    "clean",
    "Clean up messages",
    on_clean,
    DCC_GUARDS(
        DCC_GUARD_GUILD_ONLY(),
        DCC_GUARD_HAS_PERMISSIONS(
            DCC_PERMISSIONS(
                DCC_PERMISSION_MANAGE_MESSAGES,
                DCC_PERMISSION_READ_MESSAGE_HISTORY
            )
        )
    )
);
```

Preset sets such as `DCC_PERMISSION_TEXT_RICH`,
`DCC_PERMISSION_MODERATION_BASIC`, and `DCC_PERMISSION_VOICE_BASIC` cover common
bot checks without hand-writing the bit mask.

Cooldown sugar uses the same policy shape. Use `DCC_GUARD_COOLDOWN_USER`,
`DCC_GUARD_COOLDOWN_CHANNEL`, `DCC_GUARD_COOLDOWN_GUILD`, or
`DCC_GUARD_COOLDOWN_GLOBAL` inside `DCC_GUARDS(...)`. The lower-level
`DCC_ROUTE_*` and `DCC_COOLDOWN_*` names remain available for explicit route APIs.

Checks are lighter than middleware when a route only needs a predicate. Return
`DCC_OK` to continue; any other status sends the configured error response and
stops the handler.

Typed routes can live in the same feature, so a feature file can keep its command
schema, handlers, validation, component routes, events, and tasks together:

```c
dcc_app_extension_t search_cog =
    DCC_FEATURE(
        "search",
        DCC_FEATURE_COMMAND_ROUTES(search),
        DCC_FEATURE_MODAL_ROUTES(profile_modal),
        DCC_FEATURE_BUTTON_ROUTES(page_button),
        DCC_FEATURE_SELECT_ROUTES(role_select)
    );
```

Use a module setup function when the feature needs custom allocation,
dependency wiring, or cleanup:

```c
static dcc_status_t ping_module_setup(dcc_app_t *app, void *user_data) {
    return DCC_APP_ON_SLASH_DATA(app, "ping", "Reply with pong", on_ping, user_data);
}

dcc_app_module_t ping_module =
    DCC_APP_MODULE("ping", ping_module_setup, &state);

dcc_app_module_register(app, &ping_module);
```

## Commands

```c
dcc_application_command_builder_t ping =
    DCC_SLASH_CMD("ping", "Latency check");

dcc_application_command_builder_t inspect_user =
    DCC_USER_COMMAND_BUILDER("Inspect user");

dcc_application_command_builder_t quote_message =
    DCC_MESSAGE_COMMAND_BUILDER("Quote message");
```

Slash options can stay inline. DCC serializes them into the command JSON when
the command is registered or synced.

```c
dcc_application_command_builder_t search =
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
        DCC_OPTIONAL_INT_RANGE("limit", "Result count", 1, 25),
        DCC_OPTIONAL_CHANNEL_TYPES("channel", "Restrict to channel", 0U, 5U)
    );
```

`DCC_REQUIRED_*` and `DCC_OPTIONAL_*` are the sugar-first command option
builders. The older `DCC_OPT_*` names remain available when you want to pass the
Discord `required` flag explicitly.

Static slash command choices use the same shape:

```c
DCC_REQUIRED_INT_CHOICES(
    "priority",
    "Priority",
    DCC_INT_CHOICE("Low", 1),
    DCC_INT_CHOICE("High", 5)
)
```

Autocomplete choices can also stay inline inside an app autocomplete handler:

```c
static void on_search_autocomplete(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    DCC_REPLY_AUTOCOMPLETE_MATCHING(
        ctx,
        DCC_AUTOCOMPLETE_STRING_CHOICE("Feral", "feral"),
        DCC_AUTOCOMPLETE_STRING_CHOICE("Support", "support"),
        DCC_AUTOCOMPLETE_STRING_CHOICE("Feature requests", "features")
    );
}
```

`DCC_REPLY_AUTOCOMPLETE_MATCHING()` reads `DCC_CTX_FOCUSED_STRING(ctx, "")` and
returns the first matching choices by case-insensitive prefix on the display
name or string value. Use `DCC_REPLY_AUTOCOMPLETE()` when you already built the
exact response yourself.

Use typed slash commands when a handler should receive an already-bound request
struct. DCC registers the command builder, binds options into the struct,
validates it, sends an ephemeral validation error on failure, and then calls the
typed handler.

```c
typedef struct search_args {
    const char *name;
    const dcc_user_t *user;
    int64_t limit;
} search_args_t;

#define SEARCH_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, name, "name", "Member name") \
    PARAM(type, USER, user, "user", "Restrict to a user", 0U) \
    PARAM(type, INT_RANGE, limit, "limit", "Result count", 0U, 10, 1, 25)

static void on_search(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)ctx;
    (void)user_data;

    const search_args_t *search = args;
    /* search->name, search->user, and search->limit are ready. */
}

dcc_command_route_t search =
    DCC_COMMAND_ROUTE_VALIDATED(
        "search",
        "Search members",
        search_args_t,
        on_search,
        SEARCH_PARAMS,
        DCC_VALIDATORS(
            DCC_VALIDATE_STRING_LENGTH(search_args_t, name, "name", 1U, 32U),
            DCC_VALIDATE_INT_RANGE(search_args_t, limit, "limit", 1, 25)
        )
    );

DCC_APP_REGISTER_COMMAND(app, search);
```

`DCC_COMMAND_ROUTE_NO_OPTIONS*` covers commands that do not take arguments.
`DCC_COMMAND_ROUTE*` expands the same parameter list into both Discord
command options and struct field bindings for commands that do. Common parameter kinds include
`STRING`, `REQUIRED_STRING`, `INT`, `INT_RANGE`, `NUMBER`, `NUMBER_RANGE`,
`BOOL`, `USER`, `MEMBER`, `ROLE`, `CHANNEL`, `CHANNEL_TYPES`, and
`ATTACHMENT`.

For listener-table apps, use the same single-source params list directly in the
bot definition:

```c
DCC_SIMPLE_BOT_MAIN(
    "searchbot",
    DCC_LISTEN_COMMAND_PARAMS_GUARDED(
        "search",
        "Search members",
        search_args_t,
        on_search,
        SEARCH_PARAMS,
        DCC_ONLY_GUILD()
    )
)
```

`DCC_LISTEN_COMMAND_FN()` and `DCC_LISTEN_COMMAND_FN_DATA_GUARDED()` use the
handler function name as the slash command name, which keeps small commands
close to the discord.py decorator style while still generating the Discord
schema and typed bindings in C. The `DCC_ROUTE_COMMAND_FN*` names remain
compatible aliases.

Use `DCC_COMMAND_PARAMS("search", "Search members", search_args_t, SEARCH_PARAMS)`
when you only need the Discord command schema, for example to place the command
builder in a registry or to share the same schema with a manually routed handler.
Use `DCC_COMMAND_PARAMS_FN(search, "Search members", search_args_t, SEARCH_PARAMS)`
when the slash command name should be the same as the handler/schema function
name.

Use the `_DATA` variant when the route needs user data:

```c
dcc_command_route_t search =
    DCC_COMMAND_ROUTE_DATA(
        "search",
        "Search members",
        search_args_t,
        on_search,
        state,
        SEARCH_PARAMS
    );
```

When the command name is the handler name, use the `_FN` route aliases:

```c
dcc_command_route_t search =
    DCC_COMMAND_ROUTE_FN_DATA(
        search,
        "Search members",
        search_args_t,
        state,
        SEARCH_PARAMS
    );

dcc_autocomplete_route_t search_autocomplete =
    DCC_AUTOCOMPLETE_PARAMS_FN_DATA(
        search,
        search_args_t,
        state,
        SEARCH_PARAMS
    );
```

For apps you are wiring manually, the `DCC_APP_*` direct registration forms
remove the temporary route variable entirely:

```c
DCC_APP_SLASH_ROUTE_FN_DATA(
    app,
    search,
    "Search members",
    search_args_t,
    state,
    SEARCH_PARAMS
);

DCC_APP_AUTOCOMPLETE_PARAMS_FN_DATA(
    app,
    search,
    search_args_t,
    state,
    SEARCH_PARAMS
);
```

The same direct registration pattern works for typed modal flows and component
routes:

```c
DCC_APP_MODAL_V2_FLOW_NS_PARAMS_FN_DATA(
    app,
    "birthday",
    "Birthday",
    birthday_form_t,
    submit,
    state,
    BIRTHDAY_MODAL_PARAMS,
    DCC_UI_LABEL("Birthday", DCC_UI_INPUT_NS("birthday", "date", "Birthday", 1U))
);

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

When you need full manual control over option builders and bindings, use
`DCC_ON_TYPED_SLASH(...)` or `DCC_ON_TYPED_SLASH_DATA(...)` and pass
`DCC_ARGS(...)` plus `DCC_OPT_*` explicitly.

The older `DCC_TYPED_SLASH_OPTIONS(...)` and
`DCC_TYPED_SLASH_OPTIONS_DATA(...)` names remain available. New code should
prefer `DCC_COMMAND_ROUTE_NO_OPTIONS*` for empty commands,
`DCC_COMMAND_ROUTE*` for ordinary commands with options, or
`DCC_ON_TYPED_SLASH*` when it needs manual option-builder control.

Object bindings such as `DCC_ARG_USER`, `DCC_ARG_MEMBER`, `DCC_ARG_ROLE`,
`DCC_ARG_CHANNEL`, `DCC_ARG_MESSAGE`, and `DCC_ARG_ATTACHMENT` resolve the
option snowflake to a borrowed object using the interaction's resolved payload
first, then the enabled cache when that object type is cached. Use the
`DCC_ARG_REQUIRED_*` variants when unresolved input should trigger the default
validation reply.

Subcommands can be typed too. Put the Discord command tree in the parent
builder, then route each subcommand path to a handler that receives the bound
argument struct:

```c
typedef struct ban_args {
    const char *name;
    int64_t days;
} ban_args_t;

#define BAN_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, name, "name", "Member name") \
    PARAM(type, INT_RANGE, days, "days", "Delete message days", 0U, 0, 0, 7)

static void on_ban(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)user_data;

    const ban_args_t *ban = args;
    (void)ban;
    DCC_REPLY_DONE(ctx, "Ban queued.");
}

dcc_application_command_builder_t admin_command =
    DCC_CMD_TREE(
        "admin",
        "Admin tools",
        DCC_CMD_SUBGROUP(
            "moderation",
            "Moderation",
            DCC_CMD_SUB_PARAMS(
                "ban",
                "Ban a member",
                ban_args_t,
                BAN_PARAMS
            )
        )
    );

dcc_subcommand_route_t ban_command =
    DCC_ON_SUBCOMMAND_BUILDER_PARAMS_IN_DATA_VALIDATED(
        &admin_command,
        "moderation",
        "ban",
        ban_args_t,
        on_ban,
        state,
        BAN_PARAMS,
        DCC_VALIDATORS(
            DCC_VALIDATE_STRING_REQUIRED(ban_args_t, name, "name"),
            DCC_VALIDATE_INT_RANGE(ban_args_t, days, "days", 0, 7)
        )
);

DCC_APP_REGISTER_SUBCOMMAND(app, ban_command);
```

`DCC_CMD`, `DCC_CMD_OPTIONS`, `DCC_CMD_TREE`, `DCC_CMD_SUBGROUP`, and
`DCC_CMD_SUB` are thin aliases for the longer command-builder names.
`DCC_CMD_SUB_PARAMS` builds the Discord subcommand schema from the same
`*_PARAMS` list, and
`DCC_ON_SUBCOMMAND_BUILDER_PARAMS*` binds the handler from that list. Use the
`*_IN` variants when a subcommand lives under a group and you want DCC to form
the path from two string literals:

```c
DCC_SUBCOMMAND_ROUTE_IN_DATA(
    "admin",
    "moderation",
    "ban",
    ban_args_t,
    on_ban,
    state,
    BAN_PARAMS
);
```

Use `DCC_SUBCOMMAND_ROUTE*` when the parent slash command has already been
registered elsewhere and you already have a full path. `DCC_FEATURE_SUBCOMMAND_ROUTES` and
`DCC_APP_SUBCOMMAND_ROUTES` keep these routes declarative inside features and app
definitions.

For listener-table bots, use the matching listener aliases directly inside
`DCC_SIMPLE_BOT()` or `DCC_SIMPLE_BOT_MAIN()`:

```c
DCC_SIMPLE_BOT_MAIN(
    "adminbot",
    DCC_LISTEN_COMMAND_SCHEMA(
        DCC_CMD_TREE(
            "admin",
            "Admin tools",
            DCC_CMD_GROUP(
                "moderation",
                "Moderation",
                DCC_CMD_SUB(
                    "ban",
                    "Ban a member",
                    DCC_REQUIRED_USER("name", "Member name"),
                    DCC_OPTIONAL_INT_RANGE("days", "Delete message days", 0, 7)
                )
            )
        )
    ),
    DCC_LISTEN_SUBCOMMAND_IN_PARAMS_DATA_GUARDED(
        "admin",
        "moderation",
        "ban",
        ban_args_t,
        on_ban,
        state,
        BAN_PARAMS,
        DCC_ONLY_GUILD(),
        DCC_CAN(DCC_PERMISSION_BAN_MEMBERS)
    )
)
```

`DCC_LISTEN_COMMAND_SCHEMA()` registers a command tree without adding a handler.
Put it beside `DCC_LISTEN_SUBCOMMAND_*` routes when you want the listener table
to be the single source for both Discord command registration and runtime
routing.

Use `DCC_LISTEN_SUBCOMMAND_PARAMS()` when you already have a full path such as
`"moderation/ban"`, `DCC_LISTEN_SUBCOMMAND_IN_PARAMS()` when the group and
subcommand are separate string literals, and `DCC_LISTEN_SUBCOMMAND_IN_FN()`
when the handler function name should become the subcommand name. Subcommand
autocomplete has the same shape through the `DCC_LISTEN_AUTOCOMPLETE_*` aliases.

Typed autocomplete uses the same option binding shape. The handler receives the
currently focused query through the bound struct and can answer with inline
choices:

```c
typedef struct search_auto_args {
    const char *name;
} search_auto_args_t;

#define SEARCH_AUTO_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, name, "name", "Member name")

static void on_search_autocomplete(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)user_data;

    const search_auto_args_t *auto_args = args;
    (void)auto_args; /* auto_args->name is the focused text. */

    DCC_REPLY_AUTOCOMPLETE_MATCHING(
        ctx,
        DCC_AUTOCOMPLETE_STRING_CHOICE("Feral", "feral"),
        DCC_AUTOCOMPLETE_STRING_CHOICE("Support", "support"),
        DCC_AUTOCOMPLETE_STRING_CHOICE("Feature requests", "features")
    );
}

dcc_application_command_builder_t search_command =
    DCC_SLASH_OPTIONS(
        "search",
        "Search members",
        DCC_REQUIRED_STRING_AUTOCOMPLETE("name", "Member name")
    );

dcc_autocomplete_route_t search_autocomplete =
    DCC_AUTOCOMPLETE_BUILDER_PARAMS_DATA(
        &search_command,
        search_auto_args_t,
        on_search_autocomplete,
        state,
        SEARCH_AUTO_PARAMS
    );

DCC_APP_REGISTER_AUTOCOMPLETE(app, search_autocomplete);
```

For subcommands, route the autocomplete to the same path used by the command
handler:

```c
dcc_autocomplete_route_t ban_name_autocomplete =
    DCC_ON_SUBCOMMAND_AUTOCOMPLETE_BUILDER_PARAMS_DATA(
        &admin_command,
        "moderation/ban",
        search_auto_args_t,
        on_search_autocomplete,
        state,
        SEARCH_AUTO_PARAMS
    );
```

`DCC_FEATURE_AUTOCOMPLETE_ROUTES` and `DCC_APP_AUTOCOMPLETE_ROUTES` keep typed
autocomplete routes declarative inside features and app definitions. When binding or
validation fails, DCC replies with an empty autocomplete response instead of an
error message, which matches Discord's autocomplete interaction contract.

Inside command handlers, bind options straight into a small request struct when
the handler has more than one or two inputs. Use
`DCC_CTX_BIND_OPTION_FIELDS_OR_REPLY()` and `DCC_CTX_VALIDATE_OR_REPLY()` when
missing or invalid input should immediately return a friendly ephemeral embed
response from the handler:

```c
typedef struct search_args {
    const char *name;
    int64_t limit;
    uint8_t include_bots;
} search_args_t;

static void on_search(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    search_args_t args = {0};

    DCC_CTX_BIND_OPTION_FIELDS_OR_REPLY(
        ctx,
        &args,
        DCC_ARG_REQUIRED_STRING(search_args_t, name, "name"),
        DCC_ARG_INT(search_args_t, limit, "limit", 10),
        DCC_ARG_BOOL(search_args_t, include_bots, "include_bots", 0U)
    );

    DCC_CTX_VALIDATE_OR_REPLY(
        ctx,
        &args,
        DCC_VALIDATE_STRING_LENGTH(search_args_t, name, "name", 1U, 32U),
        DCC_VALIDATE_INT_RANGE(search_args_t, limit, "limit", 1, 25)
    );

    DCC_CTX_TRY(ctx, DCC_CTX_SEND_TEXT(ctx, "Search started."));

    /* args.name, args.limit, and args.include_bots are ready. */
}
```

The same bind-or-reply pattern is available for local variables and modal form
fields: `DCC_CTX_BIND_OPTIONS_OR_REPLY()`, `DCC_CTX_BIND_FORM_OR_REPLY()`, and
`DCC_CTX_BIND_FORM_FIELDS_OR_REPLY()`. The shorter aliases
`DCC_BIND_OPTIONS_OR_REPLY()`, `DCC_BIND_OPTION_FIELDS_OR_REPLY()`,
`DCC_BIND_FORM_OR_REPLY()`, and `DCC_BIND_FORM_FIELDS_OR_REPLY()` are equivalent.

Use `DCC_CTX_TRY(ctx, expression)` for handler-local calls that return
`dcc_status_t`. On failure it routes the status through the app's `on_error`
handler, or the default error response when none is installed, then returns from
the current void handler. Use `DCC_CTX_TRY_MSG()` when the failure needs a custom
message.

Modal and component params use the same single-source pattern. Define the struct
once, declare the fields once, then use the list for modal routes, modal flows,
buttons, and selects:

```c
typedef struct profile_form {
    const char *birthday;
    uint8_t public_age;
} profile_form_t;

#define PROFILE_MODAL_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_STRING, birthday, "birthday") \
    PARAM(type, BOOL, public_age, "public_age", 1U)

static void on_profile_modal(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)user_data;

    const profile_form_t *form = args;
    (void)form;

    DCC_REPLY_DONE(ctx, "Profile updated.");
}

dcc_modal_route_t profile_modal =
    DCC_MODAL_PARAMS_DATA_VALIDATED(
        "profile.edit",
        profile_form_t,
        on_profile_modal,
        state,
        PROFILE_MODAL_PARAMS,
        DCC_VALIDATORS(
            DCC_VALIDATE_DATE_MMDD_OR_YYYYMMDD(profile_form_t, birthday, "birthday")
        )
    );

DCC_APP_REGISTER_MODAL(app, profile_modal);
```

In route-table apps, put the same typed modal route directly in the bot
definition:

```c
DCC_BOT_ROUTES(
    "profilebot",
    DCC_ROUTE_MODAL_PARAMS_DATA_VALIDATED(
        "profile.edit",
        profile_form_t,
        on_profile_modal,
        state,
        PROFILE_MODAL_PARAMS,
        DCC_VALIDATORS(
            DCC_VALIDATE_DATE_MMDD_OR_YYYYMMDD(profile_form_t, birthday, "birthday")
        )
    )
);
```

For direct route declarations, prefer the params aliases so the binding list
stays beside the args struct:

```c
dcc_modal_route_t profile_modal =
    DCC_MODAL_PARAMS_DATA_VALIDATED(
        "profile.edit",
        profile_form_t,
        on_profile_modal,
        state,
        PROFILE_MODAL_PARAMS,
        DCC_VALIDATORS(
            DCC_VALIDATE_DATE_MMDD_OR_YYYYMMDD(profile_form_t, birthday, "birthday")
        )
    );
```

Strict date helpers reject impossible values before your handler logic runs:
`DCC_VALIDATE_DATE_YYYYMMDD()` accepts only real eight-digit dates,
`DCC_VALIDATE_DATE_MMDD()` accepts only real four-digit month/day values, and
`DCC_VALIDATE_BIRTHDAY()` accepts either. After validation, use
`DCC_PARSE_DATE()` or `DCC_PARSE_BIRTHDAY()` when the handler needs year,
month, day, or whether the year was provided:

```c
dcc_ctx_parsed_date_t birthday;
if (DCC_PARSE_BIRTHDAY(form->birthday, &birthday) == DCC_OK && birthday.has_year) {
    /* birthday.year, birthday.month, and birthday.day are ready. */
}
```

When the modal UI and submit route should stay in one declaration, use a modal
flow. Registering a flow copies the typed submit route. Use
`DCC_MODAL_V2_FLOW_PARAMS*` for Components v2 modals:

```c
dcc_modal_flow_t profile_flow =
    DCC_MODAL_V2_FLOW_PARAMS_DATA_VALIDATED(
        "profile.edit",
        "Edit profile",
        profile_form_t,
        on_profile_modal,
        state,
        PROFILE_MODAL_PARAMS,
        DCC_VALIDATORS(
            DCC_VALIDATE_DATE_MMDD_OR_YYYYMMDD(profile_form_t, birthday, "birthday")
        ),
        DCC_V2_LABEL(
            "Birthday",
            DCC_MODAL_V2_TEXT_INPUT_PLACEHOLDER("birthday", "Birthday", "YYYYMMDD", 1U)
        ),
        DCC_V2_LABEL(
            "Show age",
            DCC_MODAL_V2_CHECKBOX("public_age", "Show my age", 1U)
        )
    );

DCC_APP_REGISTER_MODAL_FLOW(app, profile_flow);

static void on_profile_button(dcc_ctx_t *ctx, void *user_data) {
    DCC_SHOW_MODAL_FLOW(ctx, *(dcc_modal_flow_t *)user_data);
}
```

Buttons and selects follow the same shape. Prefix button routes are useful when
a `custom_id` carries a compact action or object id:

```c
typedef struct page_click {
    const char *action;
} page_click_t;

#define PAGE_BUTTON_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_CUSTOM_ID_SUFFIX, action, "page:")

#define PAGE_SELECT_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_FIRST_VALUE, action)

static void on_page_button(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)user_data;

    const page_click_t *click = args;
    /* "next" for custom_id "page:next". */
    DCC_REPLY_DONE(ctx, click->action);
}

dcc_button_route_t page_button =
    DCC_BUTTON_PREFIX_PARAMS_DATA_VALIDATED(
        "page:",
        page_click_t,
        on_page_button,
        state,
        PAGE_BUTTON_PARAMS,
        DCC_VALIDATORS(
            DCC_VALIDATE_STRING_REQUIRED(page_click_t, action, "action")
        )
    );

DCC_APP_REGISTER_BUTTON(app, page_button);
```

Route-table component routes use the same params list and can still route by
namespace or prefix:

```c
DCC_BOT_ROUTES(
    "pages",
    DCC_ROUTE_BUTTON_NS_PREFIX_PARAMS_DATA_GUARDED(
        "page",
        "go:",
        page_click_t,
        on_page_button,
        state,
        PAGE_BUTTON_PARAMS,
        DCC_ONLY_GUILD()
    ),
    DCC_ROUTE_SELECT_NS_PARAMS_DATA(
        "page",
        "size",
        page_click_t,
        on_page_select,
        state,
        PAGE_SELECT_PARAMS
    )
);
```

For small handlers that route by namespace instead of a typed binding, read the
action directly from the context:

```c
static void on_birthday_button(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    DCC_REQUIRED_COMPONENT_ACTION(action, ctx, "birthday");

    if (strcmp(action, "register") == 0) {
        DCC_SHOW_MODAL(ctx, birthday_modal);
        return;
    }

    DCC_CTX_BAD_INPUT(ctx, "Unknown birthday action.");
}
```

Select routes can bind the first selected value or the whole selected value
array:

```c
typedef struct role_select {
    const char *first_role;
    const char **roles;
    size_t role_count;
} role_select_t;

#define ROLE_SELECT_PARAMS(PARAM, type) \
    PARAM(type, REQUIRED_FIRST_VALUE, first_role) \
    PARAM(type, REQUIRED_VALUES, roles, role_count)

static void on_role_select(dcc_ctx_t *ctx, void *args, void *user_data) {
    (void)ctx;
    (void)user_data;

    const role_select_t *select = args;
    /* select->first_role and select->roles are ready. */
}

dcc_select_route_t role_select =
    DCC_SELECT_PARAMS_DATA(
        "roles.pick",
        role_select_t,
        on_role_select,
        state,
        ROLE_SELECT_PARAMS
    );

DCC_APP_REGISTER_SELECT(app, role_select);
```

The lower-level `DCC_CTX_BIND_FORM_FIELDS`,
`DCC_CTX_COMPONENT_FIELD_BINDINGS`, and `DCC_TYPED_*` macros remain available
when a handler needs custom binding behavior.

## Context Objects

Handlers do not need to manually walk `interaction->resolved` for common
objects. Use the context macros first:

```c
static void on_profile(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    const dcc_user_t *author = DCC_CTX_AUTHOR(ctx);
    const dcc_member_t *member = DCC_CTX_MEMBER(ctx);
    const dcc_channel_t *channel = DCC_CTX_CHANNEL(ctx);
    const dcc_user_t *target = DCC_CTX_ARG_USER(ctx, "user");
    int64_t limit = DCC_CTX_ARG_INT_OR(ctx, "limit", 10);

    (void)member;
    (void)channel;
    (void)limit;
    if (author == NULL || target == NULL) {
        DCC_EPHEMERAL(ctx, "I could not resolve that user.");
        return;
    }

    DCC_SUCCESS(ctx, "Profile", target->username);
}
```

`DCC_CTX_AUTHOR()`, `DCC_CTX_MEMBER()`, `DCC_CTX_GUILD()`, and
`DCC_CTX_CHANNEL()` return borrowed cache views when available. In compact
handlers, the shorter `DCC_AUTHOR(ctx)`, `DCC_USER(ctx)`, `DCC_MEMBER(ctx)`,
`DCC_GUILD(ctx)`, `DCC_CHANNEL(ctx)`, `DCC_USER_ID(ctx)`, `DCC_CHANNEL_ID(ctx)`,
`DCC_CUSTOM_ID(ctx)`, and `DCC_ACTION(ctx, "prefix:")` aliases are equivalent
to their `DCC_CTX_*` forms.

`DCC_OPT_*`
builds slash command option definitions. In handlers, prefer the context-first
`DCC_CTX_ARG_*` names or the shorter `DCC_PARAM_*` names to read submitted
option values:

```c
const char *name = DCC_CTX_ARG_STRING_OR(ctx, "name", "");
int64_t limit = DCC_CTX_ARG_INT_OR(ctx, "limit", 10);
uint8_t public_result = DCC_CTX_ARG_BOOL_OR(ctx, "public", 0U);
const dcc_user_t *target = DCC_CTX_ARG_USER(ctx, "user");

const char *same_name = DCC_PARAM_STRING_OR(ctx, "name", "");
```

For required options in small handlers, declare and validate the local variable
in one line. Missing or empty values use the friendly bad-input response and
return from the current handler:

```c
DCC_REQUIRED_STRING_OPTION(name, ctx, "name");
DCC_REQUIRED_INT_OPTION(limit, ctx, "limit");
DCC_REQUIRED_USER_OPTION(target, ctx, "user");

DCC_CTX_REPLY_OK(ctx, name);
```

Use the context-prefixed `REQUIRE` form when the handler needs tailored
bad-input copy:

```c
DCC_CTX_REQUIRE_TEXT_OPTION(reason, ctx, "reason", "Enter a reason.");
DCC_CTX_REQUIRE_CHANNEL_OPTION(channel, ctx, "channel", "Choose a channel.");
```

`DCC_CTX_ARG_USER()`, `DCC_CTX_ARG_MEMBER()`, `DCC_CTX_ARG_ROLE()`,
`DCC_CTX_ARG_CHANNEL()`, `DCC_CTX_ARG_MESSAGE()`, and
`DCC_CTX_ARG_ATTACHMENT()`
check the interaction's resolved payload first and then the enabled cache where
that object type can be cached. `DCC_OPTION_*` and `DCC_PARAM_*` remain
available as compatibility aliases.

For UI text, use the display-name helpers instead of manually checking
`nick`, `global_name`, and `username`:

```c
const char *server = DCC_CTX_GUILD_NAME_OR(ctx, "this server");
const char *channel = DCC_CTX_CHANNEL_NAME_OR(ctx, "this channel");
const char *author = DCC_CTX_AUTHOR_DISPLAY_NAME(ctx);
const char *target = DCC_CTX_ARG_USER_DISPLAY_NAME_OR(ctx, "user", "unknown user");
const char *role = DCC_CTX_ARG_ROLE_NAME_OR(ctx, "role", "unknown role");
const char *avatar = DCC_CTX_AUTHOR_AVATAR_URL(ctx);
```

`DCC_CTX_AUTHOR_DISPLAY_NAME()` prefers member nick, then global name, then
username. The `DCC_CTX_*_AVATAR_URL()` helpers prefer guild member avatars when
Discord supplied one, then fall back to the user avatar. The same logic is
available for raw objects with `DCC_USER_DISPLAY_NAME()`,
`DCC_MEMBER_DISPLAY_NAME()`, `DCC_GUILD_NAME()`, `DCC_CHANNEL_NAME()`,
`DCC_ROLE_NAME()`, `DCC_USER_AVATAR_URL()`, and `DCC_MEMBER_AVATAR_URL()`.

Modal handlers can use field aliases:

```c
const char *birthday = DCC_CTX_FIELD_TEXT_OR(ctx, "birthday", "");
uint8_t visible = DCC_CTX_FIELD_CHECKED_OR(ctx, "visible", 1U);
const char *first_color = DCC_CTX_FIELD_FIRST_VALUE_OR(ctx, "colors", "blue");
```

For required modal fields, declare and validate the handler-local values in one
line. Checkboxes validate the field type, so a submitted `false` value is still
accepted as valid input:

```c
DCC_REQUIRED_TEXT_FIELD(birthday, ctx, "birthday");
DCC_REQUIRED_CHECKBOX_FIELD(show_age, ctx, "show_age");
DCC_REQUIRED_FIRST_VALUE_FIELD(color, ctx, "color");

DCC_CTX_REPLY_OK(ctx, birthday);
```

Use the explicit `REQUIRE` form when the field needs a tailored message:

```c
DCC_CTX_REQUIRE_TEXT_FIELD(reason, ctx, "reason", "Enter a reason.");
DCC_CTX_REQUIRE_VALUES_FIELD(tags, tag_count, ctx, "tags", "Choose at least one tag.");
```

Select handlers can use typed `DCC_COMPONENT_ARGS(...)` for larger handlers, or
read the submitted values directly for small routes:

```c
const char *role_id = DCC_CTX_SELECT_FIRST(ctx);
size_t role_count = DCC_CTX_SELECT_COUNT(ctx);
const char **roles = DCC_CTX_SELECT_VALUES(ctx, &role_count);
```

For required select values, declare the local value and bad-input guard
together:

```c
DCC_REQUIRED_SELECT_FIRST(role_id, ctx);
DCC_REQUIRED_SELECT_VALUES(role_ids, role_count, ctx);

DCC_CTX_REPLY_OK(ctx, role_id);
```

For small handler-local checks, use the `DCC_REQUIRED_*` helpers when the
default bad-input copy is enough. They send an ephemeral message and return
from the current void handler:

```c
DCC_REQUIRED_GUILD(ctx);
DCC_REQUIRED_ARG_TEXT(ctx, "name");
DCC_REQUIRED_FORM_TEXT(ctx, "birthday");
DCC_REQUIRED_SELECT(ctx);
DCC_REQUIRED_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_GUILD);
DCC_REQUIRED_ANY_ROLE_IDS(ctx, 123456789012345678ULL, 223456789012345678ULL);
DCC_REQUIRED_TEXT(ctx, args->reason);
DCC_REQUIRED_CUSTOM_ID_PREFIX(ctx, "settings:");
```

Use `DCC_CTX_REQUIRE_*` or `DCC_REQUIRE_*` when the handler needs custom copy:

```c
DCC_CTX_REQUIRE_GUILD(ctx, "This command can only be used in a server.");
DCC_REQUIRE_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_GUILD, "Missing Manage Server.");
```

When the failure should use the same friendly status embeds as
`DCC_CTX_BAD_INPUT()` or `DCC_CTX_DENY()`, use the status require aliases:

```c
DCC_CTX_REQUIRE_BAD_INPUT(ctx, limit <= 100, "Limit must be 100 or lower.");
DCC_CTX_REQUIRE_BAD_INPUT_F(ctx, limit <= 100, "Limit must be at most %d.", 100);
DCC_CTX_REQUIRE_ARG_TEXT_BAD_INPUT(ctx, "name", "Enter a name.");
DCC_CTX_REQUIRE_FORM_TEXT_BAD_INPUT(ctx, "birthday", "Enter a birthday.");
DCC_CTX_REQUIRE_SELECT_BAD_INPUT(ctx, "Choose at least one option.");

DCC_CTX_REQUIRE_DENY(ctx, is_staff, "Only staff can use this.");
DCC_CTX_REQUIRE_NOT_FOUND(ctx, member != NULL, "Member not found.");
DCC_CTX_REQUIRE_BUSY(ctx, !job_running, "A refresh is already running.");
DCC_CTX_REQUIRE_FAIL(ctx, saved, "Could not save the settings.");
DCC_CTX_REQUIRE_INTERNAL(ctx, database != NULL);
```

## App Definitions

Use `DCC_BOT(...)` when an entrypoint should read like one bot declaration
instead of a sequence of route registration calls. Use `DCC_APP(...)` when you
want to choose every app option explicitly.

```c
dcc_app_definition_t bot =
    DCC_BOT(
        "mybot",
        DCC_APP_EXTENSIONS(
            DCC_FEATURE(
                "profile",
                DCC_FEATURE_COMMANDS(DCC_COMMAND_DATA("profile", "Show your profile", on_profile, state))
            )
        ),
        DCC_APP_MODAL_ROUTES(profile_modal),
        DCC_APP_BUTTON_ROUTES(page_button),
        DCC_APP_MESSAGE_COMMANDS(DCC_MESSAGE_COMMAND("!", "ping", on_message_ping))
    );

dcc_app_t *app = NULL;
dcc_app_create_defined(&options, &bot, &app);
```

For the common `.env` case, run and destroy the app directly:

```c
dcc_status_t status = DCC_RUN_APP(bot);
```

Definitions can contain modules, declarative extensions, typed slash commands,
typed modal routes, typed button routes, typed select routes, message prefix
commands, READY-time command sync, app-wide auto-defer, and default
validation/error replies. Use
`dcc_app_apply(app, &bot)` when the app already exists.

The arrays produced by `DCC_APP_USE`, `DCC_APP_FEATURES`, `DCC_APP_MODULES`,
`DCC_APP_EXTENSIONS`, and the `DCC_APP_*_ROUTES` macros are short-lived
compound literals. They are ideal for
entrypoints and module setup functions where the definition is applied
immediately. If a definition must live beyond the current block, declare normal C
arrays and assign the pointer/count fields explicitly.

Inside void handlers, prefer `DCC_CTX_BIND_OPTIONS_OR_REPLY()` and
`DCC_CTX_BIND_FORM_OR_REPLY()` when binding into local variables is clearer for
tiny handlers. They send the default friendly validation response and return
from the handler on bad input. Use `DCC_CTX_BIND_OPTIONS()` and
`DCC_CTX_BIND_FORM()` when a handler needs custom status handling. The
validation macros are optional sugar over `dcc_ctx_validate_fields()`: use the
explicit `dcc_ctx_field_validator_t` array when validators are generated
dynamically.

Register component handlers as a small View when a message has several related
buttons, selects, or modals:

```c
DCC_APP_USE_VIEW_DATA(
    app,
    state,
    DCC_VIEW_BUTTON("settings.save", on_save),
    DCC_VIEW_BUTTON_PREFIX("page:", on_page),
    DCC_VIEW_MODAL("settings.modal", on_settings_modal)
);
```

For button-only panels, define the actions once and reuse them for both the
Components v2 row and the persistent routes:

```c
#define SETTINGS_ACTIONS \
    DCC_VIEW_ACTION_PRIMARY("Refresh", "settings.refresh", on_settings_button), \
    DCC_VIEW_ACTION_SECONDARY("Edit", "settings.edit", on_settings_button)

DCC_APP_USE_VIEW_ACTIONS(app, state, SETTINGS_ACTIONS);
/* Or in a declarative feature/app definition:
   DCC_FEATURE_VIEWS(DCC_VIEW_ACTION_ROUTES_DATA(state, SETTINGS_ACTIONS)) */

DCC_RESPOND_UI(
    ctx,
    DCC_UI_CARD_ACCENT(
        DCC_COLOR_BLURPLE,
        DCC_UI_TEXT("## Settings"),
        DCC_VIEW_ACTION_ROW(SETTINGS_ACTIONS)
    )
);
```

For persistent UI, prefer the namespace-aware View aliases so the button row,
route, and modal IDs follow the same convention:

```c
#define BIRTHDAY_ACTIONS \
    DCC_VIEW_ACTION_PRIMARY_NS("Register", "birthday", "register", on_register), \
    DCC_VIEW_ACTION_SECONDARY_NS("Schedule", "birthday", "schedule", on_schedule)

dcc_app_view_t birthday_view =
    DCC_PERSISTENT_VIEW_DATA(
        state,
        DCC_VIEW_BUTTON_NS("birthday", "register", on_register),
        DCC_VIEW_SELECT_NS_PREFIX("birthday", "month:", on_month_select),
        DCC_VIEW_MODAL_NS("birthday", "submit", on_submit)
    );

DCC_APP_USE_VIEW_ACTIONS(app, state, BIRTHDAY_ACTIONS);
DCC_APP_USE_VIEW_DATA(
    app,
    state,
    DCC_VIEW_BUTTON_NS_PREFIX("birthday", "page:", on_page)
);
```

Use the optional third `DCC_VIEW_*` argument only when one item needs a
different state pointer than the View default.
Use `dcc_new_app add view . profile dashboard` to generate a `/dashboard`
Components v2 opener plus a reusable `*_ACTIONS` list that drives both the
button row and the persistent View routes inside a Cog.

For declaration-first apps, put persistent views directly on `DCC_APP(...)`:

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

For a feature namespace, use a route group so the prefix is written once:

```c
dcc_app_route_group_t settings =
    DCC_GROUP_DATA(app, "settings:", state);

DCC_GROUP_BUTTON(&settings, "save", on_save);
DCC_GROUP_SELECT(&settings, "mode", on_mode_select);

DCC_GROUP_USE_VIEW(
    &settings,
    DCC_VIEW_BUTTON("open", on_open),
    DCC_VIEW_MODAL("edit", on_edit_modal)
);
DCC_GROUP_OWNER(&settings, owner_user_id);
```

Group routes can also take the same route policy descriptor used by slash and
component routes:

```c
dcc_app_route_policy_t settings_policy =
    DCC_ROUTE_POLICY(
        DCC_ROUTE_GUILD_ONLY(),
        DCC_ROUTE_REQUIRED_PERMISSIONS(DCC_PERMISSION_MANAGE_GUILD),
        DCC_ROUTE_COOLDOWN_USER(3U, DCC_SECONDS(10U))
    );

DCC_GROUP_POLICY(&settings, settings_policy);
```

Use explicit arrays instead of inline compound literals when a command builder
must outlive the current block.

```c
static const dcc_autocomplete_choice_t rank_sort_choices[] = {
    {
        .name = "Members",
        .value_string = "members",
        .value_type = DCC_AUTOCOMPLETE_CHOICE_STRING,
        .has_name = 1U,
        .has_value = 1U
    },
    {
        .name = "Score",
        .value_string = "score",
        .value_type = DCC_AUTOCOMPLETE_CHOICE_STRING,
        .has_name = 1U,
        .has_value = 1U
    }
};

static const dcc_application_command_option_builder_t rank_options[] = {
    {
        .type = DCC_APPLICATION_COMMAND_OPTION_STRING,
        .name = "sort",
        .description = "Sort mode",
        .choices = rank_sort_choices,
        .choices_count = DCC_ARRAY_LEN(rank_sort_choices),
        .has_required = 1U
    }
};

dcc_application_command_builder_t rank =
    DCC_SLASH_CMD("rank", "Show rank");
dcc_application_command_builder_set_options(
    &rank,
    rank_options,
    sizeof(rank_options) / sizeof(rank_options[0])
);
```

Registry options default to global command scope. Pick a guild explicitly for
development rollouts:

```c
dcc_command_registry_options_t global_plan =
    DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DRY_RUN();

dcc_command_registry_options_t guild_apply =
    DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DELETE_STALE(guild_id);
```

## Messages And Embeds

```c
dcc_message_builder_t text =
    DCC_MESSAGE_TEXT("pong");

dcc_message_builder_t private_text =
    DCC_MESSAGE_EPHEMERAL("Only you can see this.");

dcc_message_builder_t status =
    DCC_MESSAGE_TEXT_EMBED(
        "ready",
        DCC_EMBED_COLOR_FIELDS(
            "Runtime",
            "Gateway online",
            0x57F287,
            DCC_EMBED_FIELD_INLINE("Shard", "0"),
            DCC_EMBED_FIELD("Mode", "production")
        )
    );
```

Message flag helpers keep common Discord response options readable. `PRIVATE`
sets the ephemeral flag for interaction responses and followups, `SILENT`
suppresses push notifications, and `NO_EMBEDS` suppresses generated link
previews.

```c
dcc_message_builder_t private_reply =
    DCC_MESSAGE_PRIVATE(DCC_MESSAGE_TEXT("Only you can see this."));

dcc_message_builder_t quiet_update =
    DCC_MESSAGE_SILENT(DCC_MESSAGE_TEXT("Updated without a push notification."));

dcc_message_builder_t plain_link =
    DCC_MESSAGE_NO_EMBEDS(DCC_MESSAGE_TEXT("https://example.com"));

dcc_message_builder_t quiet_panel =
    DCC_MESSAGE_SILENT_V2(DCC_UI_TEXT("Quiet Components v2 panel"));
```

Handlers can use the same vocabulary directly from an interaction context:

```c
DCC_CTX_PRIVATE(ctx, "Only you can see this.");
DCC_CTX_SILENT(ctx, "Updated without a push notification.");
DCC_CTX_NO_EMBEDS(ctx, "https://example.com");
DCC_CTX_DEFER_PRIVATE(ctx);
DCC_CTX_PRIVATE_EMBED(ctx, "Private status", "Only you can see this.", DCC_COLOR_BLURPLE);
```

Ephemeral messages are a Discord interaction feature; normal channel sends will
still be public even if the flag is present.

Poll messages can be built without manually declaring answer arrays. Use
`DCC_POLL_YES_NO()` for the common two-choice case, or `DCC_POLL_MULTI()` for a
multi-select poll:

```c
dcc_message_builder_t deploy_poll =
    DCC_MESSAGE_TEXT_POLL("vote", DCC_POLL_YES_NO("Deploy now?"));

dcc_message_builder_t env_poll =
    DCC_MESSAGE_POLL(
        DCC_POLL_MULTI(
            "Pick target environments",
            2U,
            DCC_POLL_ANSWER_EMOJI("Production", DCC_COMPONENT_EMOJI_NAME("rocket")),
            DCC_POLL_ANSWER("Staging")
        )
    );
```

Use `DCC_POLL_ARRAY()` when answer storage already lives outside the current C
block, and `DCC_MESSAGE_TEXT_POLL_JSON()` when a low-level builder already
produced raw poll JSON.

Allowed mention sugar keeps channel sends safe without hand-writing Discord JSON:

```c
dcc_message_builder_t safe =
    DCC_MESSAGE_NO_MENTIONS("@everyone this will not ping");

dcc_message_builder_t targeted =
    DCC_MESSAGE_MENTION_USERS("hi <@1234567890>", 1234567890ULL);

dcc_message_builder_t private_safe =
    DCC_MESSAGE_EPHEMERAL_MENTIONS(
        "private reply",
        DCC_ALLOWED_MENTIONS_REPLY(0U)
    );
```

Use `DCC_ALLOWED_MENTIONS_USERS(...)`, `DCC_ALLOWED_MENTIONS_ROLES(...)`,
`DCC_ALLOWED_MENTIONS_ALL()`, or the `*_ARRAY` variants when IDs are already in
arrays. Typed allowed mentions cannot be mixed with raw
`allowed_mentions_json`; that combination is rejected before serialization.

Mention and timestamp formatting helpers avoid repeated `snprintf()` calls when
building Discord text:

```c
char line[192];
snprintf(
    line,
    sizeof(line),
    "%s can review this in %s %s. Use %s before %s.",
    DCC_MENTION_USER(user_id),
    DCC_MENTION_CHANNEL(channel_id),
    DCC_EMOJI("party_blob", emoji_id),
    DCC_MENTION_SUBCOMMAND("rank", "top", command_id),
    DCC_TIMESTAMP_RELATIVE(deadline_unix)
);

dcc_message_builder_t message =
    DCC_MESSAGE_MENTIONS(line, DCC_ALLOWED_MENTIONS_USERS(user_id));
```

Inside handlers, use the context-first aliases when the current interaction is
the target:

```c
DCC_RESPONDF(
    ctx,
    "%s opened this in %s: %s",
    DCC_CTX_USER_MENTION(ctx),
    DCC_CTX_CHANNEL_MENTION(ctx),
    DCC_CTX_MESSAGE_LINK(ctx)
);

DCC_RESPONDF(
    ctx,
    "Target: %s, role: %s",
    DCC_CTX_TARGET_USER_MENTION(ctx),
    DCC_OPTION_ROLE_MENTION(ctx, "role")
);
```

When the response itself is just formatted text, use the `*F` helpers instead of
declaring a temporary buffer. They allocate the exact temporary string, send
through the normal DCC message path, and free it before returning.
`DCC_RESPOND*` is the friendlier spelling for new handler code; the older
`DCC_REPLY*` names remain aliases for existing projects.

```c
DCC_CTX_RESPOND_TEXT_F(ctx, "Synced %zu commands.", command_count);
DCC_CTX_RESPOND_PRIVATE_F(ctx, "Only you can see %s.", detail);
DCC_CTX_REPLY_TEXT_F(ctx, "Rank #%lld: %s", (long long)rank, name);
DCC_CTX_PRIVATE_F(ctx, "Only you can see %s.", detail);
DCC_CTX_SILENT_F(ctx, "Refreshed %zu records.", record_count);
DCC_CTX_FOLLOWUP_PRIVATE_F(ctx, "Finished in %ums.", elapsed_ms);
DCC_CTX_UPDATE_TEXT_F(ctx, "Page %zu of %zu", page, page_count);
DCC_CTX_EDIT_TEXT_F(ctx, "Finished in %ums.", elapsed_ms);
DCC_CTX_DONE_F(ctx, "Saved %zu records.", record_count);
DCC_CTX_BAD_INPUT_F(ctx, "Limit must be between %d and %d.", 1, 100);
DCC_CTX_SUCCESS_F(ctx, "Profile updated", "Saved changes for %s.", DCC_CTX_USER_MENTION(ctx));
DCC_APP_SEND_TEXT_F(app, log_channel, "Synced %zu commands.", command_count);
DCC_CHANNEL_SEND_TEXT_F(app, log_channel, "Synced %zu commands.", command_count);
DCC_APP_EDIT_TEXT_F(app, status_channel, status_message, "Updated %zu records.", record_count);
DCC_MESSAGE_EDIT_TEXT_F(app, status_channel, status_message, "Updated %zu records.", record_count);
```

Use `DCC_FORMAT_ALLOC()` when you need the formatted text long enough to combine
it with another builder:

```c
char *body = NULL;
if (DCC_FORMAT_ALLOC(&body, "Next refresh: %s", DCC_TIMESTAMP_RELATIVE(next_unix)) == DCC_OK) {
    DCC_REPLY_TEXT(ctx, body);
    DCC_FORMAT_FREE(body);
}
```

The explicit functions are available when the output buffer needs checked error
handling:

```c
char mention[DCC_FORMAT_MENTION_MAX];
if (dcc_sugar_format_user_mention(mention, sizeof(mention), user_id) != DCC_OK) {
    DCC_CTX_BAD_INPUT(ctx, "Invalid user id.");
    return;
}
```

Slash command mentions are supported too, including subcommands and subcommand
groups:

```c
const char *help = DCC_MENTION_COMMAND("help", help_command_id);
const char *top = DCC_MENTION_SUBCOMMAND("rank", "top", rank_command_id);
const char *birthday = DCC_MENTION_SUBCOMMAND_GROUP(
    "config",
    "birthday",
    "set",
    config_command_id
);
```

Markdown helpers cover common Discord text styling without hand-writing wrapper
characters in every handler. Short macros are convenient for fixed phrases:

```c
char status[256];
snprintf(
    status,
    sizeof(status),
    "%s %s %s",
    DCC_MD_BOLD("Deploy complete"),
    DCC_MD_CODE("v1.3.2"),
    DCC_MD_HIDE_LINK_EMBED("https://example.com/release")
);
```

Use checked functions for user input or longer strings:

```c
char block[DCC_MARKDOWN_SHORT_MAX];
if (dcc_sugar_markdown_code_block(block, sizeof(block), "json", json_text) != DCC_OK) {
    DCC_CTX_BAD_INPUT(ctx, "That text cannot be displayed as a safe code block.");
    return;
}
DCC_REPLY_TEXT(ctx, block);
```

Custom emoji helpers validate the Discord emoji mention shape too:

```c
dcc_sugar_custom_emoji_t emoji;
if (dcc_sugar_parse_custom_emoji("<a:spin:1234567890>", &emoji) == DCC_OK &&
    emoji.animated) {
    DCC_REPLY_TEXT(ctx, DCC_ANIMATED_EMOJI("spin", emoji.id));
}
```

When you need an image URL rather than message markup, use the CDN helpers.
They validate ids, hash path segments, extensions, and Discord CDN image sizes:

```c
dcc_embed_builder_t profile =
    DCC_EMBED_COLOR_THUMBNAIL(
        "Profile",
        "Current member avatar",
        0x5865F2,
        DCC_CDN_MEMBER_AVATAR(guild_id, user_id, member_avatar_hash)
    );

const char *emoji_image = DCC_CDN_EMOJI(emoji_id);
```

Use the checked functions when the image size or extension comes from user
input:

```c
char avatar_url[DCC_CDN_URL_MAX];
if (dcc_sugar_cdn_user_avatar_url(
        avatar_url,
        sizeof(avatar_url),
        user_id,
        avatar_hash,
        "webp",
        2048U
    ) != DCC_OK) {
    DCC_CTX_BAD_INPUT(ctx, "Invalid avatar CDN options.");
    return;
}
```

The reverse direction is covered too. Use `DCC_PARSE_ID()` when command or modal
input may be either a raw snowflake or a pasted Discord mention. Use the typed
parsers when the input must be a specific mention shape:

```c
dcc_snowflake_t target = 0;
if (DCC_PARSE_ID(input, &target) != DCC_OK) {
    DCC_CTX_BAD_INPUT(ctx, "Paste a Discord id or mention.");
    return;
}

dcc_snowflake_t channel = 0;
if (DCC_PARSE_CHANNEL_MENTION("<#1234567890>", &channel) == DCC_OK) {
    DCC_APP_SEND_SAFE(app, channel, "Configured.");
}
```

Snowflakes carry their creation time. DCC exposes that directly and can turn it
into Discord timestamp markup:

```c
char line[160];
snprintf(
    line,
    sizeof(line),
    "Message was created %s",
    DCC_SNOWFLAKE_RELATIVE(message_id)
);

uint64_t created_ms = 0;
if (DCC_SNOWFLAKE_CREATED_AT_MS(message_id, &created_ms) != DCC_OK) {
    DCC_CTX_BAD_INPUT(ctx, "Invalid Discord id.");
    return;
}
```

Message links have the same checked low-level API and short sugar macros. This
keeps moderation tools and replay/debug commands from hand-splitting copied
Discord URLs:

```c
const char *jump_url = DCC_MESSAGE_LINK(guild_id, channel_id, message_id);

dcc_message_link_t parsed = {0};
if (DCC_PARSE_MESSAGE_LINK(input, &parsed) != DCC_OK) {
    DCC_CTX_BAD_INPUT(ctx, "Paste a Discord message link.");
    return;
}
```

`DCC_PARSE_MESSAGE_LINK()` accepts normal, canary, ptb, path-only, and DM
`@me` message links. Use `DCC_DM_MESSAGE_LINK(channel_id, message_id)` when the
target is a direct-message channel.

For richer embeds, use the rich embed helpers instead of setting `has_footer`,
`has_image`, `has_thumbnail`, or `has_author` manually:

```c
dcc_message_builder_t status =
    DCC_MESSAGE_TEXT_EMBED(
        "ready",
        DCC_EMBED_RICH_FIELDS(
            "Runtime",
            "Gateway online",
            0x57F287,
            "DCC",
            NULL,
            "attachment://status.png",
            "https://example.com/thumb.png",
            "runtime",
            "https://example.com/runtime",
            NULL,
            DCC_EMBED_FIELD_INLINE("Shard", "0"),
            DCC_EMBED_FIELD("Mode", "production")
        )
    );
```

For the common embed plus button row shape, use action helpers instead of
declaring embed and component arrays by hand:

```c
dcc_message_builder_t status_card =
    DCC_MESSAGE_TEXT_EMBED_ACTIONS(
        "ready",
        DCC_EMBED_COLOR("Runtime", "Gateway online", 0x57F287),
        DCC_ACTION_ROW(
            DCC_BUTTON_PRIMARY_BUILDER("Refresh", "status.refresh"),
            DCC_BUTTON_DANGER_BUILDER("Close", "status.close")
        )
    );

dcc_message_builder_t private_card =
    DCC_MESSAGE_EPHEMERAL_EMBED_ACTIONS(
        DCC_EMBED("Settings", "Only you can see this."),
        DCC_ACTION_ROW(DCC_BUTTON_SECONDARY_BUILDER("Dismiss", "settings.dismiss"))
    );
```

Use `DCC_MESSAGE_EMBEDS_ARRAY` and `DCC_EMBED_WITH_FIELDS_ARRAY` when the embed
or field arrays already exist.

## Handler Replies

In app handlers, reply sugar removes the callback and `user_data` placeholders
from the common path. The macros call the same flow-aware `dcc_ctx_*()` APIs, so
they still edit a deferred original response or send a followup when needed.
For discord.js/discord.py-style handler code, prefer `DCC_RESPOND*` or the
context-first `DCC_CTX_*` names. When you want the behavior to be obvious at
the call site, use `DCC_RESPOND_OR_EDIT*`: it sends the initial response when
ready, edits the original after a defer, and sends a followup after an earlier
reply.

```c
static void on_ping(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;
    DCC_CTX_RESPOND_OR_EDIT_DONE(ctx, "pong");
}

static void on_status(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    DCC_RESPOND_OR_EDIT_UI(
        ctx,
        DCC_UI_CARD_ACCENT(
            0x5865F2,
            DCC_UI_TEXT("## Runtime online"),
            DCC_UI_ROW(
                DCC_UI_PRIMARY("Refresh", "status.refresh")
            )
        )
    );
}
```

Common handler actions are all available from the same prefix:

```c
DCC_CTX_RESPOND_OR_EDIT_TEXT(ctx, "Done");
DCC_CTX_RESPOND_OR_EDIT_PRIVATE(ctx, "Only you can see this.");
DCC_CTX_RESPOND_OR_EDIT_UI(ctx, DCC_UI_TEXT("Component-first response"));
DCC_CTX_RESPOND_OR_FOLLOWUP_TEXT(ctx, "Explicit followup-safe response");
DCC_CTX_REPLY_TEXT(ctx, "Done");
DCC_CTX_PRIVATE(ctx, "Only you can see this.");
DCC_CTX_DEFER_PRIVATE(ctx);
DCC_CTX_FOLLOWUP_TEXT(ctx, "Extra detail");
DCC_CTX_FOLLOWUP_UI(ctx, DCC_UI_TEXT("Extra component detail"));
DCC_CTX_UPDATE(ctx, updated_message);
DCC_CTX_UPDATE_UI(ctx, DCC_UI_TEXT("Updated component UI"));
DCC_CTX_EDIT(ctx, edited_message);
DCC_CTX_DELETE_ORIGINAL(ctx);
DCC_CTX_SEND(ctx, channel_message);
DCC_CTX_SHOW_MODAL(ctx, modal);
DCC_CTX_SHOW_MODAL_V2(
    ctx,
    "profile.modal",
    "Profile",
    DCC_UI_LABEL("Name", DCC_UI_INPUT("profile.name", "Name", 1U))
);
DCC_CTX_SHOW_MODAL_FLOW(ctx, profile_flow);
```

Handler-only modules can include `<dcc/sugar/context.h>` for these context
aliases. It pulls in the focused reply, message, and modal sugar needed by
`DCC_CTX_REPLY_UI()`, `DCC_CTX_UPDATE_UI()`, `DCC_CTX_SEND_UI()`, and
`DCC_CTX_SHOW_MODAL_V2()` without requiring the full `<dcc/sugar.h>` umbrella.

For legacy embed plus button messages, reply directly with the action helpers:

```c
DCC_REPLY_TEXT_EMBED_ACTIONS(
    ctx,
    "ready",
    DCC_EMBED_COLOR("Runtime", "Gateway online", 0x57F287),
    DCC_ACTION_ROW(DCC_BUTTON_PRIMARY_BUILDER("Refresh", "status.refresh"))
);
```

Short status helpers are also exposed both with and without the `CTX` prefix:

```c
DCC_CTX_OK(ctx, "Saved.");
DCC_CTX_DENY(ctx, "Only moderators can use this.");
DCC_CTX_BAD_INPUT(ctx, "Use YYYYMMDD.");
DCC_CTX_NOT_FOUND(ctx, "No matching member was found.");
DCC_CTX_BUSY(ctx, "This action is already running.");
DCC_CTX_INTERNAL_ERROR(ctx);
DCC_CTX_SUCCESS(ctx, "Saved", "Your settings were updated.");
DCC_CTX_ERROR(ctx, "Action failed", "Try again later.");
DCC_CTX_WARN(ctx, "This action is already queued.");
DCC_CTX_INFO(ctx, "Gateway is online.");

DCC_PUBLIC(ctx, "Done");
DCC_PRIVATE(ctx, "Only you can see this.");
DCC_ACK(ctx, "Saved.");
DCC_DENY(ctx, "Only moderators can use this.");
DCC_CTX_BAD_INPUT(ctx, "Use YYYYMMDD.");
DCC_NOT_FOUND(ctx, "No matching member was found.");
DCC_BUSY(ctx, "This action is already running.");
DCC_INTERNAL_ERROR(ctx);
DCC_SUCCESS(ctx, "Saved", "Your settings were updated.");
DCC_ERROR(ctx, "Action failed", "Try again later.");
DCC_WARN(ctx, "This action is already queued.");
DCC_INFO(ctx, "Gateway is online.");
DCC_DEFER_PRIVATE(ctx);
DCC_RESPOND_TEXT(ctx, "Done");
DCC_RESPOND_PRIVATE(ctx, "Only you can see this.");
DCC_RESPOND_OR_EDIT_TEXT(ctx, "Done");
DCC_RESPOND_OR_EDIT_PRIVATE(ctx, "Only you can see this.");
DCC_RESPOND_OR_EDIT_V2(ctx, DCC_V2_TEXT("Component-first response"));
DCC_RESPOND_OR_EDIT_UI(ctx, DCC_UI_TEXT("Component-first response"));
DCC_RESPOND_OR_FOLLOWUP_TEXT(ctx, "Same flow-aware routing, explicit name");
DCC_RESPOND_PRIVATE_EMBED(ctx, "Private status", "Only you can see this.", DCC_COLOR_BLURPLE);
DCC_RESPOND_UI(ctx, DCC_UI_TEXT("Component-first response"));
DCC_RESPOND_PRIVATE_UI(ctx, DCC_UI_TEXT("Private component response"));
DCC_RESPOND_DEFER_PRIVATE(ctx);
DCC_REPLY_PUBLIC_EMBED(ctx, "Status", "Gateway online", DCC_COLOR_SUCCESS);
DCC_REPLY_PRIVATE_EMBED(ctx, "Private status", "Only you can see this.", DCC_COLOR_BLURPLE);
DCC_REPLY_SAFE(ctx, "@everyone this will not ping");
DCC_REPLY_STATUS(ctx, "Queued", "The job has started.", DCC_COLOR_INFO);
DCC_REPLY_OK(ctx, "Saved.");
DCC_REPLY_WARNING(ctx, "Check the form and try again.");
DCC_REPLY_INFO(ctx, "The job is still running.");
DCC_FOLLOWUP_PUBLIC(ctx, "Extra detail");
DCC_FOLLOWUP_V2(ctx, DCC_V2_TEXT("Extra component detail"));
DCC_UPDATE(ctx, updated_message);
DCC_UPDATE_V2(ctx, DCC_V2_TEXT("Updated component UI"));
DCC_EDIT(ctx, edited_message);
DCC_DELETE_ORIGINAL(ctx);
DCC_SHOW_MODAL(ctx, modal);
DCC_SHOW_MODAL_V2(
    ctx,
    "profile.modal",
    "Profile",
    DCC_V2_LABEL("Name", DCC_MODAL_V2_TEXT_INPUT("profile.name", "Name", 1U))
);
DCC_SHOW_MODAL_FLOW(ctx, profile_flow);
```

When the response is a reusable message object rather than an immediate context
reply, use the status message builders. They default to safe allowed mentions,
so copied user text cannot trigger accidental pings:

```c
dcc_message_builder_t saved = DCC_MESSAGE_SUCCESS("Saved.");
dcc_message_builder_t failed = DCC_MESSAGE_ERROR("Try again later.");
dcc_message_builder_t warning = DCC_MESSAGE_WARNING("Check the input.");
dcc_message_builder_t info = DCC_MESSAGE_INFO("Gateway online.");
dcc_message_builder_t public_status =
    DCC_MESSAGE_STATUS("Status", "Gateway online", DCC_COLOR_INFO);

DCC_REPLY(ctx, saved);
DCC_APP_SEND_SAFE(app, log_channel, "@everyone deploy complete");
```

The `DCC_CTX_PUBLIC()`, `DCC_CTX_EPHEMERAL()`, `DCC_CTX_ACK()`,
`DCC_CTX_DENY()`, `DCC_CTX_BAD_INPUT()`, `DCC_CTX_NOT_FOUND()`,
`DCC_CTX_BUSY()`, and `DCC_CTX_INTERNAL_ERROR()` aliases are the shortest path
for typical handlers. They send flow-aware status responses, so a deferred
handler edits the original response and an already-replied handler sends a
followup. The explicit names are still available when a codebase prefers the
full operation name or a custom title:

```c
DCC_REPLY_TEXT(ctx, "Done");
DCC_REPLY_EPHEMERAL(ctx, "Only you can see this.");
DCC_REPLY_ACK(ctx, "Saved.");
DCC_REPLY_DENY(ctx, "Only moderators can use this.");
DCC_REPLY_BAD_INPUT(ctx, "Use YYYYMMDD.");
DCC_REPLY_NOT_FOUND(ctx, "No matching member was found.");
DCC_REPLY_BUSY(ctx, "This action is already running.");
DCC_REPLY_ERROR(ctx, "Action failed", "Try again later.");
DCC_REPLY_SUCCESS(ctx, "Saved", "Your settings were updated.");
DCC_FOLLOWUP_TEXT(ctx, "Extra detail");
DCC_UPDATE_MESSAGE(ctx, updated_message);
DCC_SHOW_MODAL(ctx, modal);
DCC_SHOW_MODAL_FLOW(ctx, profile_flow);
```

For Components v2, the `*_V2()` reply helpers are shorthand for wrapping the
same components in `DCC_MESSAGE_COMPONENTS_V2(...)`. The `DCC_UI_*` aliases are
the shortest form for app UI: `DCC_UI_TEXT`, `DCC_UI_ROW`,
`DCC_UI_PRIMARY`, `DCC_UI_CARD`, `DCC_UI_CARD_ACCENT`, `DCC_UI_STRING_SELECT`,
`DCC_UI_LABEL`, and `DCC_UI_INPUT` map directly to Components v2 builders.

`DCC_EMBED(...)` is the embed builder macro, so embed replies intentionally use
`DCC_REPLY_PUBLIC_EMBED()` and `DCC_REPLY_EPHEMERAL_EMBED()` to avoid overload
confusion in C.

Use color helpers instead of scattering raw decimal or hex values through bot
code. Embeds, Components v2 accent colors, and role colors all use Discord's
`0xRRGGBB` integer format:

```c
uint32_t theme = DCC_COLOR_RGB(88, 101, 242);

dcc_message_builder_t panel =
    DCC_MESSAGE_EMBED(DCC_EMBED_COLOR("Theme", "Using blurple", theme));

uint32_t configured = 0;
if (DCC_COLOR_FROM_HEX("#57F287", &configured) != DCC_OK) {
    DCC_CTX_BAD_INPUT(ctx, "Use a color like #57F287.");
    return;
}

char label[8];
DCC_COLOR_TO_HEX(label, sizeof(label), configured); /* "#57F287" */
```

Named colors are available for common Discord UI states:
`DCC_COLOR_BLURPLE`, `DCC_COLOR_SUCCESS`, `DCC_COLOR_WARNING`,
`DCC_COLOR_ERROR`, `DCC_COLOR_INFO`, and `DCC_COLOR_NEUTRAL`.

The builder arguments are C compound literals when you pass inline sugar such as
`DCC_MESSAGE_COMPONENTS_V2(...)` or `DCC_MODAL(...)`. Keep those calls in the
same block as the handler call, or use explicit `dcc_ctx_*()` functions when a
message/modal object must be stored and reused later.

## Context Shortcuts

Use `DCC_CTX_*` shortcuts when a handler needs the current author, guild,
channel, command name, or component action. They are thin aliases over the
public `dcc_ctx_*()` functions, with a few NULL-safe helpers for common checks.

```c
static void on_admin(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    if (!DCC_CTX_IS_GUILD(ctx)) {
        DCC_REPLY_EPHEMERAL(ctx, "Use this in a server.");
        return;
    }
    if (!DCC_CTX_HAS_PERMISSIONS(ctx, DCC_PERMISSION_MANAGE_MESSAGES)) {
        DCC_REPLY_EPHEMERAL(ctx, "Missing permission.");
        return;
    }

    DCC_REPLY_TEXT(ctx, "Allowed.");
}
```

For handler-local checks where any one permission in a set is enough, use
`DCC_CTX_CAN_ANY_PERMISSION()`. Use `DCC_CTX_REQUIRE_CAN_ANY()` when a missing
permission should reply and return immediately:

```c
if (DCC_CTX_CAN_ANY_PERMISSION(ctx, DCC_PERMISSION_MODERATION_BASIC)) {
    DCC_REPLY_TEXT(ctx, "You can moderate this.");
}

DCC_CTX_REQUIRE_CAN_ANY(ctx, DCC_PERMISSION_MODERATION_BASIC, "Missing moderation permission.");
```

For prefixed component IDs, extract the action without writing manual
`strncmp()` code:

```c
static void on_settings_button(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    DCC_REQUIRED_ACTION(action, ctx, "settings:");
    if (strcmp(action, "save") == 0) {
        DCC_REPLY_DONE(ctx, "Settings saved.");
    }
}
```

Response-state shortcuts mirror the interaction flow state and account for
app-level auto-defer:

```c
if (DCC_CTX_DEFERRED(ctx)) {
    DCC_EDIT(ctx, DCC_MESSAGE_TEXT("Done."));
} else if (!DCC_CTX_RESPONDED(ctx)) {
    DCC_REPLY_TEXT(ctx, "Done.");
} else if (DCC_CTX_CAN_FOLLOWUP(ctx)) {
    DCC_FOLLOWUP_TEXT(ctx, "Done.");
}
```

Short option and form accessors are also available:

```c
const char *name = DCC_CTX_ARG_STRING_OR(ctx, "name", "unknown");
int64_t limit = DCC_CTX_ARG_INT_OR(ctx, "limit", 10);
const char *birthday = DCC_CTX_FIELD_TEXT_OR(ctx, "birthday", "");
uint8_t public_age = DCC_CTX_FIELD_CHECKED_OR(ctx, "public_age", 1U);
```

## Wait And Collector Sugar

For short interactive flows, wait for the next matching object from the same
user, channel, and guild as the current handler. The result owns cloned event
objects, so always deinitialize it. New code can use the explicit
`DCC_WAIT_FOR_*` names; the shorter `DCC_WAIT_*` aliases stay available.

```c
static void on_delete(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    DCC_REPLY_V2(
        ctx,
        DCC_V2_CONTAINER_ACCENT(
            0xED4245,
            DCC_V2_TEXT("## Delete this item?"),
            DCC_V2_ACTION_ROW(
                DCC_V2_BUTTON_DANGER("Delete", "delete.confirm")
            )
        )
    );

    dcc_app_wait_result_t click = {0};
    if (DCC_WAIT_FOR_BUTTON(ctx, "delete.confirm", (uint32_t)DCC_SECONDS(30U), &click) == DCC_OK) {
        DCC_REPLY_EPHEMERAL(ctx, "Deleted.");
    }
    DCC_WAIT_RESULT_DEINIT(&click);
}
```

Prefix waits keep paginator and wizard code compact:

```c
dcc_app_wait_result_t result = {0};
if (DCC_WAIT_FOR_BUTTON_PREFIX(ctx, "page:", (uint32_t)DCC_SECONDS(30U), &result) == DCC_OK) {
    const char *custom_id = DCC_WAIT_CUSTOM_ID(&result);
    const char *action = custom_id != NULL ? custom_id + strlen("page:") : "";
    /* next, prev, first, last */
}
DCC_WAIT_RESULT_DEINIT(&result);
```

Use result sugar instead of reading owned objects directly:

```c
dcc_app_wait_result_t result = {0};
if (DCC_WAIT_FOR_SELECT(ctx, "settings.role", (uint32_t)DCC_SECONDS(30U), &result) == DCC_OK) {
    const char *first_value = DCC_WAIT_VALUE(&result, 0);
    size_t count = DCC_WAIT_VALUE_COUNT(&result);
    (void)count;
    DCC_REPLY_EPHEMERAL(ctx, first_value != NULL ? first_value : "No value.");
}
DCC_WAIT_RESULT_DEINIT(&result);
```

Modal and message waits use the same style:

```c
const char *name = DCC_WAIT_FORM_TEXT(&result, "name", "");
uint8_t visible = DCC_WAIT_FORM_CHECKED(&result, "visible", 1U);
const char *message = DCC_WAIT_MESSAGE_CONTENT(&result, "");
```

The same style is available for selects, modals, generic components, and the
next message:

```c
DCC_WAIT_FOR_SELECT(ctx, "settings.role", (uint32_t)DCC_SECONDS(30U), &result);
DCC_WAIT_FOR_MODAL_PREFIX(ctx, "profile:", (uint32_t)DCC_MINUTES(1U), &result);
DCC_WAIT_FOR_COMPONENT(ctx, (uint32_t)DCC_SECONDS(30U), &result);
DCC_WAIT_FOR_MESSAGE(ctx, (uint32_t)DCC_MINUTES(1U), &result);
DCC_COLLECT_BUTTON(ctx, "confirm", (uint32_t)DCC_SECONDS(30U), &result);
```

When waiting from supervisor code instead of a handler, build an explicit filter
and call the app-level wait sugar:

```c
dcc_app_wait_filter_t filter =
    DCC_WAIT_FILTER_COMPONENT_PREFIX("setup:");
DCC_APP_WAIT_FOR_COMPONENT_FILTERED(app, &filter, (uint32_t)DCC_SECONDS(30U), &result);
DCC_APP_WAIT_FOR_BUTTON(app, (uint32_t)DCC_SECONDS(30U), &result);
DCC_APP_WAIT_FOR_MESSAGE(app, (uint32_t)DCC_SECONDS(30U), &result);
```

## Legacy Components

```c
dcc_message_builder_t controls =
    DCC_MESSAGE_COMPONENTS(
        DCC_ACTION_ROW(
            DCC_BUTTON_PRIMARY_BUILDER("Refresh", "status.refresh"),
            DCC_BUTTON_DANGER_BUILDER("Close", "status.close")
        )
    );
```

String select options can be written inline:

```c
dcc_component_builder_t select =
    DCC_STRING_SELECT_BUILDER(
        "settings.mode",
        DCC_SELECT_OPTION("Fast", "fast"),
        DCC_SELECT_OPTION_DEFAULT("Safe", "safe")
    );
```

## Components v2

Components v2 message sugar sets `DCC_MESSAGE_FLAG_IS_COMPONENTS_V2` for you.

```c
dcc_message_builder_t message =
    DCC_MESSAGE_COMPONENTS_V2(
        DCC_V2_CONTAINER_ACCENT(
            0x5865F2,
            DCC_V2_SECTION(
                DCC_V2_BUTTON_PRIMARY("Refresh", "status.refresh"),
                DCC_V2_TEXT("# Runtime status")
            ),
            DCC_V2_SEPARATOR_SMALL(),
            DCC_V2_MEDIA_GALLERY(
                DCC_V2_MEDIA("attachment://latency.png", "Latency graph")
            )
        )
    );
```

Entity select defaults can also stay inline:

```c
dcc_component_v2_builder_t channel_select =
    DCC_V2_CHANNEL_SELECT(
        "settings.channel",
        DCC_V2_DEFAULT_VALUES(DCC_V2_DEFAULT_CHANNEL(default_channel_id)),
        DCC_V2_CHANNEL_TYPES(0U, 5U)
    );
```

## Modals

```c
DCC_SHOW_MODAL_V2(
    ctx,
    "settings-modal",
    "Settings",
    DCC_MODAL_V2_FIELD_TEXT_PLACEHOLDER("profile.name", "Display name", "Enter a display name"),
    DCC_MODAL_V2_FIELD_CHECKBOX("settings.apply", "Apply immediately", 1U)
);
```

Use an explicit builder when the modal object must be reused:

```c
dcc_modal_builder_t modal =
    DCC_MODAL_V2(
        "settings-modal",
        "Settings",
        DCC_MODAL_V2_FIELD_TEXT_PLACEHOLDER("profile.name", "Display name", "Enter a display name"),
        DCC_MODAL_V2_FIELD_CHECKBOX("settings.apply", "Apply immediately", 1U)
    );
```

Legacy modals can stay compact too. Text inputs must still be inside an action
row because that is the Discord v1 modal shape.

```c
dcc_modal_builder_t modal =
    DCC_MODAL(
        "birthday-modal",
        "Birthday",
        DCC_MODAL_FIELD_TEXT_PLACEHOLDER("birthday", "Birthday", "YYYYMMDD"),
        DCC_MODAL_FIELD_PARAGRAPH_OPTIONAL("note", "Note")
    );
```

When a component needs fields not covered by a compact macro, create the sugar
value and then set the extra fields:

```c
dcc_component_v2_builder_t upload = DCC_V2_FILE_UPLOAD("config.upload");
upload.min_values = 1U;
upload.max_values = 3U;
upload.required = 1U;
upload.has_min_values = 1U;
upload.has_max_values = 1U;
upload.has_required = 1U;
```

## Component Presets

Use component presets when a handler needs common button UIs without hand-writing
action rows. They produce normal message builders, so they work with
`DCC_REPLY`, `DCC_UPDATE`, `DCC_FOLLOWUP`, `DCC_APP_SEND`, and the same button
routes you already register with `DCC_BUTTON` or `DCC_BUTTON_PREFIX`.

```c
static void on_delete(dcc_ctx_t *ctx, void *user_data) {
    (void)user_data;

    DCC_REPLY(
        ctx,
        DCC_CONFIRM_MESSAGE_LABELS(
            "Delete this item?",
            "Delete",
            "items.delete:yes",
            "Cancel",
            "items.delete:no"
        )
    );
}

dcc_app_extension_t items =
    DCC_FEATURE(
        "items",
        DCC_FEATURE_COMMANDS(DCC_ON_SLASH("delete-item", "Ask for confirmation", on_delete)),
        DCC_FEATURE_COMPONENTS(DCC_BUTTON_PREFIX("items.delete:", on_delete_button))
    );
```

For page controls, pass the current edge state and DCC disables the impossible
direction for you:

```c
DCC_UPDATE(
    ctx,
    DCC_PAGINATOR_MESSAGE("Page 1/3", "users.page:prev", "users.page:next", 1U, 0U)
);
```

Components v2 variants are available when the message should be built from the
new layout system:

```c
DCC_REPLY(ctx, DCC_CONFIRM_V2("Continue?", "flow.confirm", "flow.cancel"));
DCC_UPDATE(ctx, DCC_PAGINATOR_V2("Page 2/5", "page.prev", "page.next", 0U, 0U));
```

Settings panels use the same route style but keep the common edit/enable/disable/reset
row out of handler code:

```c
DCC_REPLY(
    ctx,
    DCC_SETTINGS_PANEL_V2(
        "## Server settings\nChoose what to change.",
        "settings.edit",
        "settings.enable",
        "settings.disable",
        "settings.reset",
        config_enabled
    )
);
```

The app generator can create these routes for you:

```sh
dcc_new_app add preset . profile announcement --guild-only
dcc_new_app add preset . profile confirm
dcc_new_app add preset . profile paginator
dcc_new_app add preset . profile form
dcc_new_app add preset . profile settings
dcc_new_app add preset . profile wizard
dcc_new_app add preset . profile counter
dcc_new_app add preset . profile crud
dcc_new_app add preset . profile help
dcc_new_app add preset . profile menu
dcc_new_app add preset . profile poll
dcc_new_app add preset . profile profile --name profile_card
dcc_new_app add preset . profile roles --guild-only
dcc_new_app add preset . profile ticket
dcc_new_app add preset . profile welcome
dcc_new_app add preset . profile flow
dcc_new_app add action . profile gateway_status gateway --guild-only
dcc_new_app add action . profile dm_user dm --guild-only
dcc_new_app add action . profile grant_role role-add --guild-only --permission DCC_PERMISSION_MANAGE_ROLES
dcc_new_app add context-menu . profile inspect_user user --guild-only
dcc_new_app add context-menu . profile quote_message message
dcc_new_app add middleware . profile require_config config
```

`add action` is the quick-start bridge from a slash command to app-level
shortcuts. Use `send` for a generated `DCC_CTX_SEND_UI(...)` message,
`gateway` for `DCC_GATEWAY_BOT_FETCH(...)`, `dm` for typed user DMs,
`role-add`/`role-remove` for typed role management, or `voice-regions` for
`DCC_GUILD_VOICE_REGIONS_FETCH(...)`.

Those commands append the slash handler, route, and command JSON entry to the
target feature. `announcement` adds a modal composer, app-store draft state,
ephemeral preview, and send/cancel button routes. `confirm` and `paginator` add button-prefix routes; `form` adds a
typed modal-submit route and a Components v2 modal opener. `settings` adds a
settings panel, button-prefix route, and typed edit modal route. `wizard` adds
a start panel, button-prefix route, typed modal submit route, and review buttons
for setup-style flows. `counter` adds a persistent app-store counter with `+1`
and reset buttons. `crud` adds a store-backed item list with add, delete-last,
and clear actions. `help` adds a paged help center with buttons. `menu` adds a
typed select-menu navigation panel. `poll` adds
a store-backed yes/no vote panel. `profile` adds a user profile card with a
refresh button. `roles` adds a guild-only self-role menu with a typed select
route and add/remove role actions. `ticket` adds
an open-ticket button, typed modal, ephemeral ticket card, and close button.
`welcome` adds a guild member join event route, slash preview, and channel-id
macro for the target welcome channel.
`flow` adds a slash-to-modal-to-review interaction with confirm, edit, and
cancel buttons. The generated copy is ready for app-specific state or copy
changes.

## Component Sessions

Session buttons still go through `dcc_component_session_button*()` because DCC
must sign the `custom_id`. Sugar covers the option objects around that flow.

```c
dcc_component_session_options_t session_options =
    DCC_COMPONENT_SESSION_OPTIONS_USER(secret, secret_len, interaction_user_id);
session_options.now_ms = now_ms;
session_options.ttl_ms = DCC_MINUTES(5U);
session_options.channel_id = channel_id;
session_options.lock_channel = 1U;

dcc_component_session_t session;
dcc_component_session_create(&session_options, &session);

dcc_component_v2_builder_t next;
dcc_component_session_button_v2(
    &session,
    "next",
    "Next",
    DCC_BUTTON_PRIMARY,
    &next
);
```

Register signed session routes declaratively when the app definition owns the
UI routing:

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

## Interaction Flows

Use flow sugar when one handler may defer, edit the original response, and then
send followups. `DCC_FLOW_REPLY` follows the current flow state: it sends the
initial interaction response when ready, edits the original after a defer, and
falls back to a followup after the original response has already been sent.

```c
dcc_interaction_flow_t flow =
    DCC_FLOW_AUTO_DEFER_EPHEMERAL_STARTED(client, interaction, started_at_ms, DCC_MS(1500U));

DCC_FLOW_MAYBE_AUTO_DEFER(&flow, now_ms);
DCC_FLOW_REPLY_TEXT(&flow, "Working...");
DCC_FLOW_EDIT_TEXT(&flow, "Done.");
DCC_FLOW_FOLLOWUP_TEXT(&flow, "Extra details.");

if (DCC_FLOW_STATE(&flow) == DCC_INTERACTION_FLOW_FOLLOWED_UP) {
    printf("state=%s\n", DCC_FLOW_STATE_NAME(&flow));
}
```

For one-shot replies, direct interaction helpers remain available. They are thin
wrappers over the REST callback API and avoid constructing a flow object.

```c
dcc_interaction_reply_ephemeral_text(
    client,
    interaction,
    "Saved.",
    NULL,
    NULL
);

dcc_interaction_reply_error(
    client,
    interaction,
    "Invalid birthday",
    "Use YYYYMMDD and enter a real calendar date.",
    NULL,
    NULL
);

dcc_interaction_show_modal(client, interaction, &modal, NULL, NULL);
```

Use a flow when a handler may defer, edit the original response, or send
followups.

```c
dcc_interaction_flow_t flow =
    DCC_FLOW_AUTO_DEFER_EPHEMERAL(client, interaction, DCC_MS(1500U));

dcc_message_builder_t message =
    DCC_MESSAGE_EPHEMERAL("Working...");

dcc_flow_reply(&flow, &message, NULL, NULL);
```

## App REST Shortcuts

Inside an interaction handler, use context shortcuts when the target is the
current interaction channel, message, or author:

```c
DCC_CTX_SEND_TEXT(ctx, "hello");
DCC_CTX_REPLY_TEXT(ctx, "done");

DCC_CTX_THREAD_FROM_MESSAGE(ctx, "rolling paper");
DCC_CTX_LOCK_THREAD(ctx);
DCC_CTX_ARCHIVE_THREAD(ctx);
DCC_CTX_UNLOCK_THREAD(ctx);

DCC_AUTHOR_ADD_ROLE(ctx, verified_role_id);
DCC_AUTHOR_REMOVE_ROLE(ctx, pending_role_id);
DCC_CTX_MEMBER_ADD_ROLE(ctx, target_user_id, muted_role_id);
```

Use app-level shortcuts when a feature needs common Discord side effects outside
an interaction handler or when the target channel/message is explicit:

```c
DCC_CHANNEL_SEND_TEXT(app, channel_id, "hello");
DCC_CHANNEL_SEND_SAFE(app, channel_id, "@everyone safe announcement");
DCC_CHANNEL_SEND(app, channel_id, DCC_MESSAGE_SUCCESS("Saved."));
DCC_CHANNEL_FETCH(app, channel_id);
DCC_CHANNEL_TYPING(app, channel_id);
DCC_CHANNEL_EDIT_PARAMS(app, &channel_params);
DCC_CHANNEL_DELETE(app, channel_id);
DCC_CHANNEL_INVITES_FETCH(app, channel_id);
DCC_CHANNEL_INVITE_CREATE_PARAMS(app, &invite_params);
DCC_CHANNEL_PERMISSION_EDIT_PARAMS(app, channel_id, &permission_overwrite);
DCC_CHANNEL_PERMISSION_DELETE(app, channel_id, role_id);
DCC_GUILD_CHANNELS_FETCH(app, guild_id);
DCC_GUILD_CHANNEL_CREATE_PARAMS(app, &channel_params);
DCC_GUILD_CHANNELS_REORDER_PARAMS(app, &channel_positions_params);

DCC_GUILD_ROLES_FETCH(app, guild_id);
DCC_GUILD_ROLE_CREATE_PARAMS(app, &role_params);
DCC_ROLE_EDIT_PARAMS(app, &role_params);
DCC_ROLE_DELETE(app, guild_id, role_id);

DCC_MESSAGE_EDIT(app, channel_id, message_id, DCC_MESSAGE_TEXT("updated"));
DCC_MESSAGE_DELETE(app, channel_id, message_id);
DCC_MESSAGE_PIN(app, channel_id, message_id);
DCC_MESSAGE_REACT(app, channel_id, message_id, "thumbsup");
DCC_MESSAGE_UNREACT(app, channel_id, message_id, "thumbsup");
DCC_MESSAGE_CLEAR_REACTIONS(app, channel_id, message_id);
DCC_MESSAGE_CROSSPOST(app, channel_id, message_id);

DCC_MESSAGE_THREAD(app, channel_id, message_id, "rolling paper");
DCC_THREAD_LOCK(app, thread_id);
DCC_THREAD_UNLOCK(app, thread_id);

DCC_APP_SEND_TEXT(app, channel_id, "hello");
DCC_APP_SEND_SAFE(app, channel_id, "@everyone safe announcement");
DCC_APP_SEND(app, channel_id, DCC_MESSAGE_SUCCESS("Saved."));
DCC_APP_EDIT_MESSAGE(app, channel_id, message_id, DCC_MESSAGE_TEXT("updated"));
DCC_APP_DELETE_MESSAGE(app, channel_id, message_id);

DCC_APP_THREAD_FROM_MESSAGE(app, channel_id, message_id, "rolling paper");
DCC_APP_LOCK_THREAD(app, thread_id);

DCC_MEMBER_ADD_ROLE(app, guild_id, user_id, role_id);
DCC_MEMBER_REMOVE_ROLE(app, guild_id, user_id, role_id);
DCC_MEMBER_FETCH(app, guild_id, user_id);
DCC_MEMBER_TIMEOUT(app, guild_id, user_id, "2026-06-23T12:00:00Z");
DCC_MEMBER_CLEAR_TIMEOUT(app, guild_id, user_id);
DCC_MEMBER_MOVE(app, guild_id, user_id, voice_channel_id);
DCC_MEMBER_KICK(app, guild_id, user_id);
DCC_MEMBER_BAN(app, guild_id, user_id);
DCC_MEMBER_BAN_SECONDS(app, guild_id, user_id, 3600U);
DCC_MEMBER_UNBAN(app, guild_id, user_id);
DCC_ME_SET_NICK(app, guild_id, "new nick");

DCC_GUILD_MEMBER_ADD_ROLE(app, guild_id, user_id, role_id);
DCC_GUILD_MEMBER_REMOVE_ROLE(app, guild_id, user_id, role_id);
DCC_GUILD_BANS_FETCH(app, guild_id);
DCC_GUILD_PRUNE_COUNT(app, guild_id, 7U);
DCC_GUILD_AUDIT_LOG_FETCH_PAGE(app, guild_id, user_id, action_type, 0U, 0U, 50U);
DCC_GUILD_INTEGRATIONS_FETCH(app, guild_id);
DCC_GUILD_INTEGRATION_SYNC(app, guild_id, integration_id);
DCC_GUILD_WIDGET_FETCH(app, guild_id);
DCC_GUILD_WIDGET_EDIT_PARAMS(app, guild_id, &widget_params);
DCC_GUILD_VANITY_URL_FETCH(app, guild_id);
DCC_GUILD_ONBOARDING_FETCH(app, guild_id);
DCC_GUILD_ONBOARDING_EDIT_PARAMS(app, &onboarding_params);
DCC_GUILD_WELCOME_SCREEN_FETCH(app, guild_id);
DCC_GUILD_WELCOME_SCREEN_EDIT_PARAMS(app, guild_id, &welcome_params);
DCC_AUTO_MOD_RULES_FETCH(app, guild_id);
DCC_AUTO_MOD_RULE_CREATE_PARAMS(app, guild_id, &auto_mod_rule_params);
DCC_AUTO_MOD_RULE_EDIT_PARAMS(app, guild_id, &auto_mod_rule_params);
DCC_AUTO_MOD_RULE_DELETE(app, guild_id, rule_id);
DCC_GUILD_EMOJIS_FETCH(app, guild_id);
DCC_GUILD_EMOJI_CREATE_PARAMS(app, guild_id, &emoji_params);
DCC_GUILD_EMOJI_EDIT_PARAMS(app, guild_id, &emoji_params);
DCC_GUILD_EMOJI_DELETE(app, guild_id, emoji_id);
DCC_GUILD_STICKERS_FETCH(app, guild_id);
DCC_GUILD_STICKER_CREATE_PARAMS(app, &sticker_params);
DCC_STICKER_PACKS_FETCH(app);
DCC_GUILD_MESSAGES_SEARCH_PARAMS(app, guild_id, &message_search_params);
DCC_GATEWAY_BOT_FETCH(app);
DCC_MY_VOICE_STATE_FETCH(app, guild_id);
DCC_USER_VOICE_STATE_EDIT_PARAMS(app, &voice_state_params);
DCC_VOICE_REGIONS_FETCH(app);
DCC_GUILD_VOICE_REGIONS_FETCH(app, guild_id);
DCC_SKUS_FETCH(app, application_id);
DCC_ENTITLEMENTS_FETCH_PAGE(app, application_id, user_id, sku_ids, sku_count, 0U, 0U, 25U, guild_id, 1U);
DCC_TEST_ENTITLEMENT_CREATE_PARAMS(app, &test_entitlement_params);
DCC_ENTITLEMENT_CONSUME(app, application_id, entitlement_id);

DCC_GUILD_SCHEDULED_EVENTS_FETCH(app, guild_id);
DCC_SCHEDULED_EVENT_CREATE_PARAMS(app, &scheduled_event_params);
DCC_SCHEDULED_EVENT_EDIT_PARAMS(app, &scheduled_event_params);
DCC_SCHEDULED_EVENT_USERS_FETCH_PAGE(app, guild_id, event_id, 25U, 0U, 0U);

DCC_CHANNEL_WEBHOOKS_FETCH(app, channel_id);
DCC_GUILD_WEBHOOKS_FETCH(app, guild_id);
DCC_WEBHOOK_CREATE_PARAMS(app, &webhook_params);
DCC_WEBHOOK_SEND_TEXT(app, webhook_id, webhook_token, "hello from webhook");
DCC_WEBHOOK_SEND(app, webhook_id, webhook_token, DCC_MESSAGE_SUCCESS("Posted."));
DCC_WEBHOOK_SEND_UI(app, webhook_id, webhook_token, DCC_UI_TEXT("Components v2 webhook"));
DCC_WEBHOOK_MESSAGE_EDIT(app, webhook_id, webhook_token, message_id, DCC_MESSAGE_TEXT("edited"));
DCC_WEBHOOK_MESSAGE_DELETE(app, webhook_id, webhook_token, message_id);

DCC_ME_FETCH(app);
DCC_USER_FETCH(app, user_id);
DCC_USER_DM_SEND_TEXT(app, user_id, "hello in DM");
DCC_USER_DM_SEND(app, user_id, DCC_MESSAGE_SUCCESS("Sent."));
DCC_USER_DM_SEND_UI(app, user_id, DCC_UI_TEXT("Components v2 DM"));
DCC_GROUP_DM_RECIPIENT_ADD_PARAMS(app, &group_dm_recipient_params);
DCC_GROUP_DM_RECIPIENT_REMOVE(app, group_dm_channel_id, user_id);
DCC_GUILD_INVITES_FETCH(app, guild_id);
DCC_INVITE_FETCH_FULL(app, invite_code);
DCC_INVITE_DELETE(app, invite_code);

DCC_STAGE_CREATE_PARAMS(app, &stage_params);
DCC_STAGE_FETCH(app, stage_channel_id);
DCC_STAGE_EDIT_PARAMS(app, &stage_params);
DCC_STAGE_DELETE(app, stage_channel_id);

DCC_TEMPLATE_FETCH(app, template_code);
DCC_GUILD_TEMPLATES_FETCH(app, guild_id);
DCC_GUILD_TEMPLATE_CREATE_PARAMS(app, &template_params);
DCC_GUILD_TEMPLATE_SYNC(app, guild_id, template_code);
DCC_GUILD_TEMPLATE_DELETE(app, guild_id, template_code);

DCC_APP_ADD_MEMBER_ROLE(app, guild_id, user_id, role_id);
DCC_APP_REMOVE_MEMBER_ROLE(app, guild_id, user_id, role_id);
```

`<dcc/sugar.h>` exposes the same common app, channel, message, thread, and
member shortcuts. Small feature modules can include `<dcc/sugar/app_actions.h>`
when they want only that focused subset without pulling in the full umbrella
sugar header. That focused header also includes `<dcc/sugar/messages.h>`, so
`DCC_MESSAGE_TEXT()` and `DCC_MESSAGE_COMPONENTS_V2()` are available for direct
sends and edits.

## Managed Latest Messages

For announcement, registration, and schedule messages that should always remain
the latest message in a channel, let DCC delete the previously saved message,
post the new payload, parse the created message ID, and save the new reference.

```sh
dcc_new_app add config . birthday_channel channel BIRTHDAY_CHANNEL
dcc_new_app add latest-message . birthday registration birthday_channel
dcc_new_app add scheduled-latest . birthday daily_registration birthday_channel --daily-kst 09:00
```

```c
dcc_message_builder_t registration =
    DCC_MESSAGE_COMPONENTS_V2(
        DCC_V2_CONTAINER_ACCENT(
            0x5865F2,
            DCC_V2_TEXT("**다른 유저들의 생일이 궁금하시다면 아래 버튼을 눌러보세요!**"),
            DCC_V2_ACTION_ROW(
                DCC_V2_BUTTON_PRIMARY("🎊 유저 생일 스케줄", "birthday.schedule")
            )
        )
    );

DCC_APP_PUBLISH_LATEST_STORE(
    app,
    "birthday.registration.message",
    birthday_channel_id,
    &registration,
    NULL,
    NULL
);
```

For small app state, the same store can persist plain strings or integers:

```c
DCC_APP_STORE_SET_U64(app, "birthday.last_kst_day", today_key);
last_key = DCC_APP_STORE_U64_OR(app, "birthday.last_kst_day", 0U);
```

## REST Response IDs

When you use a raw REST callback and need the created resource ID, parse only
the field you need:

```c
static void created_message(
    dcc_client_t *client,
    const dcc_rest_response_t *response,
    void *user_data
) {
    dcc_snowflake_t message_id = 0;
    if (dcc_rest_response_message_id(response, &message_id) == DCC_OK) {
        save_message_id(message_id);
    }
    (void)client;
    (void)user_data;
}
```

## Operations

```c
dcc_hot_reload_canary_options_t canary =
    DCC_HOT_RELOAD_CANARY_OPTIONS_DEFAULT();

dcc_hot_reload_options_t hot_reload =
    DCC_HOT_RELOAD_ISOLATED_CANARY_OPTIONS("dcc_hot_reload_worker", canary);

dcc_rest_firewall_options_t firewall =
    DCC_REST_FIREWALL_OPTIONS();

dcc_replay_record_t record =
    DCC_REPLAY_GATEWAY_RECORD("READY", now_ms, payload, payload_len);
```

## Lifetime Rules

Variadic macros create block-scoped compound literal arrays. This is ideal when
the value is immediately serialized, registered, or passed into DCC during the
same block.

Use an explicit array or the matching `*_ARRAY` macro when data must outlive the
current block or when it is generated dynamically:

```c
dcc_component_v2_builder_t rows[2];
size_t row_count = build_rows(rows, DCC_ARRAY_LEN(rows));

dcc_message_builder_t message =
    DCC_MESSAGE_COMPONENTS_V2_ARRAY(rows, row_count);
```

Use the explicit builder API when you need validation after each setter, complex
branching, dynamic allocation, or ABI-stable objects returned from another
module.
