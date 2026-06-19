#ifndef DCC_JSON_GATEWAY_ROLE_ID_FIELDS_H
#define DCC_JSON_GATEWAY_ROLE_ID_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_role_id_field(
    dcc_json_parser_t *parser,
    dcc_role_t *role,
    dcc_json_key_id_t key_id,
    int *handled
);

#endif
