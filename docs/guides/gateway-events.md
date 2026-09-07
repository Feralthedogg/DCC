# Gateway and Events

The App runtime owns Gateway connections by default. Application code includes
`<dcc/bot.h>` and registers typed, status-returning listeners.

```c
#include <dcc/bot.h>
#include <stdio.h>

DCC_READY_FN(on_ready) {
    (void)app;
    (void)user_data;
    printf("ready shard=%u/%u\n", ready->shard_id, ready->shard_count);
    return DCC_OK;
}

DCC_BOT_MAIN(DCC_LISTEN_READY(on_ready))
```

Use `DCC_MESSAGE_FN` with `DCC_LISTEN_MESSAGE_CREATE` for messages.
Once behavior, middleware, cleanup, and user data belong in
`dcc_listener_config_t`, passed through the matching `_WITH` constructor.
See [handler declarations](../reference/api/bot/handlers.md) and
[listener constructors](../reference/api/bot/listeners.md).

Event/accessor results are borrowed for the callback unless explicitly
documented as owned. Clone the supported object type before retaining it;
never keep pointers into the event after the callback. Read
[ownership and async](../concepts/ownership-and-async.md).

## Low-level integration

Focused `<dcc/client.h>` and `<dcc/events.h>` headers expose raw client
listeners. Register with `dcc_client_on`, check its status, retain the returned
listener ID, and unregister with `dcc_client_off`. The raw callback receives
a `dcc_client_t *`, `const dcc_event_t *`, and user data; it returns void,
unlike Bot handlers.

Use [typed event accessors](../reference/api/events/accessors.md), not unchecked
casts. The [client declarations](../reference/api/client.md) define the current
client control surface.

## Interaction subtypes

The runtime normalizes interaction dispatch into slash command, autocomplete,
button, select, form-submit, and user/message context-menu events. Use the
matching Bot listener for application routing, or `dcc_event_interaction`
for a borrowed low-level interaction view.

Use [replay testing](replay.md) for offline validation.
[Offline performance](../performance.md) describes Gateway parsing and dispatch
microbenchmarks; they do not measure network latency.
