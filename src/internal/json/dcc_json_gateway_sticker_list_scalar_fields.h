#ifndef DCC_JSON_GATEWAY_STICKER_LIST_SCALAR_FIELDS_H
#define DCC_JSON_GATEWAY_STICKER_LIST_SCALAR_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

#include <dcc/objects.h>

dcc_status_t dcc_json_gateway_parse_sticker_list_scalar_field(
    dcc_json_parser_t *parser,
    dcc_sticker_t *sticker,
    dcc_json_key_id_t key_id,
    int *handled
);

#endif
