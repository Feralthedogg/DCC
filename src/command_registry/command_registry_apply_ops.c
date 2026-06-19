#include "internal/command_registry/dcc_command_registry_internal.h"

#include <dcc/rest/application_commands/global_commands/write.h>
#include <dcc/rest/application_commands/guild_commands/write.h>

dcc_snowflake_t dcc_command_registry_plan_guild_id(
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_plan_t *plan
) {
    dcc_snowflake_t guild_id = dcc_command_registry_options_guild_id(options);
    return guild_id != 0 || plan == NULL ? guild_id : plan->guild_id;
}

dcc_status_t dcc_command_registry_apply_create(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    const char *json,
    dcc_rest_cb cb,
    void *user_data
) {
    return guild_id != 0
        ? dcc_rest_create_guild_command(client, application_id, guild_id, json, cb, user_data)
        : dcc_rest_create_global_command(client, application_id, json, cb, user_data);
}

dcc_status_t dcc_command_registry_apply_update(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const char *json,
    dcc_rest_cb cb,
    void *user_data
) {
    if (command_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    return guild_id != 0
        ? dcc_rest_edit_guild_command(client, application_id, guild_id, command_id, json, cb, user_data)
        : dcc_rest_edit_global_command(client, application_id, command_id, json, cb, user_data);
}

dcc_status_t dcc_command_registry_apply_delete(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    dcc_rest_cb cb,
    void *user_data
) {
    if (command_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    return guild_id != 0
        ? dcc_rest_delete_guild_command(client, application_id, guild_id, command_id, cb, user_data)
        : dcc_rest_delete_global_command(client, application_id, command_id, cb, user_data);
}
