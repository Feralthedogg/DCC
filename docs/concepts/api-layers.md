# Choose An API Layer

DCC exposes three layers over the same object model. Pick the highest layer
that gives you the control you need. Moving lower increases lifecycle,
ownership, and error-handling responsibilities.

## Decision Table

| If you are building... | Primary layer | Entry point |
| --- | --- | --- |
| A slash-command or component bot | Sugar | `<dcc/sugar.h>` |
| A modular bot with explicit setup and teardown | App | `<dcc/app.h>` |
| A framework, embedded runtime, or custom dispatcher | Core | `<dcc/client.h>` plus focused headers |
| A webhook-only interaction service | Interaction Server | `<dcc/interaction_server.h>` |
| A voice service | App/Core plus Voice | `<dcc/voice.h>` |

## Sugar Layer

Use Sugar for normal application code. It provides declarative bot definitions,
typed command/component routes, reply helpers, guards, scheduled tasks, and
builder literals.

```c
#include <dcc/sugar.h>

DCC_SLASH_FN(on_ping) {
    (void)user_data;
    (void)DCC_REPLY_TEXT(ctx, "pong");
}

DCC_SIMPLE_BOT_MAIN(
    "minimal",
    DCC_LISTEN_SLASH("ping", "Reply with pong", on_ping)
)
```

Start here when you do not need to own `dcc_client_t` directly. The macros
expand to public structs and functions; they do not create a separate runtime.

## App Layer

Use App when setup needs branches, dynamically registered routes, shared
application state, or explicit shutdown.

```c
#include <dcc/app.h>

dcc_app_options_t options;
dcc_app_options_init(&options);
options.client.token = token;

dcc_app_t *app = NULL;
dcc_status_t status = dcc_app_create(&options, &app);
if (status == DCC_OK) {
    status = dcc_app_run(app);
}
dcc_app_destroy(app);
```

The App object owns its client, command registry, route tables, store,
component sessions, and scheduled tasks. Prefer `dcc_app_client(app)` when an
App handler needs a focused REST or Gateway helper.

## Core Layer

Use Core when another framework owns the process lifecycle or when you need
direct client, event, REST, cache, cluster, or Gateway control.

Include focused headers rather than the full aggregate:

```c
#include <dcc/client.h>
#include <dcc/events/listeners.h>
#include <dcc/rest/messages.h>
```

At this layer your code is responsible for creating and destroying runtime
objects, checking every `dcc_status_t`, selecting intents, and respecting all
borrowed-pointer lifetimes.

## Task-To-Guide Map

| Task | Guide or reference |
| --- | --- |
| Create commands and typed handlers | [Sugar-First API](../guides/sugar.md) |
| Register routes and control app startup | [App Runtime](../guides/app-runtime.md) |
| Receive Gateway events | [Gateway And Events](../guides/gateway-events.md) |
| Reconcile application commands | [Slash Command Registry](../guides/command-registry.md) |
| Build buttons, selects, and modals | [Component Sessions](../guides/component-sessions.md) |
| Use Discord REST endpoints | [Public API Guide](../api.md) |
| Run modules without dropping Gateway | [Hot Reload](../guides/hot-reload.md) |
| Prepare a production deployment | [Production Playbooks](../production-playbooks.md) |

## Mixing Layers Safely

- Keep one owner for each runtime object.
- Do not create a second `dcc_client_t` inside an App handler.
- Passing public builder/value structs between layers is safe while their
  referenced storage remains valid.
- Clone borrowed event or cache objects before storing them.
- Keep low-level operations behind small application functions so most feature
  code remains on Sugar or App.

If the reason for moving lower is only “I cannot find the helper,” check the
[Public API Guide](../api.md) and [Official Discord Surface](../reference/official-api-surface.md)
before taking over lifecycle management.
