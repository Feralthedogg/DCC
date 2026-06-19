#ifndef DCC_JSON_GATEWAY_STAGE_INSTANCE_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_STAGE_INSTANCE_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resource_buffers.h"

dcc_status_t dcc_json_gateway_parse_stage_instance_object_fields(
    dcc_json_parser_t *parser,
    dcc_stage_instance_t *stage_instance,
    const dcc_json_gateway_stage_instance_buffers_t *buffers
);

#endif
