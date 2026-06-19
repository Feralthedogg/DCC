#ifndef DCC_JSON_GATEWAY_MEMBER_LIST_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_MEMBER_LIST_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_member_buffers.h"

dcc_status_t dcc_json_gateway_parse_member_list_object_fields(
    dcc_json_parser_t *parser,
    dcc_member_t *member,
    const dcc_json_gateway_member_buffers_t *buffers
);

#endif
