# CLI Tools

DCC installs runtime helper tools when `DCC_BUILD_TOOLS=ON`.

## dcc_new_app

Creates a small sugar-first DCC bot project.

```sh
dcc_new_app mybot
cd mybot
cp .env.example .env
dcc_new_app add feature . profile
dcc_new_app add command . profile greet
dcc_new_app add subcommand . profile greet stats
dcc_new_app add button . profile refresh
dcc_new_app add select . profile role
dcc_new_app add modal . profile edit
dcc_new_app add autocomplete . profile profile
dcc_new_app add event . profile ready
dcc_new_app add view . profile dashboard
dcc_new_app add config . log_channel channel LOG_CHANNEL
dcc_new_app add error-handler . bot_error friendly
dcc_new_app add latest-message . profile status log_channel
dcc_new_app add scheduled-latest . profile daily_status log_channel --daily-kst 09:00
dcc_new_app add message-command . profile hello --prefix '!'
dcc_new_app add task . profile heartbeat --every-seconds 60
dcc_new_app add action . profile gateway_status gateway --guild-only
dcc_new_app add action . profile dm_user dm --guild-only
dcc_new_app add action . profile grant_role role-add --guild-only --permission DCC_PERMISSION_MANAGE_ROLES
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
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/.local"
cmake --build build
```

The generated project includes `src/main.c`, `src/config.h`, `src/ping.c`,
`src/ping.h`, `commands.json`, and a README.
The generated app definition uses `DCC_BOT(...)`, which includes
`DCC_APP_DEV_MODE()`, so it reconciles
the sample `/ping` slash command on READY, infers the application id from the
Gateway READY event, auto-defers slow interactions, and enables friendly default
errors. The app template reads `DCC_TOKEN` or `DISCORD_TOKEN`,
`DCC_INTENTS`, `DCC_AUTO_DEFER_MS`, `DCC_AUTO_DEFER_EPHEMERAL`, and
`DCC_STORE_FILE`, and `DCC_COMMAND_GUILD_ID` after loading `.env`, then binds
feature config with `DCC_CONFIG(...)` before running through `DCC_RUN_APP_ENV(...)`.
`DCC_INTENTS` can be `all`, `default`, `messages`, `guilds,message_content`,
or a decimal/hex mask.
The default template includes `BOT_DEBUG=false` as a typed boolean config example.
Add more typed config fields with `dcc_new_app add config PATH FIELD_NAME TYPE
ENV_NAME`; it updates `src/config.h`, the `DCC_CONFIG(...)` binding list, and
`.env.example` together. Generated feature files include `config.h`, so handlers can use
`BOT_CONFIG(user_data)` or `BOT_CTX_CONFIG(ctx)` without duplicating the config
struct.
The sample handler uses `DCC_CTX_STORE_U64_OR(...)` and
`DCC_CTX_STORE_SET_U64(...)` to keep a persistent ping count without
hand-written app/store plumbing. Routes are declared with the sugar-first `DCC_BOT(...)`,
`DCC_DEFINE_PUBLIC_FEATURE(...)`, and `DCC_APP_USE(DCC_USE_FEATURE(...))` style. Generated slash
commands use typed args by default through `DCC_FEATURE_COMMAND_ROUTES(...)`,
`DCC_COMMAND_ROUTE_NO_OPTIONS*` for empty commands, and `DCC_COMMAND_ROUTE*`
with generated `*_PARAMS` lists; component, event,
message-command, view, action, and task routes use the shorter `DCC_TYPED_*`,
`DCC_COMPONENT_ARGS(...)`, `DCC_FORM_ARGS(...)`, `DCC_ON_*`, and `DCC_TASK_*`
aliases. Preset generation emits ready-to-edit confirm, paginator, modal-form,
settings-panel, wizard, store-backed counter, store-backed CRUD, store-backed poll, or modal confirmation flows using component and modal sugar. Generated handlers
reply with context-first `DCC_CTX_*` helpers, route status-returning calls
through `DCC_CTX_TRY(...)`, and use Components v2 `DCC_UI_*` builders such as
`DCC_CTX_REPLY_UI(ctx, DCC_UI_TEXT(reply))`.
If a generated command later gains subcommands, the tool converts that command's
Discord schema to a builder route while reusing the same params list for typed
binding.

Use `dcc_new_app add feature PATH FEATURE_NAME` to add another feature file. When the
generated markers are still present, it creates `src/FEATURE_NAME.c`,
`src/FEATURE_NAME.h`, adds the source to `CMakeLists.txt`, includes the header from
`src/main.c`, adds `DCC_USE_FEATURE(FEATURE_NAME, &config)`, and appends the slash
command to `commands.json`.

Use `dcc_new_app add config PATH FIELD_NAME string|bool|u64|i64|channel|role|guild|snowflake ENV_NAME`
to add app-level typed environment config without editing `src/main.c` by hand.
For example, `dcc_new_app add config . log_channel channel LOG_CHANNEL` adds
`dcc_snowflake_t log_channel;` to `src/config.h`, binds
`DCC_CONFIG_CHANNEL("LOG_CHANNEL", &config.log_channel)`, and adds
`LOG_CHANNEL=123456789012345678` to `.env.example`.

