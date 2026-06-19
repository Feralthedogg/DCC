#include "internal/command_registry/dcc_command_registry_internal.h"

#include <dcc/rest/application_commands/global_commands/fetch.h>
#include <dcc/rest/application_commands/guild_commands/fetch.h>

dcc_status_t dcc_command_registry_fetch_remote(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *options,
    dcc_rest_cb cb,
    void *user_data
) {
    if (client == NULL || application_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_snowflake_t guild_id = dcc_command_registry_options_guild_id(options);
    return guild_id != 0
        ? dcc_rest_get_guild_commands_with_localizations(client, application_id, guild_id, 0, cb, user_data)
        : dcc_rest_get_global_commands_with_localizations(client, application_id, 0, cb, user_data);
}
