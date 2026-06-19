#include "internal/interaction_flow/dcc_interaction_flow_internal.h"

dcc_status_t dcc_flow_auto_defer(dcc_interaction_flow_t *flow, uint64_t after_ms) {
    if (flow == NULL || after_ms == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    flow->auto_defer_after_ms = after_ms;
    flow->auto_defer_ephemeral = 0U;
    return DCC_OK;
}

dcc_status_t dcc_flow_auto_defer_ephemeral(dcc_interaction_flow_t *flow, uint64_t after_ms) {
    if (flow == NULL || after_ms == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    flow->auto_defer_after_ms = after_ms;
    flow->auto_defer_ephemeral = 1U;
    return DCC_OK;
}

dcc_status_t dcc_flow_maybe_auto_defer(
    dcc_interaction_flow_t *flow,
    uint64_t now_ms,
    dcc_rest_cb cb,
    void *user_data
) {
    if (flow == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_flow_initial_sent(flow) || flow->auto_defer_after_ms == 0U ||
        now_ms < flow->started_at_ms ||
        now_ms - flow->started_at_ms < flow->auto_defer_after_ms) {
        return DCC_OK;
    }
    return flow->auto_defer_ephemeral
        ? dcc_flow_defer_ephemeral(flow, cb, user_data)
        : dcc_flow_defer(flow, cb, user_data);
}
