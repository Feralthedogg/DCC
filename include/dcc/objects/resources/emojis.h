#ifndef DCC_OBJECTS_RESOURCES_EMOJIS_H
#define DCC_OBJECTS_RESOURCES_EMOJIS_H

/* Emoji resource object. */

#include <dcc/objects/base.h>

typedef struct dcc_emoji {
    dcc_snowflake_t id;
    dcc_user_t user;
    const dcc_snowflake_t *role_ids;
    size_t role_ids_count;
    uint8_t role_ids_truncated;
    uint8_t require_colons;
    uint8_t managed;
    uint8_t animated;
    uint8_t available;
    const char *name;
} dcc_emoji_t;

#endif
