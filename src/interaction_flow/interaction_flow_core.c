#include "internal/interaction_flow/dcc_interaction_flow_internal.h"

#include <string.h>

void dcc_flow_init(
    dcc_interaction_flow_t *flow,
    dcc_client_t *client,
    const dcc_interaction_t *interaction
) {
    if (flow != NULL) {
        memset(flow, 0, sizeof(*flow));
        flow->size = sizeof(*flow);
        flow->client = client;
        flow->interaction = interaction;
        flow->state = DCC_INTERACTION_FLOW_READY;
    }
}

dcc_status_t dcc_flow_set_started_at(dcc_interaction_flow_t *flow, uint64_t started_at_ms) {
    if (flow == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    flow->started_at_ms = started_at_ms;
    return DCC_OK;
}

dcc_interaction_flow_state_t dcc_flow_state(const dcc_interaction_flow_t *flow) {
    return flow != NULL ? flow->state : DCC_INTERACTION_FLOW_FAILED;
}

uint8_t dcc_flow_initial_sent(const dcc_interaction_flow_t *flow) {
    return flow != NULL && flow->state != DCC_INTERACTION_FLOW_READY;
}

dcc_status_t dcc_flow_require_ready(dcc_interaction_flow_t *flow) {
    if (flow == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_flow_initial_sent(flow) ? DCC_ERR_STATE : DCC_OK;
}

dcc_status_t dcc_flow_require_context(const dcc_interaction_flow_t *flow) {
    if (flow == NULL || flow->client == NULL || flow->interaction == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

void dcc_flow_mark(dcc_interaction_flow_t *flow, dcc_interaction_flow_state_t state, dcc_status_t status) {
    if (flow != NULL) {
        flow->state = status == DCC_OK ? state : DCC_INTERACTION_FLOW_FAILED;
    }
}

const char *dcc_flow_state_string(dcc_interaction_flow_state_t state) {
    switch (state) {
        case DCC_INTERACTION_FLOW_READY:
            return "ready";
        case DCC_INTERACTION_FLOW_DEFERRED:
            return "deferred";
        case DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL:
            return "deferred_ephemeral";
        case DCC_INTERACTION_FLOW_DEFERRED_UPDATE:
            return "deferred_update";
        case DCC_INTERACTION_FLOW_REPLIED:
            return "replied";
        case DCC_INTERACTION_FLOW_ORIGINAL_EDITED:
            return "original_edited";
        case DCC_INTERACTION_FLOW_FOLLOWED_UP:
            return "followed_up";
        case DCC_INTERACTION_FLOW_MODAL:
            return "modal";
        case DCC_INTERACTION_FLOW_FAILED:
            return "failed";
        default:
            return "unknown";
    }
}
