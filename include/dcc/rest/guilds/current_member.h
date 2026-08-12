#ifndef DCC_REST_GUILDS_CURRENT_MEMBER_H
#define DCC_REST_GUILDS_CURRENT_MEMBER_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_modify_current_guild_member(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_current_guild_member_update_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_set_current_guild_member_nickname(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_current_guild_member_nickname_t *body,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

#ifdef __cplusplus
}
#endif

#endif
