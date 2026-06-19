#ifndef DCC_JSON_GATEWAY_STICKER_BUFFERS_H
#define DCC_JSON_GATEWAY_STICKER_BUFFERS_H

#include <stddef.h>

typedef struct dcc_json_gateway_sticker_buffers {
    char *name;
    size_t name_cap;
    char *description;
    size_t description_cap;
    char *tags;
    size_t tags_cap;
    char *asset;
    size_t asset_cap;
    char *username;
    size_t username_cap;
    char *global_name;
    size_t global_name_cap;
    char *avatar;
    size_t avatar_cap;
} dcc_json_gateway_sticker_buffers_t;

#endif
