#ifndef DCC_REST_RESOURCES_STAGE_INSTANCES_H
#define DCC_REST_RESOURCES_STAGE_INSTANCES_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_create_stage_instance(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_stage_instance_params(
    dcc_client_t *client,
    const dcc_stage_instance_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_stage_instance(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_stage_instance(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_stage_instance_params(
    dcc_client_t *client,
    const dcc_stage_instance_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_stage_instance(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
