#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_VALUE_FIELDS_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_VALUE_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_interaction_state.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_value_field(
    dcc_json_parser_t *parser,
    dcc_json_key_id_t key_id,
    dcc_json_gateway_interaction_option_state_t *state,
    int *handled
);

#endif
