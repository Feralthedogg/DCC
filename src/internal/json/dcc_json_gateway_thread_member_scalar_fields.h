#ifndef DCC_JSON_GATEWAY_THREAD_MEMBER_SCALAR_FIELDS_H
#define DCC_JSON_GATEWAY_THREAD_MEMBER_SCALAR_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

#include <dcc/objects.h>

dcc_status_t dcc_json_gateway_parse_thread_member_scalar_field(
    dcc_json_parser_t *parser,
    dcc_thread_member_t *member,
    dcc_json_key_id_t key_id,
    int *handled
);

#endif
