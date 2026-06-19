#ifndef DCC_OBJECTS_GATEWAY_UPDATES_VOICE_H
#define DCC_OBJECTS_GATEWAY_UPDATES_VOICE_H

/* Voice gateway update payload objects. */

#include <dcc/objects/resources.h>

typedef struct dcc_voice_channel_effect {
    dcc_snowflake_t channel_id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t user_id;
    dcc_snowflake_t emoji_id;
    dcc_snowflake_t animation_id;
    dcc_snowflake_t sound_id;
    uint32_t animation_type;
    double volume;
    const char *emoji_name;
} dcc_voice_channel_effect_t;

typedef struct dcc_voice_server_update {
    dcc_snowflake_t guild_id;
    const char *token;
    const char *endpoint;
} dcc_voice_server_update_t;

typedef struct dcc_voice_channel_start_time_update {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    uint64_t voice_start_time;
} dcc_voice_channel_start_time_update_t;

typedef struct dcc_voice_channel_status_update {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    const char *status;
} dcc_voice_channel_status_update_t;

#endif
