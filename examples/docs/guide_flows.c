/* DCC_DOC_SNIPPET_BEGIN(interaction-flows) */
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
/* DCC_DOC_SNIPPET_END(interaction-flows) */
