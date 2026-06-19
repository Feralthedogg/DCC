#ifndef DCC_JSON_GATEWAY_USER_PAYLOAD_ENTRY_H
#define DCC_JSON_GATEWAY_USER_PAYLOAD_ENTRY_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

dcc_status_t dcc_json_gateway_parse_author(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
dcc_status_t dcc_json_gateway_parse_user(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);

#endif
