#ifndef DCC_JSON_GATEWAY_ROLE_SCALAR_FIELDS_H
#define DCC_JSON_GATEWAY_ROLE_SCALAR_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_role_scalar_field(
    dcc_json_parser_t *parser,
    dcc_role_t *role,
    dcc_json_key_id_t key_id,
    uint64_t *cache_fields,
    int *handled
);

#endif
