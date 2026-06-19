#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTIONS_ENTRY_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTIONS_ENTRY_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

dcc_status_t dcc_json_gateway_parse_interaction_options(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);

#endif
