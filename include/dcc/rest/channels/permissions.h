#ifndef DCC_REST_CHANNELS_PERMISSIONS_H
#define DCC_REST_CHANNELS_PERMISSIONS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_modify_channel_permission(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_channel_permission_params(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_channel_permission_overwrite_t *overwrite,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_channel_permission(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