Use `dcc_new_app add command PATH FEATURE_NAME COMMAND_NAME` to add another slash
command to an existing generated feature file. It appends a typed args struct, a
`DCC_COMMAND_ARGS_IMPL(...)` handler, a `*_PARAMS` list, a
`DCC_COMMAND_ROUTE_DATA(...)` route, and a
`commands.json` entry when the feature still
contains the generated command markers.
Pass guard options such as `--guild-only`, `--owner USER_ID`,
`--permission DCC_PERMISSION_MANAGE_GUILD`, or `--cooldown-user 1:5000` to emit
`DCC_COMMAND_ROUTE_DATA_GUARDED(...)` automatically. Generated guarded routes use
the short policy aliases such as
`DCC_ONLY_GUILD()`, `DCC_OWNER_ONLY(...)`, `DCC_CAN(...)`, and
`DCC_RATE_LIMIT_USER(...)`.

Use `dcc_new_app add subcommand PATH FEATURE_NAME COMMAND_NAME SUBCOMMAND_NAME` to
add a slash subcommand handler to an existing generated command. The tool adds a
typed subcommand args struct, a `*_PARAMS` list, a
`DCC_SUBCOMMAND_ROUTE_DATA(...)` route, and rewrites the generated command
schema with `DCC_CMD_SUB_PARAMS(...)` so the Discord command schema matches the
new route without duplicating option definitions.
If the parent command already has route guards, the schema rewrite preserves them
by converting to `DCC_ON_TYPED_SLASH_BUILDER_DATA_GUARDED(...)`.

Use `dcc_new_app add button PATH FEATURE_NAME BUTTON_NAME` to add an exact custom ID
typed button route such as `FEATURE_NAME.BUTTON_NAME`. `dcc_new_app add select`
creates a typed select-menu route with the same custom ID convention, and
`dcc_new_app add modal` creates a typed modal submit route. All three generate a
small args struct and inline readable bindings with `DCC_COMPONENT_ARGS(...)` or
`DCC_FORM_ARGS(...)`. They accept the same guard options and emit the matching
`DCC_BUTTON_PARAMS_DATA_POLICY(...)`, `DCC_SELECT_PARAMS_DATA_POLICY(...)`, or
`DCC_MODAL_PARAMS_DATA_POLICY(...)` when a guard is present.

Guard option summary:

```sh
dcc_new_app add command . admin reload --guild-only --owner 803177060956766231ULL
dcc_new_app add button . admin confirm --role 123456789012345678ULL
dcc_new_app add select . admin target --any-role 1ULL,2ULL --cooldown-user 3:10000
```

Use `dcc_new_app add autocomplete PATH FEATURE_NAME COMMAND_NAME` to add an
typed autocomplete handler for a generated slash command. When the command still
has a generated `*_PARAMS` list, the route is added with
`DCC_AUTOCOMPLETE_PARAMS_DATA(...)`, or
`DCC_AUTOCOMPLETE_PARAMS_DATA_GUARDED(...)` when guard options are present. The
tool also rewrites the generated `name` string option to
`STRING_AUTOCOMPLETE`/`DCC_REQUIRED_STRING_AUTOCOMPLETE(...)` and marks the generated
`commands.json` option with `"autocomplete": true`.

Use `dcc_new_app add event` with `ready`, `message_create`, `message_update`,
`message_delete`, or a lower-case event name such as `guild_create`; unknown
names are generated as `DCC_EVENT_*` generic event routes, so invalid event names
fail at compile time instead of being silently ignored.
Common payload events such as `member_join`, `role_update`, `channel_create`,
`typing_start`, `reaction_add`, and `voice_state_update` generate typed Gateway
event sugar like `DCC_MEMBER_JOIN_FN(...)` and `DCC_MEMBER_JOIN_DATA(...)`
instead of raw `dcc_event_*()` accessor boilerplate.

Use `dcc_new_app add view PATH FEATURE_NAME VIEW_NAME` to append a slash opener and
a persistent `DCC_FEATURE_VIEWS(...)` block with a button-prefix route and modal
route. The generated custom IDs use the `FEATURE_NAME.VIEW_NAME.*` convention, so
Components v2 messages can keep their routes after process restarts without
hand-registering each custom ID.

Use `dcc_new_app add latest-message PATH FEATURE_NAME COMMAND_NAME CHANNEL_FIELD` to
generate a slash command that publishes a Components v2 message through
`DCC_APP_PUBLISH_LATEST_STORE(...)`. `CHANNEL_FIELD` is a field from
`bot_config_t`, usually created with `dcc_new_app add config . log_channel
channel LOG_CHANNEL`. DCC deletes the previously saved message, posts the new
payload, and stores the new message id in the app store.

Use `dcc_new_app add scheduled-latest PATH FEATURE_NAME TASK_NAME CHANNEL_FIELD`
for the same latest-message pattern on a task schedule. It accepts the same
schedule flags as `add task`, including `--every-seconds 60`, `--every-hours 1`,
and `--daily-kst 09:00`.

