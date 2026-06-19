#ifndef DCC_REST_GUILDS_CORE_H
#define DCC_REST_GUILDS_CORE_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild_params(
    dcc_client_t *client,
    const dcc_guild_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild_params(
    dcc_client_t *client,
    const dcc_guild_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_guild(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_guild_preview(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
