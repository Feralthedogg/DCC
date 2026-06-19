#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_STATE_FINALIZE_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_STATE_FINALIZE_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_interaction_state.h"
#include "internal/json/dcc_json_gateway_payload.h"

dcc_status_t dcc_json_gateway_finalize_interaction_option_state(
    dcc_interaction_option_t *option,
    const dcc_json_gateway_interaction_option_state_t *state
);

#endif
