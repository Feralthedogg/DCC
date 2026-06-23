# Gateway And Events

DCC keeps Gateway ownership inside the app runtime by default. Application code
usually registers typed listener handlers through `<dcc/sugar.h>` and lets the
runtime own the underlying `dcc_client_t`.

## Register A Listener

Use `DCC_READY_FN`, `DCC_MESSAGE_CREATE_FN`, and the matching `DCC_LISTEN_*`
entry in a `DCC_SIMPLE_BOT_MAIN()` or feature definition:

```c
#include <dcc/sugar.h>

#include <stdio.h>
#include <string.h>

DCC_READY_FN(on_ready) {
    (void)app;
    (void)user_data;
    printf(
        "ready shard=%u/%u session=%s\n",
        ready->shard_id,
        ready->shard_count,
        ready->session_id
    );
}

DCC_MESSAGE_CREATE_FN(on_message) {
    (void)event;
    (void)user_data;
    if (message->content != NULL && strcmp(message->content, "!ping") == 0) {
        (void)DCC_APP_SEND_TEXT(app, message->channel_id, "pong");
    }
}

DCC_SIMPLE_BOT_MAIN(
    "gateway-events",
    DCC_LISTEN_READY_ONCE(on_ready),
    DCC_LISTEN_MESSAGE_CREATE(on_message)
)
```

Typed event views are borrowed for the callback. Clone values that need to live
after the handler returns.

When you are writing a low-level runtime, register directly on `dcc_client_t`:

```c
static void on_raw_ready(dcc_client_t *client, const dcc_event_t *event, void *data) {
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    (void)client;
    (void)data;
    if (ready != NULL) {
        /* ready->session_id is borrowed for this callback. */
    }
}

dcc_listener_id_t id = 0;
dcc_client_on(client, DCC_EVENT_READY, on_raw_ready, user_data, &id);
dcc_client_off(client, DCC_EVENT_READY, id);
```

## Interaction Subtypes

`INTERACTION_CREATE` is normalized into subtype events:

- `DCC_EVENT_SLASH_COMMAND`
- `DCC_EVENT_AUTOCOMPLETE`
- `DCC_EVENT_BUTTON_CLICK`
- `DCC_EVENT_SELECT_CLICK`
- `DCC_EVENT_FORM_SUBMIT`
- `DCC_EVENT_USER_CONTEXT_MENU`
- `DCC_EVENT_MESSAGE_CONTEXT_MENU`

Use `dcc_event_interaction(event)` for the borrowed interaction view.

## Wait Helpers

Use event waits when supervisor code needs deterministic admission:

```c
dcc_event_wait_result_t result;
dcc_client_wait_for_gateway_ready_or_resumed(client, 30000, &result);
```

Owned wait helpers clone the event payload when it must outlive the callback.

## Cache

When enabled, Gateway dispatch updates the client-owned cache for guilds,
channels, roles, members, users, messages, and voice states. Cache accessors
return DCC-owned data; clone values that need independent lifetime.
