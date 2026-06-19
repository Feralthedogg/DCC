#ifndef DCC_JSON_GATEWAY_STICKER_LIST_TEXT_FIELDS_H
#define DCC_JSON_GATEWAY_STICKER_LIST_TEXT_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_list_buffers.h"
#include "internal/json/dcc_json_keys.h"

#include <dcc/objects.h>

dcc_status_t dcc_json_gateway_parse_sticker_list_text_field(
    dcc_json_parser_t *parser,
    dcc_sticker_t *sticker,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_sticker_buffers_t *buffers,
    int *handled
);

#endif
