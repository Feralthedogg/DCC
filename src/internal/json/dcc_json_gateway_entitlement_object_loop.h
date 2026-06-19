#ifndef DCC_JSON_GATEWAY_ENTITLEMENT_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_ENTITLEMENT_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resource_buffers.h"

dcc_status_t dcc_json_gateway_parse_entitlement_object_fields(
    dcc_json_parser_t *parser,
    dcc_entitlement_t *entitlement,
    const dcc_json_gateway_entitlement_buffers_t *buffers
);

#endif
