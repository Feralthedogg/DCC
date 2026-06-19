#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_interaction_state.h"
#include "internal/json/dcc_json_gateway_payload.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_object_fields(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_interaction_option_t *option,
    dcc_json_gateway_interaction_option_state_t *state,
    unsigned depth
);

#endif
