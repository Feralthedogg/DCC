#ifndef DCC_JSON_GATEWAY_ACTIVITY_ARRAY_ITEM_H
#define DCC_JSON_GATEWAY_ACTIVITY_ARRAY_ITEM_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_activity_buffers.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_activity_array_item(
    dcc_json_parser_t *parser,
    const dcc_json_gateway_activity_array_t *array,
    size_t *stored,
    uint8_t *truncated
);

#endif
