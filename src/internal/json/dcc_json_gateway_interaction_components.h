#ifndef DCC_JSON_GATEWAY_INTERACTION_COMPONENTS_H
#define DCC_JSON_GATEWAY_INTERACTION_COMPONENTS_H

#include "internal/json/dcc_json_core.h"

typedef struct dcc_json_gateway_payload dcc_json_gateway_payload_t;

dcc_status_t dcc_json_gateway_parse_interaction_components(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);

#endif
