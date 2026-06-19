#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_application_role_connection_metadata(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/role-connections/metadata",
        (unsigned long long)application_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_update_application_role_connection_metadata(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/role-connections/metadata",
        (unsigned long long)application_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_update_application_role_connection_metadata_params(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_role_connection_metadata_params_t *metadata,
    size_t metadata_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_application_role_connection_metadata_body(
        metadata,
        metadata_count,
        &body
    );
    if (status == DCC_OK) {
        status = dcc_rest_update_application_role_connection_metadata(client, application_id, body, cb, user_data);
    }
    free(body);
    return status;
}
