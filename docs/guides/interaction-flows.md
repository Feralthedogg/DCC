# Interaction Flows

Discord interactions must receive an initial response quickly. DCC's
interaction flow helpers keep the common slash command sequence compact:
defer, edit original response, send followups, show modals, and wait for
components.

## Auto Defer

Use auto-defer when a handler might run close to Discord's response deadline.

```c
dcc_interaction_flow_t flow =
    DCC_FLOW_AUTO_DEFER(client, interaction, 1500U);
```

If the handler has not sent a response after the threshold, DCC sends a
deferred response.

## Reply And Followup

```c
dcc_message_builder_t message =
    DCC_MESSAGE_TEXT("Working...");

dcc_flow_reply(&flow, &message, NULL, NULL);
dcc_flow_followup(&flow, &message, NULL, NULL);
```

Use ephemeral responses for user-specific state:

```c
dcc_flow_defer_ephemeral(&flow, NULL, NULL);
```

## Components And Modals

Combine interaction flows with component sessions when a command produces a UI:

```c
dcc_component_v2_builder_t next;
dcc_component_session_button_v2(
    &session,
    "next",
    "Next",
    DCC_BUTTON_PRIMARY,
    &next
);

dcc_message_builder_t message_with_buttons =
    DCC_MESSAGE_COMPONENTS_V2(DCC_V2_ACTION_ROW(next));

dcc_flow_reply(&flow, &message_with_buttons, NULL, NULL);
dcc_component_session_listener_options_t listen =
    DCC_COMPONENT_SESSION_LISTENER_OPTIONS();
dcc_client_on_component_session(client, &session, &listen, on_action, state, &listener);
```

For forms, build a modal and show it as the interaction response:

```c
dcc_modal_builder_t modal =
    DCC_MODAL_V2_BUILDER(
        "settings",
        "Settings",
        DCC_V2_LABEL(
            "Display name",
            DCC_V2_TEXT_INPUT("profile.name", "Name", DCC_TEXT_INPUT_SHORT)
        )
    );
dcc_flow_show_modal(&flow, &modal, NULL, NULL);
```
