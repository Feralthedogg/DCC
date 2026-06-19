#ifndef DCC_JSON_GATEWAY_STICKER_LIST_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_STICKER_LIST_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_list_buffers.h"

dcc_status_t dcc_json_gateway_parse_sticker_list_object_fields(
    dcc_json_parser_t *parser,
    dcc_sticker_t *sticker,
    const dcc_json_gateway_sticker_buffers_t *buffers
);

#endif
