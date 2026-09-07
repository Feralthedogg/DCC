# Runtime Modules

Hot-reload modules are shared libraries loaded by DCC. The host owns Discord
state; modules supply handler logic and optional state.

Include `<dcc/hot_reload.h>` for the module API, or `<dcc/dcc.h>` when the
module also needs other subsystems. Descriptor and state macros live in the
[module declarations](api/hot_reload/module/macros.md). Use the maintained
[full example](https://github.com/Feralthedogg/DCC/blob/main/examples/hot_reload/bot.c)
for a complete handler/descriptor implementation.

## Installed CMake helper

```cmake
find_package(dcc CONFIG REQUIRED)
dcc_add_hot_reload_module(my_bot SOURCES bot.c)
```

The helper creates a `MODULE` target with DCC's naming and link settings.
State lifetime and descriptor compatibility are specified in
[module descriptors](api/hot_reload/module/descriptor.md) and
[module context](api/hot_reload/module/context.md).

## Isolated workers

The parent owns Gateway sockets. Do not retain raw event pointers after a
callback. Build responses through the supported worker protocol, and treat a
worker exit as a failed request rather than successful completion.

Read the [hot-reload guide](../guides/hot-reload.md) and
[production playbooks](../production-playbooks.md) for health checks,
last-good recovery, and rollout policy.
