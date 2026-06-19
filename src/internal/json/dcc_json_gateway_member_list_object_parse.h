#ifndef DCC_JSON_GATEWAY_MEMBER_LIST_OBJECT_PARSE_H
#define DCC_JSON_GATEWAY_MEMBER_LIST_OBJECT_PARSE_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_member_buffers.h"

dcc_status_t dcc_json_gateway_parse_member_list_object_with_buffers(
    dcc_json_parser_t *parser,
    dcc_member_t *member,
    dcc_snowflake_t guild_id,
    const dcc_json_gateway_member_buffers_t *buffers
);

#endif
