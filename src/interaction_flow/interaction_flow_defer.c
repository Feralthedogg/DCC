#include "internal/interaction_flow/dcc_interaction_flow_internal.h"

dcc_status_t dcc_flow_defer(
    dcc_interaction_flow_t *flow,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_status_t status = dcc_flow_require_context(flow);
    if (status == DCC_OK) {
        status = dcc_flow_require_ready(flow);
    }
    if (status == DCC_OK) {
        status = dcc_rest_interaction_response_create_deferred_message_from_interaction(
            flow->client,
            flow->interaction,
            cb,
            user_data
        );
    }
    dcc_flow_mark(flow, DCC_INTERACTION_FLOW_DEFERRED, status);
    return status;
}

dcc_status_t dcc_flow_defer_ephemeral(
    dcc_interaction_flow_t *flow,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_status_t status = dcc_flow_require_context(flow);
    if (status == DCC_OK) {
        status = dcc_flow_require_ready(flow);
    }
    dcc_message_builder_t message;
    if (status == DCC_OK) {
        dcc_message_builder_init(&message);
        status = dcc_message_builder_set_flags(&message, DCC_INTERACTION_FLOW_FLAG_EPHEMERAL);
    }
    if (status == DCC_OK) {
        status = dcc_rest_interaction_response_create_from_interaction_message_builder(
            flow->client,
            flow->interaction,
            DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE,
            &message,
            cb,
            user_data
        );
    }
    dcc_flow_mark(flow, DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL, status);
    return status;
}

dcc_status_t dcc_flow_defer_update(
    dcc_interaction_flow_t *flow,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_status_t status = dcc_flow_require_context(flow);
    if (status == DCC_OK) {
        status = dcc_flow_require_ready(flow);
    }
    if (status == DCC_OK) {
        status = dcc_rest_interaction_response_create_deferred_update_from_interaction(
            flow->client,
            flow->interaction,
            cb,
            user_data
        );
    }
    dcc_flow_mark(flow, DCC_INTERACTION_FLOW_DEFERRED_UPDATE, status);
    return status;
}
