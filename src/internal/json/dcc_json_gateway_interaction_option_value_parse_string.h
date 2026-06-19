#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_VALUE_PARSE_STRING_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_VALUE_PARSE_STRING_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_interaction_state.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_string_value(
    dcc_json_parser_t *parser,
    dcc_json_gateway_interaction_option_state_t *state
);

#endif
