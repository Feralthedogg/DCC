#ifndef DCC_REST_RESOURCES_USERS_H
#define DCC_REST_RESOURCES_USERS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_current_application(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_current_user(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_current_user(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_current_user_params(
    dcc_client_t *client,
    const dcc_current_user_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_user(
    dcc_client_t *client,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_current_user_connections(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_current_user_guilds(
    dcc_client_t *client,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_current_user_dms(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_dm_channel(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_dm_channel_params(
    dcc_client_t *client,
    const dcc_dm_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_leave_guild(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
