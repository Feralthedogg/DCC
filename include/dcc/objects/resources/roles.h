#ifndef DCC_OBJECTS_RESOURCES_ROLES_H
#define DCC_OBJECTS_RESOURCES_ROLES_H

/* Role resource object. */

#include <dcc/objects/base.h>

typedef struct dcc_role {
    dcc_snowflake_t id;
    dcc_snowflake_t guild_id;
    dcc_permission_t permissions;
    uint32_t color;
    uint32_t position;
    uint8_t hoist;
    uint8_t managed;
    uint8_t mentionable;
    const char *name;
} dcc_role_t;

#endif
