#ifndef DCC_REST_APPLICATION_COMMANDS_PERMISSIONS_H
#define DCC_REST_APPLICATION_COMMANDS_PERMISSIONS_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_edit_guild_command_permissions(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const dcc_guild_command_permissions_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_all_guild_command_permissions(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_guild_command_permissions(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
