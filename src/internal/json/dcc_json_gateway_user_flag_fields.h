#ifndef DCC_JSON_GATEWAY_USER_FLAG_FIELDS_H
#define DCC_JSON_GATEWAY_USER_FLAG_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_user_flag_field(
    dcc_json_parser_t *parser,
    dcc_user_t *user,
    dcc_json_key_id_t key_id,
    int *handled
);

#endif
