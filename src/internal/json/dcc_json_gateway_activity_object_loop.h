#ifndef DCC_JSON_GATEWAY_ACTIVITY_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_ACTIVITY_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_activity_buffers.h"

dcc_status_t dcc_json_gateway_parse_activity_object_fields(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activity,
    const dcc_json_gateway_activity_buffers_t *buffers
);

#endif
