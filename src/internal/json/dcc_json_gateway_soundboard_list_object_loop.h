#ifndef DCC_JSON_GATEWAY_SOUNDBOARD_LIST_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_SOUNDBOARD_LIST_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_list_buffers.h"

dcc_status_t dcc_json_gateway_parse_soundboard_list_object_fields(
    dcc_json_parser_t *parser,
    dcc_soundboard_sound_t *sound,
    const dcc_json_gateway_soundboard_buffers_t *buffers
);

#endif
