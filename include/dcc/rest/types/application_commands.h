#ifndef DCC_REST_TYPES_APPLICATION_COMMANDS_H
#define DCC_REST_TYPES_APPLICATION_COMMANDS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_guild_command_permissions_params {
    size_t size;
    dcc_snowflake_t application_id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t command_id;
    const dcc_application_command_permission_t *permissions;
    size_t permission_count;
} dcc_guild_command_permissions_params_t;

typedef struct dcc_guild_command_permissions_update_params {
    dcc_snowflake_t command_id;
    const dcc_application_command_permission_t *permissions;
    size_t permission_count;
} dcc_guild_command_permissions_update_params_t;

typedef struct dcc_bulk_guild_command_permissions_params {
    size_t size;
    dcc_snowflake_t application_id;
    dcc_snowflake_t guild_id;
    const dcc_guild_command_permissions_update_params_t *commands;
    size_t command_count;
} dcc_bulk_guild_command_permissions_params_t;

typedef struct dcc_application_command_registration_options {
    size_t size;
    dcc_snowflake_t guild_id;
} dcc_application_command_registration_options_t;

#ifdef __cplusplus
}
#endif

#endif
