#ifndef DCC_REST_TYPES_EMOJIS_STICKERS_H
#define DCC_REST_TYPES_EMOJIS_STICKERS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_guild_sticker_params {
    size_t size;
    dcc_snowflake_t guild_id;
    const char *name;
    const char *description;
    const char *tags;
    const char *filename;
    const char *content_type;
    const void *data;
    size_t data_len;
} dcc_guild_sticker_params_t;

typedef struct dcc_emoji_params {
    size_t size;
    dcc_snowflake_t emoji_id;
    const char *name;
    const char *image;
    const dcc_snowflake_t *roles;
    size_t role_count;
} dcc_emoji_params_t;

#ifdef __cplusplus
}
#endif

#endif
