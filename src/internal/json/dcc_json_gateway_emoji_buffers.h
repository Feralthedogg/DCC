#ifndef DCC_JSON_GATEWAY_EMOJI_BUFFERS_H
#define DCC_JSON_GATEWAY_EMOJI_BUFFERS_H

#include <stddef.h>

typedef struct dcc_json_gateway_emoji_buffers {
    char *name;
    size_t name_cap;
    char *username;
    size_t username_cap;
    char *global_name;
    size_t global_name_cap;
    char *avatar;
    size_t avatar_cap;
} dcc_json_gateway_emoji_buffers_t;

#endif
