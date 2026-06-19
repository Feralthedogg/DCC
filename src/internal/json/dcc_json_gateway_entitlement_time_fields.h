#ifndef DCC_JSON_GATEWAY_ENTITLEMENT_TIME_FIELDS_H
#define DCC_JSON_GATEWAY_ENTITLEMENT_TIME_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resource_buffers.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_entitlement_time_field(
    dcc_json_parser_t *parser,
    dcc_entitlement_t *entitlement,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_entitlement_buffers_t *buffers,
    int *handled
);

#endif
