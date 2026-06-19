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

dcc_component_session_options_t options;
dcc_component_session_options_init(&options);
dcc_component_session_options_set_secret(&options, secret, sizeof(secret) - 1);
dcc_component_session_options_set_ttl(&options, now_ms, 300000);
dcc_component_session_options_lock_user(&options, interaction_user_id);
dcc_component_session_options_lock_channel(&options, channel_id);
dcc_component_session_options_lock_guild(&options, guild_id);

dcc_component_session_t session;
dcc_component_session_create(&options, &session);
```

Build a button:

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
dcc_client_on_component_session(client, &session, NULL, on_action, state, &listener);
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
