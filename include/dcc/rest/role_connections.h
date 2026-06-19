#ifndef DCC_REST_ROLE_CONNECTIONS_H
#define DCC_REST_ROLE_CONNECTIONS_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_application_role_connection_metadata(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_update_application_role_connection_metadata(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_update_application_role_connection_metadata_params(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_role_connection_metadata_params_t *metadata,
    size_t metadata_count,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_current_user_application_role_connection(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_update_current_user_application_role_connection(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_update_current_user_application_role_connection_params(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_role_connection_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
