#ifndef DCC_REST_GUILDS_SCHEDULED_EVENTS_H
#define DCC_REST_GUILDS_SCHEDULED_EVENTS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_scheduled_events(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_guild_scheduled_events_with_user_count(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_guild_scheduled_event_with_user_count(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild_scheduled_event_params(
    dcc_client_t *client,
    const dcc_scheduled_event_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild_scheduled_event_params(
    dcc_client_t *client,
    const dcc_scheduled_event_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_guild_scheduled_event_users(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_guild_scheduled_event_users_page(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    uint8_t limit,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
