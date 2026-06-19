#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_ARRAY_ENTRY_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_ARRAY_ENTRY_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_interaction_option_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    const dcc_interaction_option_t **items_out,
    size_t *count_out,
    uint8_t *truncated_out,
    unsigned depth
);

#endif
