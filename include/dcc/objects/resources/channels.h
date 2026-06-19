#ifndef DCC_OBJECTS_RESOURCES_CHANNELS_H
#define DCC_OBJECTS_RESOURCES_CHANNELS_H

/* Channel resource object. */

#include <dcc/objects/base.h>

typedef struct dcc_channel {
    dcc_snowflake_t id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t parent_id;
    dcc_snowflake_t last_message_id;
    uint32_t type;
    uint32_t position;
    uint32_t rate_limit_per_user;
    uint32_t bitrate;
    uint32_t user_limit;
    uint8_t nsfw;
    const char *name;
    const char *description;
    const char *topic;
} dcc_channel_t;

#endif