Use `dcc_new_app add message-command PATH FEATURE_NAME COMMAND_NAME` for prefix
commands. The default prefix is `!`; pass `--prefix ?` to generate another
prefix. Use `dcc_new_app add task PATH FEATURE_NAME TASK_NAME` for a background
loop. Tasks default to `--every-seconds 60` and also accept `--every-ms`,
`--every-minutes`, `--every-hours`, or `--daily-kst HH:MM`.

Use `dcc_new_app add action PATH FEATURE_NAME ACTION_NAME send|gateway|dm|role-add|role-remove|voice-regions`
to append a slash command that demonstrates a common app-level shortcut without
hand-writing REST boilerplate. `send` posts a Components v2 message with
`DCC_CTX_SEND_UI(...)`, `gateway` queues `DCC_GATEWAY_BOT_FETCH(...)`, `dm`
generates typed `user`/`message` options and calls `DCC_USER_DM_SEND_TEXT(...)`,
`role-add` and `role-remove` generate typed `user`/`role` options and call
`DCC_MEMBER_ADD_ROLE(...)` or `DCC_MEMBER_REMOVE_ROLE(...)`, and
`voice-regions` queues `DCC_GUILD_VOICE_REGIONS_FETCH(...)` for the current
guild. Action routes accept the same guard options as slash commands, so role
actions can be generated with `--guild-only --permission DCC_PERMISSION_MANAGE_ROLES`.

Use `dcc_new_app add preset PATH FEATURE_NAME announcement` to generate a modal
announcement composer with ephemeral preview, app-store draft state, and
send/cancel buttons. Use `confirm` to generate a confirmation
slash command plus yes/no button routes. Use `paginator` to generate a paginated
message skeleton with previous/next routes. Use `form` to generate a slash
command that opens a Components v2 modal plus a typed modal-submit route.
Use `settings` to generate a settings panel with edit/enable/disable/reset
buttons and a typed edit modal route. Use `wizard` to generate a start panel,
button-prefix routes, a typed modal submit route, and review buttons for
finish/edit/cancel. Use `counter` to generate a persistent app-store counter
with `+1` and reset buttons. Use `flow` to generate a slash command that opens
a modal, shows a review message, and handles confirm/edit/cancel buttons. Use
`crud` to generate a store-backed list with add, delete-last, and clear actions.
Use `help` to generate a paged help center with buttons. Use `menu` to generate
a typed select-menu navigation panel. Use `poll` to
generate a store-backed yes/no vote panel with reset. Use `profile` to generate
a user profile card with a refresh button. Use `roles` to generate a guild-only
self-role menu with typed select handling and add/remove role actions. Use `ticket`
to generate an open-ticket button, typed modal, ephemeral ticket card, and close
button. Use `welcome` to generate a guild member join event route plus slash
preview; set the generated `*_WELCOME_CHANNEL_ID` macro to the target channel.
Presets accept the same interaction guard
flags as slash and component routes, so `--guild-only`, `--owner`, `--role`,
`--permission`, and cooldown guards stay declarative.

## dcc_hot_reload_host

Runs a Discord client and forwards bot logic to a hot reload module.

```sh
dcc_hot_reload_host --check
dcc_hot_reload_host --global
dcc_hot_reload_host --guild 123456789012345678
```

Important environment variables:

- `DISCORD_TOKEN`
- `DCC_BOT_MODULE`
- `DCC_HOT_RELOAD_WORKER`
- `DCC_HOT_RELOAD_HEALTH_ADDRESS`
- `DCC_HOT_RELOAD_HEALTH_PORT`

## dcc_hot_reload_worker

Loads a bot module in an isolated worker process. It is normally launched by
`dcc_hot_reload_host`.

## dcc_command_sync

Plans or applies application command reconciliation.

```sh
dcc_command_sync --commands commands.json --application-id "$DCC_APPLICATION_ID" --plan
dcc_command_sync --commands commands.json --application-id "$DCC_APPLICATION_ID" --apply
dcc_command_sync --commands commands.json --application-id "$DCC_APPLICATION_ID" --guild "$DCC_COMMAND_GUILD_ID" --apply
```

Use `--remote remote.json` to compare against a captured remote fixture without
network access. `DCC_TOKEN`, `BOT_TOKEN`, or `DISCORD_TOKEN` are accepted for
REST writes. For projects generated by `dcc_new_app`, this CLI is optional
because `DCC_APP_DEV_MODE()` runs the same reconciliation when the app starts.

## dcc_replay

Summarizes, validates, or replays captured Discord JSONL payloads.

```sh
dcc_replay --file events.jsonl --summary
dcc_replay --file events.jsonl --validate
dcc_replay --file events.jsonl --module ./bot.so --isolated
```

## dcc_interaction_webhook

Runs the shardless interaction webhook example with Discord Ed25519
verification.

```sh
dcc_interaction_webhook --check
```

Important environment variables:

- `DISCORD_PUBLIC_KEY`
- `DCC_INTERACTION_ADDRESS`
- `DCC_INTERACTION_PORT`
- `DCC_INTERACTION_PATH`
