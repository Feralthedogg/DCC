#ifndef DCC_JSON_GATEWAY_PAYLOAD_PARSE_FIELDS_H
#define DCC_JSON_GATEWAY_PAYLOAD_PARSE_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload_state.h"
#include "internal/json/dcc_json_keys.h"

void dcc_json_gateway_payload_reset(dcc_json_gateway_payload_t *out);
dcc_status_t dcc_json_gateway_parse_payload_value(
    dcc_json_parser_t *parser,
    dcc_json_key_id_t key_id,
    dcc_json_gateway_payload_t *out
);
dcc_status_t dcc_json_gateway_parse_d(dcc_json_parser_t *parser, dcc_json_gateway_payload_t *out);

#endif
