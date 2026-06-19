#ifndef DCC_JSON_GATEWAY_ROLE_TEXT_FIELDS_H
#define DCC_JSON_GATEWAY_ROLE_TEXT_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_role_text_field(
    dcc_json_parser_t *parser,
    dcc_role_t *role,
    dcc_json_key_id_t key_id,
    char *name,
    size_t name_cap,
    uint64_t *cache_fields,
    int *handled
);

#endif
