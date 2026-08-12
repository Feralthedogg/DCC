#ifndef DCC_REST_GUILDS_SCHEDULED_EVENTS_H
#define DCC_REST_GUILDS_SCHEDULED_EVENTS_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_create_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_scheduled_event_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_delete_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const dcc_rest_scheduled_event_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_guild_scheduled_event_users(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const dcc_rest_scheduled_event_users_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_guild_scheduled_events(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_rest_scheduled_event_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_modify_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const dcc_scheduled_event_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
