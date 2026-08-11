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
        dcc_flow_initial_sent(flow) &&
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
        dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
        dcc_rest_message_payload_init(&payload, message);
        dcc_rest_call_options_t options;
        void *bridge = NULL;
        status = dcc_endpoint_legacy_options(cb, user_data, &options, &bridge);
        if (status == DCC_OK) status = dcc_rest_interaction_original_response_edit(
            flow->client, flow->interaction->application_id,
            flow->interaction->token, &payload, &options, NULL);
        if (status != DCC_OK) dcc_endpoint_legacy_bridge_release(bridge);
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
        dcc_rest_message_payload_t payload = DCC_REST_MESSAGE_PAYLOAD_INIT;
        dcc_rest_message_payload_init(&payload, message);
        dcc_rest_call_options_t options;
        void *bridge = NULL;
        status = dcc_endpoint_legacy_options(cb, user_data, &options, &bridge);
        if (status == DCC_OK) status = dcc_rest_interaction_followup_create(
            flow->client, flow->interaction->application_id,
            flow->interaction->token, &payload, &options, NULL);
        if (status != DCC_OK) dcc_endpoint_legacy_bridge_release(bridge);
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

    if (!dcc_flow_initial_sent(flow)) {
        status = dcc_flow_claim_initial(flow);
        if (status != DCC_OK) {
            return status;
        }
        dcc_rest_interaction_response_t response = DCC_REST_INTERACTION_RESPONSE_INIT;
        status = dcc_rest_interaction_response_set_message(&response, message);
        dcc_rest_call_options_t options;
        void *bridge = NULL;
        if (status == DCC_OK) status = dcc_endpoint_legacy_options(
            cb, user_data, &options, &bridge);
        if (status == DCC_OK) status = dcc_rest_interaction_response_create(
            flow->client, flow->interaction->id, flow->interaction->token,
            &response, &options, NULL);
        if (status != DCC_OK) dcc_endpoint_legacy_bridge_release(bridge);
        return dcc_flow_mark_initial(
            flow,
            DCC_INTERACTION_FLOW_REPLIED,
            status
        );
    }
    if (flow->state == DCC_INTERACTION_FLOW_DEFERRED ||
        flow->state == DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL ||
        flow->state == DCC_INTERACTION_FLOW_DEFERRED_UPDATE) {
        return dcc_flow_edit_original(flow, message, cb, user_data);
    }
    return dcc_flow_followup(flow, message, cb, user_data);
}
