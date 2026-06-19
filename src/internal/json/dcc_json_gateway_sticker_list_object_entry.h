#ifndef DCC_JSON_GATEWAY_STICKER_LIST_OBJECT_ENTRY_H
#define DCC_JSON_GATEWAY_STICKER_LIST_OBJECT_ENTRY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_sticker_list_object(
    dcc_json_parser_t *parser,
    dcc_sticker_t *sticker,
    char *name,
    size_t name_cap,
    char *description,
    size_t description_cap,
    char *tags,
    size_t tags_cap,
    char *asset,
    size_t asset_cap,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *avatar,
    size_t avatar_cap
);

#endif
