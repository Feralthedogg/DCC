#include "internal/interaction_flow/dcc_interaction_flow_internal.h"

dcc_status_t dcc_flow_show_modal(
    dcc_interaction_flow_t *flow,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_status_t status = dcc_flow_require_context(flow);
    if (status == DCC_OK) {
        status = dcc_flow_require_ready(flow);
    }
    if (status == DCC_OK && modal == NULL) {
        status = DCC_ERR_INVALID_ARG;
    }
    if (status == DCC_OK) {
        status = dcc_rest_interaction_response_create_modal_from_interaction(
            flow->client,
            flow->interaction,
            modal,
            cb,
            user_data
        );
    }
    dcc_flow_mark(flow, DCC_INTERACTION_FLOW_MODAL, status);
    return status;
}
