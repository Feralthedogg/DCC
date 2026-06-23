# Getting Started

The normal path is to install a published DCC release. The release archive is
self-contained and includes a compatible LLAM runtime; the install script also
refreshes the DCC-tested LLAM 2.1.0 runtime through LLAM's release installer.

## Install Release

```sh
curl -fsSL https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local"
```

For system-wide installs use a system prefix:

```sh
curl -fsSL https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh |
  sudo sh -s -- --prefix /usr/local
```

Upgrade an existing prefix with:

```sh
curl -fsSL https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local" --force
```

Use `--skip-llam` only when you want the LLAM copy bundled in the DCC archive
to remain untouched. By default the script installs DCC and then installs LLAM
2.1.0 into the same prefix. Pass `--llam-version latest` when you intentionally
want the newest LLAM release instead of the DCC-tested runtime.

The POSIX installer detects the host target automatically. If you pass
`--target` manually and it differs from the current machine, the installer emits
a warning because the installed binaries and static libraries may not run there.

Point CMake consumers at the prefix:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/.local"
cmake --build build
```

## Build From Source

This path is for working on DCC itself.

## Requirements

- CMake 3.20 or newer
- C11 compiler
- OpenSSL development headers
- LLAM checkout at `../LLAM`
- Optional libopus development headers for voice Opus encode/decode

Linux also needs `liburing` for LLAM's io_uring backend.
macOS and BSD use LLAM's kqueue backend. FreeBSD, OpenBSD, NetBSD, and
DragonFlyBSD builds need CMake, OpenSSL or LibreSSL development headers, and
`pkgconf` when validating pkg-config consumers. NetBSD also needs `librt`; DCC
propagates that through CMake package files and pkg-config metadata.

## Build

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ROOT=../LLAM
cmake --build build
ctest --test-dir build --output-on-failure
```

## Create A Bot

The fastest path is the scaffolded app runtime project:

```sh
dcc_new_app mybot
cd mybot
cp .env.example .env
$EDITOR .env
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
./build/mybot
```

