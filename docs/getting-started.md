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
#include <dcc/dcc.h>

#include <stdlib.h>

int main(void) {
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = getenv("DISCORD_TOKEN"),
        .intents = DCC_INTENT_GUILDS,
    };

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

Use the aggregate header while prototyping:

```c
#include <dcc/dcc.h>
```

Use focused headers in library code:

```c
#include <dcc/client.h>
#include <dcc/events/listeners.h>
#include <dcc/rest/application_commands.h>
```

Public headers live under `include/dcc/`. Internal headers under
`src/internal/` are not part of the package API.
