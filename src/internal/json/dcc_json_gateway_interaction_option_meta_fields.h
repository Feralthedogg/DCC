#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_META_FIELDS_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_META_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_interaction_state.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_meta_field(
    dcc_json_parser_t *parser,
    dcc_interaction_option_t *option,
    dcc_json_key_id_t key_id,
    dcc_json_gateway_interaction_option_state_t *state,
    int *handled
);

#endif
