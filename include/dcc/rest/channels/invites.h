#ifndef DCC_REST_CHANNELS_INVITES_H
#define DCC_REST_CHANNELS_INVITES_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_channel_invites(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_channel_invite(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_channel_invite_params(
    dcc_client_t *client,
    const dcc_invite_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
