#ifndef DCC_JSON_GATEWAY_EMOJI_LIST_OBJECT_DISPATCH_H
#define DCC_JSON_GATEWAY_EMOJI_LIST_OBJECT_DISPATCH_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_list_buffers.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_emoji_list_object_field(
    dcc_json_parser_t *parser,
    dcc_emoji_t *emoji,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_emoji_buffers_t *buffers
);

#endif
