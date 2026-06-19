#ifndef DCC_JSON_GATEWAY_PRESENCE_LIST_OBJECT_PARSE_H
#define DCC_JSON_GATEWAY_PRESENCE_LIST_OBJECT_PARSE_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_presence_buffers.h"

dcc_status_t dcc_json_gateway_parse_presence_list_object_with_buffers(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    const dcc_json_gateway_presence_list_buffers_t *buffers
);

#endif
