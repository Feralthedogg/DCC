#ifndef DCC_JSON_GATEWAY_ACTIVITY_EMOJI_OBJECT_FIELDS_H
#define DCC_JSON_GATEWAY_ACTIVITY_EMOJI_OBJECT_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_activity_emoji(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activity,
    char *emoji_name,
    size_t emoji_name_cap
);
dcc_status_t dcc_json_gateway_parse_activity_emoji_field(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activity,
    dcc_json_key_id_t key_id,
    char *emoji_name,
    size_t emoji_name_cap
);

#endif
