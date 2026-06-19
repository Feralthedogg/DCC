#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_modify_channel_permission(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/permissions/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)overwrite_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, json_body, cb, user_data) : status;
}
dcc_status_t dcc_rest_modify_channel_permission_params(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_channel_permission_overwrite_t *overwrite,
    dcc_rest_cb cb,
    void *user_data
) {
    if (overwrite == NULL || overwrite->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_build_channel_permission_overwrite_body(overwrite, 0, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_channel_permission(client, channel_id, overwrite->id, body.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
dcc_status_t dcc_rest_delete_channel_permission(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/permissions/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)overwrite_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
