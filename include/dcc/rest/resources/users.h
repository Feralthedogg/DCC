#ifndef DCC_REST_RESOURCES_USERS_H
#define DCC_REST_RESOURCES_USERS_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_current_application(
    dcc_client_t *client, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_get_current_user(
    dcc_client_t *client, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_modify_current_user(
    dcc_client_t *client, const dcc_current_user_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_get_user(
    dcc_client_t *client, dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_get_current_user_connections(
    dcc_client_t *client, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_get_current_user_guilds(
    dcc_client_t *client, const dcc_rest_current_user_guilds_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_create_dm_channel(
    dcc_client_t *client, const dcc_dm_channel_params_t *params,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

DCC_API dcc_status_t dcc_rest_leave_guild(
    dcc_client_t *client, dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request);

#ifdef __cplusplus
}
#endif

#endif
