#ifndef DCC_REST_RESOURCES_GROUP_DMS_H
#define DCC_REST_RESOURCES_GROUP_DMS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_add_group_dm_recipient(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_add_group_dm_recipient_params(
    dcc_client_t *client,
    const dcc_group_dm_recipient_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_remove_group_dm_recipient(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
