#ifndef DCC_JSON_GATEWAY_ACTIVITY_ARRAY_LOOP_H
#define DCC_JSON_GATEWAY_ACTIVITY_ARRAY_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_activity_buffers.h"

dcc_status_t dcc_json_gateway_parse_activity_array_items(
    dcc_json_parser_t *parser,
    const dcc_json_gateway_activity_array_t *array
);

#endif
