#ifndef DCC_INTERACTION_FLOW_INTERNAL_H
#define DCC_INTERACTION_FLOW_INTERNAL_H

#include <dcc/interaction_flow.h>
#include <dcc/rest.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DCC_INTERACTION_FLOW_FLAG_EPHEMERAL UINT64_C(64)

uint8_t dcc_flow_initial_sent(const dcc_interaction_flow_t *flow);
dcc_status_t dcc_flow_require_ready(dcc_interaction_flow_t *flow);
dcc_status_t dcc_flow_require_context(const dcc_interaction_flow_t *flow);
void dcc_flow_mark(dcc_interaction_flow_t *flow, dcc_interaction_flow_state_t state, dcc_status_t status);

#ifdef __cplusplus
}
#endif

#endif
