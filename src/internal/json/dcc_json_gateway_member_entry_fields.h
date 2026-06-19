#ifndef DCC_JSON_GATEWAY_MEMBER_ENTRY_FIELDS_H
#define DCC_JSON_GATEWAY_MEMBER_ENTRY_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

dcc_status_t dcc_json_gateway_parse_member(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
dcc_status_t dcc_json_gateway_parse_member_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
void dcc_json_gateway_finish_member_object(dcc_json_gateway_payload_t *out);

#endif
