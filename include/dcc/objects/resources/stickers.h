#ifndef DCC_OBJECTS_RESOURCES_STICKERS_H
#define DCC_OBJECTS_RESOURCES_STICKERS_H

/* Sticker resource object. */

#include <dcc/objects/base.h>

typedef struct dcc_sticker {
    dcc_snowflake_t id;
    dcc_snowflake_t pack_id;
    dcc_snowflake_t guild_id;
    dcc_user_t user;
    uint32_t type;
    uint32_t format_type;
    uint32_t sort_value;
    const char *name;
    const char *description;
    const char *tags;
    const char *asset;
} dcc_sticker_t;

#endif
