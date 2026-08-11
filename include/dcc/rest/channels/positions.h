#ifndef DCC_REST_CHANNELS_POSITIONS_H
#define DCC_REST_CHANNELS_POSITIONS_H

#include <dcc/rest/types.h>
#include <dcc/rest/request.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_modify_guild_channel_positions(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_channel_positions_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
