# Getting Started

This guide takes the recommended application path: install LLAM and DCC,
scaffold one bot, confirm `/ping`, and add one feature. If another framework
will own the client lifecycle, start with
[Choose An API Layer](concepts/api-layers.md) instead.

## Before You Start

Have these ready:

- a Discord application with a bot user;
- the bot token stored outside source control;
- the bot invited to a development server;
- a C11 compiler and CMake 3.20 or newer.

Your first success criterion is intentionally small: the process reaches READY,
the development guild contains `/ping`, and `/ping` replies. Add features only
after this works.

## 1. Install LLAM And DCC

DCC keeps LLAM as a separate runtime dependency. Install both into the same
prefix:

```sh
curl -fsSL https://github.com/Feralthedogg/LLAM/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local"
curl -fsSL https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local"
```

To let the DCC installer fetch its tested LLAM version, use one command:

```sh
curl -fsSL https://github.com/Feralthedogg/DCC/releases/latest/download/install.sh |
  sh -s -- --prefix "$HOME/.local" --install-llam
```

Use `--force` to upgrade an existing prefix. For a system installation, replace
`$HOME/.local` with `/usr/local` and run the installer with appropriate
permissions.

## 2. Scaffold A Bot

```sh
dcc_new_app mybot
cd mybot
cp .env.example .env
$EDITOR .env
```

Put the bot token in `.env`. While developing, set `DCC_COMMAND_GUILD_ID` to the
ID of one test guild so command changes appear quickly. Never commit `.env`.

The generated project starts with `/ping`, automatic command reconciliation,
friendly error replies, auto-defer, and a file-backed store.

## 3. Build And Run

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/.local"
cmake --build build
./build/mybot
```

Look for the READY log, then run `/ping` in the development guild.

!!! success "Checkpoint"
    Continue only after `/ping` replies. If the bot does not reach READY or the
    command is missing, use [Troubleshooting](troubleshooting.md).

## 4. Add One Feature

Create a feature module and one typed slash command:

```sh
dcc_new_app add feature . profile
dcc_new_app add command . profile greet
cmake --build build
./build/mybot
```

Generated feature code lives under `src/` and is wired into the application for
you. Handlers use typed argument structs, so normal command code does not need
to parse raw interaction options.

Use the generator only for features you need. The full command catalog is in
[CLI Tools](reference/cli.md); route, guard, component, task, and store behavior
is explained in [App Runtime](guides/app-runtime.md).

## Generated Project Map

| Path | Purpose |
| --- | --- |
| `src/main.c` | Process entry point and bot definition |
| `src/config.h` | Typed application configuration |
| `src/ping.c` | Initial feature and `/ping` handler |
| `src/<feature>.c` | Generated routes, events, and tasks for one feature |
| `.env.example` | Safe configuration template |
| `.env` | Local secrets and runtime configuration; never commit it |
| `CMakeLists.txt` | DCC package discovery and application build |

The generated bot uses `DCC_DEV_BOT(...)` with explicit development defaults. It infers the
application ID from Gateway READY, reconciles commands, and makes the app-owned
store available through handler context.

Before production, switch to `DCC_BOT(...)` and deploy commands explicitly with
`dcc_command_sync --plan` followed by `--apply`. Production bot aliases do not
modify Discord commands during READY.

## Minimal One-File App

You can also write a bot without the generator:

```c
#include <dcc/sugar.h>

DCC_SLASH_FN(ping) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_DEV_BOT_MAIN(
    "minimal",
    DCC_APP_LISTENERS(DCC_LISTEN_SLASH("ping", "Reply with pong", ping))
)
```

`DCC_DEV_BOT_MAIN` performs automatic development command sync. For production,
use `DCC_BOT_MAIN` and deploy commands explicitly. The generated feature layout
also scales better than a single translation unit.

## Command Sync Without The App Default

Use `dcc_command_sync` when deployment needs an explicit plan or check:

```sh
set -a
. ./.env
set +a

dcc_command_sync \
  --commands commands.json \
  --application-id "$DCC_APPLICATION_ID" \
  --plan
```

See [Slash Command Registry](guides/command-registry.md) before applying command
deletions or moving from guild to global scope.

## Build DCC From Source

This path is for working on DCC itself. Requirements are:

- CMake 3.20 or newer;
- a C11 compiler;
- OpenSSL development headers;
- LLAM 2.2.0 or newer;
- optional libopus development headers for Opus voice encode/decode.

Linux also needs `liburing`. macOS and BSD use LLAM's kqueue backend.

With DCC and LLAM checked out side by side:

```sh
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DDCC_BUILD_EXAMPLES=ON \
  -DDCC_BUILD_TESTS=ON \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ROOT=../LLAM
cmake --build build
ctest --test-dir build --output-on-failure
```

DCC disables LLAM install rules and LLAM tests in this combined build by
default. See [Build Options](reference/build-options.md) for packaging,
sanitizer, fuzzer, shared-library, and LLAM test controls.

## Include Style

Application code should normally begin with:

```c
#include <dcc/sugar.h>
```

Use `<dcc/app.h>` for explicit app lifecycle and registration. Use focused
headers in low-level code:

```c
#include <dcc/client.h>
#include <dcc/events/listeners.h>
#include <dcc/rest/messages.h>
```

`<dcc/dcc.h>` aggregates the public API without Sugar helpers. Headers under
`src/internal/` are implementation details and are not installed API.

## Where To Go Next

- [Choose An API Layer](concepts/api-layers.md) explains Sugar, App, and Core.
- [Ownership And Async](concepts/ownership-and-async.md) covers callback and
  cache lifetimes.
- [Sugar-First API](guides/sugar.md) is the complete application helper guide.
- [Components v2](guides/components-v2.md) covers modern Discord layouts.
- [Production Playbooks](production-playbooks.md) covers deployment and health.
