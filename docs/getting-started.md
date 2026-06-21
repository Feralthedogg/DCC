# Getting Started

The normal path is to install a published DCC release. The release archive is
self-contained and includes a compatible LLAM runtime; the install script also
refreshes LLAM through LLAM's latest release installer.

## Install Release

```sh
curl -fsSL https://raw.githubusercontent.com/Feralthedogg/DCC/main/tools/install.sh |
  sh -s -- --prefix "$HOME/.local"
```

For system-wide installs use a system prefix:

```sh
curl -fsSL https://raw.githubusercontent.com/Feralthedogg/DCC/main/tools/install.sh |
  sudo sh -s -- --prefix /usr/local
```

Upgrade an existing prefix with:

```sh
curl -fsSL https://raw.githubusercontent.com/Feralthedogg/DCC/main/tools/install.sh |
  sh -s -- --prefix "$HOME/.local" --force
```

Use `--skip-llam` only when you want the LLAM copy bundled in the DCC archive
to remain untouched. By default the script installs DCC and then fetches the
latest LLAM release installer into the same prefix.

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

## Minimal Client

```c
#include <dcc/sugar.h>

#include <stdlib.h>

int main(void) {
    dcc_client_options_t options =
        DCC_CLIENT_OPTIONS(getenv("DISCORD_TOKEN"), DCC_INTENTS_DEFAULT);

    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK) {
        return 1;
    }

    dcc_status_t status = dcc_client_start(client);
    if (status == DCC_OK) {
        status = dcc_client_wait(client);
    }

    dcc_client_destroy(client);
    return status == DCC_OK ? 0 : 1;
}
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
        DCC_EMBED_COLOR("Latency", "Gateway online", 0x5865F2)
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
