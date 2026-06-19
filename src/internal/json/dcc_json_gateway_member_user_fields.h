#ifndef DCC_JSON_GATEWAY_MEMBER_USER_FIELDS_H
#define DCC_JSON_GATEWAY_MEMBER_USER_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_member_user_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
);

#endif
