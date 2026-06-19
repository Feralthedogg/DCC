# Runtime Modules

Hot reload modules are shared libraries loaded by DCC. The parent process owns
Discord state; the module owns handler logic and optional module state.

## Minimal Module

```c
#include <dcc/dcc.h>

static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *data) {
    (void)client;
    (void)event;
    (void)data;
}

static const dcc_bot_module_event_t events[] = {
    { DCC_EVENT_READY, on_ready },
};

DCC_BOT_MODULE_EXPORT int dcc_bot_module_init(dcc_bot_module_context_t *ctx) {
    ctx->events = events;
    ctx->events_count = sizeof(events) / sizeof(events[0]);
    return 0;
}
```

## State Macro

Use descriptor macros when a module needs DCC-preserved state across reloads.
The host keeps the state block; the module version only supplies callbacks.

See `examples/hot_reload/bot.c` for the current full example.

## CMake Consumer Helper

Installed DCC packages expose:

```cmake
find_package(dcc CONFIG REQUIRED)

dcc_add_hot_reload_module(my_bot
    SOURCES bot.c
)
```

The helper builds a `MODULE` target with DCC's module naming and link settings.

## Isolation Rules

In isolated worker mode:

- do not own Gateway sockets in the module
- do not keep raw event pointers after a callback
- build response payloads and return them through the worker protocol
- treat process exit as a failed request

The parent may retry failed interactions against last-good worker once, then
respond with an ephemeral temporary error if recovery is not possible.
