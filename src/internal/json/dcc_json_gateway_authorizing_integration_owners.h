#ifndef DCC_JSON_GATEWAY_AUTHORIZING_INTEGRATION_OWNERS_H
#define DCC_JSON_GATEWAY_AUTHORIZING_INTEGRATION_OWNERS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

dcc_status_t dcc_json_gateway_parse_authorizing_integration_owners(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);

#endif
