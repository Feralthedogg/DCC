#ifndef DCC_OBJECTS_RESOURCES_SOUNDBOARD_H
#define DCC_OBJECTS_RESOURCES_SOUNDBOARD_H

/* Soundboard sound resource object. */

#include <dcc/objects/base.h>

typedef struct dcc_soundboard_sound {
    dcc_snowflake_t id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t user_id;
    dcc_snowflake_t emoji_id;
    dcc_user_t user;
    double volume;
    uint8_t available;
    const char *name;
    const char *emoji_name;
} dcc_soundboard_sound_t;

#endif
