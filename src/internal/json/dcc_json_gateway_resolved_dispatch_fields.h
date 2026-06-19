#ifndef DCC_JSON_GATEWAY_RESOLVED_DISPATCH_FIELDS_H
#define DCC_JSON_GATEWAY_RESOLVED_DISPATCH_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_interaction_resolved(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
dcc_status_t dcc_json_gateway_parse_interaction_resolved_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id
);

#endif
