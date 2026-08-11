#ifndef DCC_INTERACTION_FLOW_INTERNAL_H
#define DCC_INTERACTION_FLOW_INTERNAL_H

#include <dcc/interaction_flow.h>
#include <dcc/rest.h>

#include "internal/rest/dcc_rest_endpoint_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_INTERACTION_FLOW_FLAG_EPHEMERAL UINT64_C(64)
#define DCC_INTERACTION_FLOW_RESPONSE_INITIAL_ADMITTED UINT64_C(1)
#define DCC_INTERACTION_FLOW_RESPONSE_INITIAL_CLAIMED UINT64_C(2)

uint8_t dcc_flow_initial_sent(const dcc_interaction_flow_t *flow);
uint8_t dcc_flow_initial_claimed(const dcc_interaction_flow_t *flow);
dcc_status_t dcc_flow_claim_initial(dcc_interaction_flow_t *flow);
dcc_status_t dcc_flow_mark_initial(
    dcc_interaction_flow_t *flow,
    dcc_interaction_flow_state_t state,
    dcc_status_t status
);
dcc_status_t dcc_flow_require_ready(dcc_interaction_flow_t *flow);
dcc_status_t dcc_flow_require_context(const dcc_interaction_flow_t *flow);
void dcc_flow_mark(dcc_interaction_flow_t *flow, dcc_interaction_flow_state_t state, dcc_status_t status);

#ifdef __cplusplus
}
#endif

#endif
