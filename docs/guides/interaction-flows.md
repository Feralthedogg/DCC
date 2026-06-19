# Interaction Flows

Discord interactions must receive an initial response quickly. DCC's
interaction flow helpers keep the common slash command sequence compact:
defer, edit original response, send followups, show modals, and wait for
components.

## Auto Defer

Use auto-defer when a handler might run close to Discord's response deadline.

```c
dcc_interaction_flow_t flow;
dcc_interaction_flow_init(&flow, client, interaction);
dcc_flow_auto_defer(&flow, 1500);
```

If the handler has not sent a response after the threshold, DCC sends a
deferred response.

## Reply And Followup

```c
dcc_message_builder_t message;
dcc_message_builder_init(&message);
dcc_message_builder_set_content(&message, "Working...");

dcc_flow_reply(&flow, &message);
dcc_flow_followup(&flow, &message);
```

Use ephemeral responses for user-specific state:

```c
dcc_flow_defer_ephemeral(&flow);
```

## Components And Modals

Combine interaction flows with component sessions when a command produces a UI:

```c
dcc_flow_reply(&flow, &message_with_buttons);
dcc_client_on_component_session(client, &session, NULL, on_action, state, &listener);
```

For forms, build a modal and show it as the interaction response:

```c
dcc_modal_builder_t modal;
dcc_modal_builder_init(&modal, "settings", "Settings");
dcc_flow_show_modal(&flow, &modal);
```
