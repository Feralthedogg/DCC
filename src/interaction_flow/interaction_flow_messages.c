#include "internal/interaction_flow/dcc_interaction_flow_internal.h"

static uint8_t dcc_flow_can_edit_original(const dcc_interaction_flow_t *flow) {
    return flow != NULL &&
        (flow->state == DCC_INTERACTION_FLOW_DEFERRED ||
         flow->state == DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
         flow->state == DCC_INTERACTION_FLOW_DEFERRED_UPDATE ||
         flow->state == DCC_INTERACTION_FLOW_REPLIED ||
         flow->state == DCC_INTERACTION_FLOW_ORIGINAL_EDITED);
}

static uint8_t dcc_flow_can_followup(const dcc_interaction_flow_t *flow) {
    return flow != NULL &&
        flow->state != DCC_INTERACTION_FLOW_READY &&
        flow->state != DCC_INTERACTION_FLOW_FAILED;
}

dcc_status_t dcc_flow_edit_original(
    dcc_interaction_flow_t *flow,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_status_t status = dcc_flow_require_context(flow);
    if (status == DCC_OK && (message == NULL || !dcc_flow_can_edit_original(flow))) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_rest_interaction_original_response_edit_from_interaction_builder(
            flow->client,
            flow->interaction,
            message,
            cb,
            user_data
        );
    }
    dcc_flow_mark(flow, DCC_INTERACTION_FLOW_ORIGINAL_EDITED, status);
    return status;
}

dcc_status_t dcc_flow_followup(
    dcc_interaction_flow_t *flow,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_status_t status = dcc_flow_require_context(flow);
    if (status == DCC_OK && (message == NULL || !dcc_flow_can_followup(flow))) {
        status = DCC_ERR_STATE;
    }
    if (status == DCC_OK) {
        status = dcc_rest_interaction_followup_create_from_interaction_builder(
            flow->client,
            flow->interaction,
            message,
            cb,
            user_data
        );
    }
    dcc_flow_mark(flow, DCC_INTERACTION_FLOW_FOLLOWED_UP, status);
    return status;
}

dcc_status_t dcc_flow_reply(
    dcc_interaction_flow_t *flow,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_status_t status = dcc_flow_require_context(flow);
    if (status != DCC_OK || message == NULL) {
        if (status == DCC_OK) {
            status = DCC_ERR_INVALID_ARG;
        }
        dcc_flow_mark(flow, DCC_INTERACTION_FLOW_FAILED, status);
        return status;
    }

    if (flow->state == DCC_INTERACTION_FLOW_READY) {
        status = dcc_rest_interaction_response_create_from_interaction_message_builder(
            flow->client,
            flow->interaction,
            DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE,
            message,
            cb,
            user_data
        );
        dcc_flow_mark(flow, DCC_INTERACTION_FLOW_REPLIED, status);
        return status;
    }
    if (flow->state == DCC_INTERACTION_FLOW_DEFERRED ||
        flow->state == DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
        flow->state == DCC_INTERACTION_FLOW_DEFERRED_UPDATE) {
        return dcc_flow_edit_original(flow, message, cb, user_data);
    }
    return dcc_flow_followup(flow, message, cb, user_data);
}
