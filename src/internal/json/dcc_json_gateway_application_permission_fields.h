#ifndef DCC_JSON_GATEWAY_APPLICATION_PERMISSION_FIELDS_H
#define DCC_JSON_GATEWAY_APPLICATION_PERMISSION_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_application_command_permissions_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
dcc_status_t dcc_json_gateway_parse_application_command_permission_object(
    dcc_json_parser_t *parser,
    dcc_application_command_permission_t *permission
);
dcc_status_t dcc_json_gateway_parse_application_command_permission_object_fields(
    dcc_json_parser_t *parser,
    dcc_application_command_permission_t *permission
);
dcc_status_t dcc_json_gateway_parse_application_command_permission_field(
    dcc_json_parser_t *parser,
    dcc_application_command_permission_t *permission,
    dcc_json_key_id_t key_id,
    int *handled
);

#endif
