#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_create_dm_channel(
    dcc_client_t *client,
    const dcc_dm_channel_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_CREATE_DM_CHANNEL, DCC_REST_POST);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT,
        DCC_ENDPOINT_AUDIT_REASON_DENIED
    );
    dcc_endpoint_record_view_t params_view;
    if (status != DCC_OK || client == NULL ||
        dcc_endpoint_record_read(
            params, offsetof(dcc_dm_channel_params_t, version), SIZE_MAX,
            DCC_ENDPOINT_FIELD_END(dcc_dm_channel_params_t, recipient_id),
            DCC_DM_CHANNEL_PARAMS_VERSION, 0U, &params_view
        ) != DCC_OK ||
        params->recipient_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_rest_buffer_t json = {0};
    status = dcc_rest_buffer_append_cstr(&json, "{");
    int first = 1;
    if (status == DCC_OK) status = dcc_rest_json_append_u64_string_member(
        &json, &first, "recipient_id", params->recipient_id
    );
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "}");
    dcc_endpoint_body_t body = {json.data, json.len, "application/json"};
    if (status == DCC_OK) status = dcc_endpoint_submit_named(
        client, "dcc_rest_create_dm_channel", DCC_REST_POST,
        "/users/@me/channels", &body, &resolved, DCC_ENDPOINT_PATH_PUBLIC,
        out_request
    );
    dcc_rest_buffer_deinit(&json);
    return status;
}
