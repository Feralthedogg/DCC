# Gateway And Events

DCC keeps Gateway ownership inside `dcc_client_t`. Events are delivered through
typed listeners, and event accessors return borrowed callback views.

## Register A Listener

```c
static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *data) {
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready != NULL) {
        /* ready->session_id is borrowed for this callback. */
    }
    (void)client;
    (void)data;
}

dcc_listener_id_t id = 0;
dcc_client_on(client, DCC_EVENT_READY, on_ready, user_data, &id);
```

Remove listeners with the returned id:

```c
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
