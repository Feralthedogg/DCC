#ifndef DCC_REST_GUILD_MEMBERS_QUERY_H
#define DCC_REST_GUILD_MEMBERS_QUERY_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_member(
    dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_list_guild_members(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_members_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_search_guild_members(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_member_search_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

#ifdef __cplusplus
}
#endif

#endif
