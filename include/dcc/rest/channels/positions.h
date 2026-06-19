#ifndef DCC_REST_CHANNELS_POSITIONS_H
#define DCC_REST_CHANNELS_POSITIONS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_modify_guild_channel_positions(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_guild_channel_positions_params(
    dcc_client_t *client,
    const dcc_channel_positions_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