The generated app uses `DCC_BOT(...)`, which includes `DCC_APP_DEV_MODE()`, so
it syncs `/ping` automatically, infers the application id from the Gateway READY
event, auto-defers slow interactions, and enables friendly default error
replies. Its `.env.example`
also enables `DCC_STORE_FILE=bot-state.kv`, so the sample handler can use the
app-owned store for a persistent ping count. For fast development, set
`DCC_COMMAND_GUILD_ID` in `.env` to use guild-scoped commands; leave it
commented out for global commands. `src/main.c` stays as the entrypoint, while
feature routes live in feature files such as `src/ping.c`. Shared typed config
lives in `src/config.h`, and generated feature files include it so handlers can use
`BOT_CONFIG(user_data)` or `BOT_CTX_CONFIG(ctx)`. Generated slash commands use
typed args by default with `DCC_FEATURE_COMMAND_ROUTES(...)`,
`DCC_COMMAND_ROUTE_NO_OPTIONS*`, `DCC_COMMAND_ROUTE*`, and a small `*_PARAMS`
list, so handlers receive a small C struct instead of manually reading every option from the context. Generated
handlers are declared with `DCC_COMMAND_IMPL`, `DCC_HANDLER`, `DCC_COMMAND_ARGS_IMPL`,
`DCC_BUTTON_ARGS_IMPL`, `DCC_SELECT_ARGS_IMPL`, `DCC_MODAL_ARGS_IMPL`,
`DCC_READY_FN`, and `DCC_TASK_FN`, then reply with context-first `DCC_CTX_*`
helpers and short Components v2 `DCC_UI_*` builders. Use
`dcc_new_app add feature . profile` to create and wire another feature file, then
`dcc_new_app add command . profile greet`,
`dcc_new_app add subcommand . profile greet stats`,
`dcc_new_app add button . profile refresh`, `dcc_new_app add select . profile role`,
`dcc_new_app add modal . profile edit`, `dcc_new_app add autocomplete . profile profile`,
`dcc_new_app add event . profile ready`,
`dcc_new_app add view . profile dashboard`,
`dcc_new_app add config . log_channel channel LOG_CHANNEL`,
`dcc_new_app add latest-message . profile status log_channel`,
`dcc_new_app add scheduled-latest . profile daily_status log_channel --daily-kst 09:00`,
`dcc_new_app add message-command . profile hello --prefix '!'`, or
`dcc_new_app add task . profile heartbeat --every-seconds 60` to append handlers
and routes to that feature file, or typed app config. Use
`dcc_new_app add action . profile gateway_status gateway --guild-only` when you
want a generated slash command that demonstrates app-level shortcuts such as
`DCC_GATEWAY_BOT_FETCH(...)`, `DCC_GUILD_VOICE_REGIONS_FETCH(...)`, or
`DCC_CTX_SEND_UI(...)` without writing REST wiring by hand. `add action` also
generates typed target-user workflows such as
`dcc_new_app add action . profile dm_user dm --guild-only` and
`dcc_new_app add action . profile grant_role role-add --guild-only --permission DCC_PERMISSION_MANAGE_ROLES`,
so DM and role-management commands start with `DCC_USER_DM_SEND_TEXT(...)` or
`DCC_MEMBER_ADD_ROLE(...)` already wired. Use
`dcc_new_app add preset . profile announcement --guild-only` for a modal-based
announcement composer with ephemeral preview and send/cancel buttons. Use
`dcc_new_app add preset . profile confirm` or
`dcc_new_app add preset . profile paginator` when you want generated slash
commands plus button routes for common UI flows. Use
`dcc_new_app add preset . profile form` for a slash command that opens a modal
and receives typed form args. Use `dcc_new_app add preset . profile settings`
for a settings panel with buttons plus an edit modal. Use
`dcc_new_app add preset . profile wizard` for a slash-to-button-to-modal setup
flow with review buttons. Use `dcc_new_app add preset . profile counter` for a
store-backed slash command with `+1` and reset buttons. Use
`dcc_new_app add preset . profile crud` for a store-backed item list with add,
delete-last, and clear actions. Use `dcc_new_app add preset . profile help`
for a paged help center with command/component/about buttons. Use
`dcc_new_app add preset . profile menu`
for a typed select-menu navigation panel. Use `dcc_new_app add preset . profile poll`
for a store-backed yes/no vote panel. Use
`dcc_new_app add preset . profile profile --name profile_card` for a user profile
card with a refresh button. Use `dcc_new_app add preset . profile roles --guild-only`
for a generated self-role menu with typed select handling and add/remove role
actions. Use
`dcc_new_app add preset . profile ticket` for an open-ticket button, typed modal,
ephemeral ticket card, and close button. Use `dcc_new_app add preset . profile welcome`
for a member-join welcome event plus slash preview. Use
`dcc_new_app add preset . profile flow` for a slash-to-modal-to-confirm flow.
Button, select, and modal generation use typed params so
handlers receive a generated args struct instead of parsing `custom_id`, values,
or form fields by hand. Subcommand generation rewrites the generated command
schema to include `DCC_CMD_SUB_PARAMS(...)`; autocomplete generation also
flips the generated slash command's `name` option to `STRING_AUTOCOMPLETE` and
adds a typed `DCC_AUTOCOMPLETE_PARAMS_DATA(...)` route so Discord sends
autocomplete interactions for that option.

`dcc_command_sync` is still available when you want an explicit plan or a
manual apply:

```sh
set -a
. ./.env
set +a
DCC_APPLICATION_ID=your-application-id
dcc_command_sync --commands commands.json --application-id "$DCC_APPLICATION_ID" --plan
```

## Minimal App

```c
#include <dcc/sugar.h>

DCC_SLASH_FN(ping) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_SIMPLE_BOT_MAIN(
    "minimal",
    DCC_LISTEN_SLASH("ping", "Reply with pong", ping)
)
```

## Include Style

Application code should usually start with the sugar header:

```c
#include <dcc/sugar.h>
```

It includes the common public types and adds compact literals for options,
commands, messages, embeds, components, modals, sessions, flows, hot reload,
firewall, and replay records:

```c
dcc_message_builder_t message =
    DCC_MESSAGE_TEXT_EMBEDS(
        "pong",
        DCC_EMBED_COLOR("Latency", "Gateway online", DCC_COLOR_BLURPLE)
    );
```

Use the aggregate header when you want every public declaration without sugar
macros:

```c
#include <dcc/dcc.h>
```

Use focused headers in low-level library code:

```c
#include <dcc/client.h>
#include <dcc/events/listeners.h>
#include <dcc/rest/application_commands.h>
```

Public headers live under `include/dcc/`. Internal headers under
`src/internal/` are not part of the package API.
