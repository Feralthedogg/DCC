#ifndef DCC_OBJECTS_RESOURCES_INVITES_H
#define DCC_OBJECTS_RESOURCES_INVITES_H

/* Invite resource object. */

#include <dcc/objects/base.h>

typedef struct dcc_invite {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t target_id;
    dcc_snowflake_t scheduled_event_id;
    dcc_user_t inviter;
    uint32_t target_type;
    uint32_t max_age;
    uint32_t max_uses;
    uint32_t uses;
    uint8_t temporary;
    const char *code;
    const char *created_at;
    const char *expires_at;
} dcc_invite_t;

#endif
