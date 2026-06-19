#ifndef DCC_JSON_GATEWAY_INTERACTION_DATA_DISPATCH_FIELDS_H
#define DCC_JSON_GATEWAY_INTERACTION_DATA_DISPATCH_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload_state.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_interaction_data_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id
);

#endif
