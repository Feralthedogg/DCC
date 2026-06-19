#ifndef DCC_JSON_GATEWAY_ROLE_LIST_OBJECT_DISPATCH_H
#define DCC_JSON_GATEWAY_ROLE_LIST_OBJECT_DISPATCH_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_role_list_object_field(
    dcc_json_parser_t *parser,
    dcc_role_t *role,
    char *name,
    size_t name_cap
);

#endif
