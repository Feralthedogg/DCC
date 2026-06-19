#ifndef DCC_JSON_GATEWAY_MEMBER_LIST_TIME_FIELDS_H
#define DCC_JSON_GATEWAY_MEMBER_LIST_TIME_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_member_buffers.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_member_list_time_field(
    dcc_json_parser_t *parser,
    dcc_member_t *member,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_member_buffers_t *buffers,
    int *handled
);

#endif
