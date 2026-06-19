#ifndef DCC_JSON_GATEWAY_ROLE_OBJECT_ENTRY_H
#define DCC_JSON_GATEWAY_ROLE_OBJECT_ENTRY_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

dcc_status_t dcc_json_gateway_parse_role(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);

#endif
