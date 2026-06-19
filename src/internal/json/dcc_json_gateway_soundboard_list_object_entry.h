#ifndef DCC_JSON_GATEWAY_SOUNDBOARD_LIST_OBJECT_ENTRY_H
#define DCC_JSON_GATEWAY_SOUNDBOARD_LIST_OBJECT_ENTRY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_soundboard_list_object(
    dcc_json_parser_t *parser,
    dcc_soundboard_sound_t *sound,
    char *name,
    size_t name_cap,
    char *emoji_name,
    size_t emoji_name_cap,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *avatar,
    size_t avatar_cap
);

#endif
