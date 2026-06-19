#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_OBJECT_ENTRY_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_OBJECT_ENTRY_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_interaction_option_object(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_interaction_option_t *option,
    size_t list_index,
    size_t index,
    unsigned depth
);

#endif
