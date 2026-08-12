#ifndef DCC_REST_ROLES_H
#define DCC_REST_ROLES_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_roles(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_create_guild_role(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_role_create_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_modify_guild_role(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t role_id,
    const dcc_rest_guild_role_update_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_modify_guild_role_positions(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_role_position_t *positions, size_t position_count,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_delete_guild_role(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t role_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

#ifdef __cplusplus
}
#endif

#endif
