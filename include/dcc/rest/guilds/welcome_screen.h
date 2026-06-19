#ifndef DCC_REST_GUILDS_WELCOME_SCREEN_H
#define DCC_REST_GUILDS_WELCOME_SCREEN_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_guild_welcome_screen(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild_welcome_screen(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild_welcome_screen_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_welcome_screen_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
