#ifndef DCC_OBJECTS_GATEWAY_UPDATES_APPLICATION_COMMANDS_H
#define DCC_OBJECTS_GATEWAY_UPDATES_APPLICATION_COMMANDS_H

/* Application command gateway update payload objects. */

#include <dcc/objects/resources.h>

typedef struct dcc_application_command_permission {
    dcc_snowflake_t id;
    uint32_t type;
    uint8_t permission;
} dcc_application_command_permission_t;

typedef struct dcc_application_command_permissions_update {
    dcc_snowflake_t id;
    dcc_snowflake_t application_id;
    dcc_snowflake_t guild_id;
    const dcc_application_command_permission_t *permissions;
    size_t permissions_count;
    uint8_t permissions_truncated;
} dcc_application_command_permissions_update_t;

#endif
