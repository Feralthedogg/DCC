# Component Sessions

Component sessions turn buttons, selects, and modals into signed stateful UI
flows. Application code should not hand-build `custom_id` strings.

## Custom ID Format

```text
dcc:v1:<session_id>:<action>:<nonce>:<sig>
```

`sig` authenticates the session id, action, nonce, and session secret. A forged
or edited component id fails verification before user code handles it.

## Create A Session

```c
static const unsigned char secret[] = "rotate-this-secret";

dcc_component_session_options_t options =
    DCC_COMPONENT_SESSION_OPTIONS_USER(secret, sizeof(secret) - 1, interaction_user_id);
options.now_ms = now_ms;
options.ttl_ms = 300000U;
options.channel_id = channel_id;
options.guild_id = guild_id;
options.lock_channel = 1U;
options.lock_guild = 1U;

dcc_component_session_t session;
dcc_component_session_create(&options, &session);
```

Build a signed legacy button. The button itself is created by the session API
because DCC must sign the `custom_id`:

```c
dcc_component_builder_t next;
dcc_component_session_button(
    &session,
    "next",
    "Next",
    DCC_BUTTON_PRIMARY,
    &next
);
```

For Components v2 layouts, build the signed button directly as a v2 component:

```c
dcc_component_v2_builder_t next_v2;
dcc_component_session_button_v2(
    &session,
    "next",
    "Next",
    DCC_BUTTON_PRIMARY,
    &next_v2
);

dcc_message_builder_t page =
    DCC_MESSAGE_COMPONENTS_V2(
        DCC_V2_ACTION_ROW(next_v2)
    );
```

## Route Session Actions

```c
static void on_action(
    dcc_client_t *client,
    const dcc_event_t *event,
    const dcc_component_session_result_t *result,
    void *data
) {
    if (result->status == DCC_COMPONENT_SESSION_VERIFY_EXPIRED) {
        /* reply with an expired UI message */
        return;
    }
    if (result->status != DCC_COMPONENT_SESSION_VERIFY_OK) {
        return;
    }
    if (strcmp(result->action, "next") == 0) {
        /* advance page */
    }
}

dcc_component_session_listener_t listener;
dcc_component_session_listener_options_t listen =
    DCC_COMPONENT_SESSION_LISTENER_OPTIONS();
dcc_client_on_component_session(client, &session, &listen, on_action, state, &listener);
```

Stop routing:

```c
dcc_client_off_component_session(client, &listener);
```

## Store Multiple Sessions

Use `dcc_component_session_store_t` when a bot owns many active UIs:

```c
dcc_component_session_store_t store;
dcc_component_session_store_init(&store);
dcc_component_session_store_add(&store, &session);
dcc_client_on_component_session_store(client, &store, NULL, on_action, state, &listener);
```

Sweep expired sessions periodically:

```c
dcc_component_session_store_sweep(&store, now_ms);
```
