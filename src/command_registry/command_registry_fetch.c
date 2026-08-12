#include "internal/command_registry/dcc_command_registry_internal.h"

#include <dcc/rest/application_commands/global_commands/fetch.h>
#include <dcc/rest/application_commands/guild_commands/fetch.h>

dcc_status_t dcc_command_registry_fetch_remote(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *registry_options,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    if (out_request != NULL) *out_request = NULL;
    if (client == NULL || application_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_snowflake_t guild_id = dcc_command_registry_options_guild_id(registry_options);
    return guild_id != 0
        ? dcc_rest_get_guild_commands(client, application_id, guild_id, NULL,
                                      options, out_request)
        : dcc_rest_get_global_commands(client, application_id, NULL, options,
                                       out_request);
}
