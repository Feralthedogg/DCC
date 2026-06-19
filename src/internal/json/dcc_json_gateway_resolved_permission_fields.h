#ifndef DCC_JSON_GATEWAY_RESOLVED_PERMISSION_FIELDS_H
#define DCC_JSON_GATEWAY_RESOLVED_PERMISSION_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

void dcc_json_gateway_add_interaction_resolved_permission(
    dcc_json_gateway_payload_t *out,
    dcc_snowflake_t user_id,
    dcc_permission_t permissions
);

#endif
