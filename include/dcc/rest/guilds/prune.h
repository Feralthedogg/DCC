#ifndef DCC_REST_GUILDS_PRUNE_H
#define DCC_REST_GUILDS_PRUNE_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_prune_count(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_prune_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_begin_guild_prune(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_guild_prune_t *body, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

#ifdef __cplusplus
}
#endif

#endif
