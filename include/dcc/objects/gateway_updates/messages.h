#ifndef DCC_OBJECTS_GATEWAY_UPDATES_MESSAGES_H
#define DCC_OBJECTS_GATEWAY_UPDATES_MESSAGES_H

/* Message and poll gateway update payload objects. */

#include <dcc/objects/resources.h>

typedef struct dcc_message_reaction {
    dcc_snowflake_t user_id;
    dcc_user_t user;
    dcc_member_t member;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t message_id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t message_author_id;
    dcc_snowflake_t emoji_id;
    dcc_emoji_t emoji;
    uint8_t burst;
    const char *emoji_name;
} dcc_message_reaction_t;

typedef struct dcc_poll_vote {
    dcc_snowflake_t user_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t message_id;
    dcc_snowflake_t guild_id;
    uint32_t answer_id;
} dcc_poll_vote_t;

typedef struct dcc_message_delete_bulk {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    const dcc_snowflake_t *ids;
    size_t ids_count;
    uint8_t ids_truncated;
} dcc_message_delete_bulk_t;

#endif
