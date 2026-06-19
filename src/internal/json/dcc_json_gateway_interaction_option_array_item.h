#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_ARRAY_ITEM_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_ARRAY_ITEM_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_interaction_option_array_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_interaction_option_t *items,
    size_t list_index,
    size_t index,
    unsigned depth,
    size_t *count_out,
    uint8_t *truncated_out
);

#endif
