#include "internal/rest/dcc_rest_channel_permission_builders_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

dcc_status_t dcc_rest_modify_channel_permission(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    const dcc_channel_permission_overwrite_t *overwrite,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_CHANNEL_EDIT_PERMISSIONS, DCC_REST_PUT);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT,
        DCC_ENDPOINT_AUDIT_REASON_ALLOWED
    );
    if (status != DCC_OK || client == NULL || channel_id == 0U ||
        overwrite_id == 0U || overwrite == NULL)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_rest_buffer_t json = {0};
    status = dcc_rest_build_channel_permission_overwrite_body(overwrite, 0U, &json);
    char path[112];
    if (status == DCC_OK) status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/permissions/%llu",
        (unsigned long long)channel_id, (unsigned long long)overwrite_id
    );
    dcc_endpoint_body_t body = { json.data, json.len, "application/json" };
    if (status == DCC_OK) status = dcc_endpoint_submit_named(
        client, "dcc_rest_modify_channel_permission", DCC_REST_PUT, path,
        &body, &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
    dcc_rest_buffer_deinit(&json);
    return status;
}

dcc_status_t dcc_rest_delete_channel_permission(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    dcc_snowflake_t overwrite_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_CHANNEL_DELETE_PERMISSION, DCC_REST_DELETE);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT,
        DCC_ENDPOINT_AUDIT_REASON_ALLOWED
    );
    if (status != DCC_OK || client == NULL || channel_id == 0U || overwrite_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char path[112];
    status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/permissions/%llu",
        (unsigned long long)channel_id, (unsigned long long)overwrite_id
    );
    return status == DCC_OK ? dcc_endpoint_submit_named(
        client, "dcc_rest_delete_channel_permission", DCC_REST_DELETE, path,
        NULL, &resolved, DCC_ENDPOINT_PATH_PUBLIC, out_request
    ) : status;
}
