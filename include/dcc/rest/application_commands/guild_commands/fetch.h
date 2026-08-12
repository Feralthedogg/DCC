#ifndef DCC_REST_APPLICATION_COMMANDS_GUILD_COMMANDS_FETCH_H
#define DCC_REST_APPLICATION_COMMANDS_GUILD_COMMANDS_FETCH_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_command(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_guild_commands(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    const dcc_rest_command_list_query_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
