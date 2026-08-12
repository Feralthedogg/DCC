#ifndef DCC_REST_GUILD_MEMBERS_ROLES_H
#define DCC_REST_GUILD_MEMBERS_ROLES_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_add_guild_member_role(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t user_id,
    dcc_snowflake_t role_id, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_remove_guild_member_role(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t user_id,
    dcc_snowflake_t role_id, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

#ifdef __cplusplus
}
#endif

#endif
