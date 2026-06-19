#ifndef DCC_OBJECTS_RESOURCES_MEMBERS_H
#define DCC_OBJECTS_RESOURCES_MEMBERS_H

/* Guild member resource object. */

#include <dcc/objects/base.h>

typedef struct dcc_member {
    dcc_snowflake_t guild_id;
    dcc_user_t user;
    const dcc_snowflake_t *role_ids;
    size_t role_ids_count;
    uint8_t role_ids_truncated;
    uint64_t flags;
    uint8_t pending;
    const char *nick;
    const char *avatar;
    const char *joined_at;
    const char *premium_since;
    const char *communication_disabled_until;
} dcc_member_t;

#endif
