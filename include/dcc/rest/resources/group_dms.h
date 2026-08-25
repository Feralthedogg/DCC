#ifndef DCC_REST_RESOURCES_GROUP_DMS_H
#define DCC_REST_RESOURCES_GROUP_DMS_H

#include <dcc/rest/types.h>
#include <dcc/rest/request.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_add_group_dm_recipient(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    const dcc_group_dm_recipient_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_remove_group_dm_recipient(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
