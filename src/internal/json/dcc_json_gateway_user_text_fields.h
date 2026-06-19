#ifndef DCC_JSON_GATEWAY_USER_TEXT_FIELDS_H
#define DCC_JSON_GATEWAY_USER_TEXT_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_member_buffers.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_user_text_field(
    dcc_json_parser_t *parser,
    dcc_user_t *user,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_user_buffers_t *buffers,
    int *handled
);

#endif
