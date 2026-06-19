#ifndef DCC_JSON_GATEWAY_STICKER_LIST_BUFFER_INIT_H
#define DCC_JSON_GATEWAY_STICKER_LIST_BUFFER_INIT_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_list_buffers.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_init_sticker_list_buffers(
    dcc_json_gateway_sticker_buffers_t *buffers,
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
