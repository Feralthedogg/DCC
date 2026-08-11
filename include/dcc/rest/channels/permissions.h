#ifndef DCC_REST_CHANNELS_PERMISSIONS_H
#define DCC_REST_CHANNELS_PERMISSIONS_H

#include <dcc/rest/types.h>
#include <dcc/rest/request.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_modify_channel_permission(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    const dcc_channel_permission_overwrite_t *overwrite,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_delete_channel_permission(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
