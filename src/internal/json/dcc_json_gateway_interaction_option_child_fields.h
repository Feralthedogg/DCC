#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_CHILD_FIELDS_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_CHILD_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_interaction_option_child_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_interaction_option_t *option,
    dcc_json_key_id_t key_id,
    unsigned depth,
    int *handled
);

#endif
