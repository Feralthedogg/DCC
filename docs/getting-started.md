# Quickstart

This page gets a local DCC build running with the neighboring LLAM runtime.

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
  -DDCC_LLAM_ROOT=../LLAM \
  -DDCC_LLAM_LIBRARY=../LLAM/libllam_runtime.a

cmake --build build
ctest --test-dir build --output-on-failure
```

For CI or clean clones where LLAM has not already produced
`libllam_runtime.a`, build LLAM as a subdirectory:

```sh
cmake -S . -B build \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ROOT=../LLAM
cmake --build build
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
