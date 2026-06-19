#ifndef DCC_JSON_GATEWAY_STAGE_INSTANCE_OBJECT_DISPATCH_H
#define DCC_JSON_GATEWAY_STAGE_INSTANCE_OBJECT_DISPATCH_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resource_buffers.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_stage_instance_object_field(
    dcc_json_parser_t *parser,
    dcc_stage_instance_t *stage_instance,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_stage_instance_buffers_t *buffers
);

#endif
