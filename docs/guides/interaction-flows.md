# Interaction Flows

Interaction Flows coordinate initial replies, defers, edits, followups and
modals. `dcc_interaction_flow_t` is opaque: create a pointer owner with
`dcc_flow_create` and release it with `dcc_flow_destroy`, never stack-allocate it.

## Defer, Edit, And Teardown

This source-owned example compiles against installed headers in
`examples/docs/guide_flows.c`. The function parameters define its handler inputs.

<!-- DCC_DOC_SNIPPET_BEGIN(interaction-flows) -->
```c
#include <dcc/interaction_flow.h>

/* Inputs are borrowed from an interaction handler. The client runtime must
 * keep running to complete accepted REST work. */
dcc_status_t dcc_example_flow(dcc_client_t *client,
                              const dcc_interaction_t *interaction) {
    dcc_interaction_flow_t *flow = NULL;
    dcc_status_t status = dcc_flow_create(client, interaction, &flow);
    if (status != DCC_OK) return status;
    status = dcc_flow_auto_defer(flow, 1500U);

    /* This example knows work will take time, so defer immediately.
     * Auto-defer alone is a policy, not a background timer: an owner using
     * it must call dcc_flow_maybe_auto_defer(flow, now_ms, NULL, NULL). */
    if (status == DCC_OK) status = dcc_flow_defer_ex(flow, NULL, NULL);
    if (status == DCC_OK) {
        /* Perform application work here, without blocking the runtime. */
        dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
        status = dcc_message_builder_set_content(&message, "Work finished");
        if (status == DCC_OK)
            status = dcc_flow_edit_original_ex(flow, &message, NULL, NULL);
    }
    dcc_flow_destroy(flow); /* Accepted work survives the Flow owner. */
    return status;
}
```
<!-- DCC_DOC_SNIPPET_END(interaction-flows) -->

For lower-level auto-defer, set the interaction's start time with
`dcc_flow_set_started_at` using the same millisecond clock as your periodic
`dcc_flow_maybe_auto_defer` calls. Merely configuring the threshold does not
schedule a timer or let a blocked runtime send the response. Use
`dcc_flow_auto_defer_ephemeral` / `dcc_flow_defer_ephemeral` for private replies.

## Reply And Request Ownership

For an immediate initial reply, use `dcc_flow_reply_ex`; after an initial reply
or defer, use `dcc_flow_edit_original_ex` or `dcc_flow_followup_ex`. Pass the
Flow pointer itself, not its address.

The `_ex` calls return admission status, not HTTP completion. A non-NULL
`out_request` transfers one handle to the caller, which must eventually call
`dcc_rest_request_destroy`; NULL output auto-releases the handle. Accepted work
survives Flow destruction and copies builder inputs before admission returns.
Rejected admission produces no callback or handle. Keep callback user data alive
through completion, and never block the runtime/callback thread in a request wait.
See the complete [request ownership example](../concepts/ownership-and-async.md).

## Components And Modals

Use the installed builders and explicit arrays shown in
[Component Sessions](component-sessions.md). Builders borrow their arrays and
strings, so retain all nested storage until the submit call returns.

For forms, initialize a `dcc_modal_builder_t` with `DCC_MODAL_BUILDER_INIT`,
set its custom id/title and Components v2 array, then call `dcc_flow_show_modal`
while the Flow is ready for its initial response. A modal is an alternative to
reply/defer, not a response that can follow a defer. See the installed
[modal builder API](../reference/api/modal.md) and
[Components v2 API](../reference/api/component_v2.md).
