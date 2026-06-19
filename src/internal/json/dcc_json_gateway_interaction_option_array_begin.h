#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_ARRAY_BEGIN_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_ARRAY_BEGIN_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_gateway_begin_interaction_option_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    const dcc_interaction_option_t **items_out,
    uint8_t *truncated_out,
    unsigned depth,
    size_t *list_index_out,
    dcc_interaction_option_t **items_mut_out
);

#endif
