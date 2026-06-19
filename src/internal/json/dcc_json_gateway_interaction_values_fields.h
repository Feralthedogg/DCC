#ifndef DCC_JSON_GATEWAY_INTERACTION_VALUES_FIELDS_H
#define DCC_JSON_GATEWAY_INTERACTION_VALUES_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload_state.h"

dcc_status_t dcc_json_gateway_parse_interaction_values(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);

#endif
