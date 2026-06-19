#ifndef DCC_REST_GUILDS_INTEGRATIONS_H
#define DCC_REST_GUILDS_INTEGRATIONS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_integrations(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild_integration(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_guild_integration(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_sync_guild_integration(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
