#ifndef DCC_REST_CHANNELS_INVITES_H
#define DCC_REST_CHANNELS_INVITES_H

#include <dcc/rest/types.h>
#include <dcc/rest/request.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_channel_invites(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_create_channel_invite(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_invite_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
