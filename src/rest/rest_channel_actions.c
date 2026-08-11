#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t submit_action(
    dcc_client_t *client, const char *operation, dcc_rest_method_t method,
    const char *path, const dcc_endpoint_body_t *body,
    const dcc_rest_call_options_t *options, dcc_endpoint_audit_policy_t audit,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT, audit
    );
    if (status != DCC_OK || client == NULL)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    return dcc_endpoint_submit_named(
        client, operation, method, path, body, &resolved,
        DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
}

dcc_status_t dcc_rest_follow_news_channel(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_rest_follow_news_channel_t *follow,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_CHANNEL_FOLLOW_NEWS, DCC_REST_POST);
    if (out_request != NULL) *out_request = NULL;
    dcc_endpoint_record_view_t follow_view;
    if (channel_id == 0U ||
        dcc_endpoint_record_read(
            follow, offsetof(dcc_rest_follow_news_channel_t, version),
            SIZE_MAX,
            DCC_ENDPOINT_FIELD_END(dcc_rest_follow_news_channel_t, webhook_channel_id),
            DCC_REST_FOLLOW_NEWS_CHANNEL_VERSION, 0U, &follow_view
        ) != DCC_OK ||
        follow->webhook_channel_id == 0U) return DCC_ERR_INVALID_ARG;
    char path[80];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/followers",
        (unsigned long long)channel_id
    );
    dcc_rest_buffer_t json = {0};
    int first = 1;
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "{");
    if (status == DCC_OK) status = dcc_rest_json_append_u64_string_member(
        &json, &first, "webhook_channel_id", follow->webhook_channel_id
    );
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "}");
    dcc_endpoint_body_t body = { json.data, json.len, "application/json" };
    if (status == DCC_OK) status = submit_action(
        client, "dcc_rest_follow_news_channel", DCC_REST_POST, path, &body,
        options, DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request
    );
    dcc_rest_buffer_deinit(&json);
    return status;
}

dcc_status_t dcc_rest_trigger_channel_typing(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_DPP_CHANNEL_TYPING, DCC_REST_POST);
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    if (out_request != NULL) *out_request = NULL;
    if (channel_id == 0U) return DCC_ERR_INVALID_ARG;
    char path[80];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/typing",
        (unsigned long long)channel_id
    );
    return status == DCC_OK ? submit_action(
        client, "dcc_rest_trigger_channel_typing", DCC_REST_POST, path, NULL,
        options, DCC_ENDPOINT_AUDIT_REASON_DENIED, out_request
    ) : status;
}

dcc_status_t dcc_rest_set_channel_voice_status(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_channel_voice_status_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_CHANNEL_SET_VOICE_STATUS, DCC_REST_PUT);
    if (out_request != NULL) *out_request = NULL;
    dcc_endpoint_record_view_t params_view;
    size_t status_scalars = 0U;
    if (channel_id == 0U ||
        dcc_endpoint_record_read(
            params, offsetof(dcc_channel_voice_status_params_t, version),
            offsetof(dcc_channel_voice_status_params_t, present),
            DCC_ENDPOINT_FIELD_END(dcc_channel_voice_status_params_t, present),
            DCC_CHANNEL_VOICE_STATUS_PARAMS_VERSION,
            DCC_CHANNEL_VOICE_STATUS_PRESENT_STATUS, &params_view
        ) != DCC_OK ||
        params_view.present != DCC_CHANNEL_VOICE_STATUS_PRESENT_STATUS ||
        !dcc_endpoint_present_field_covered(
            &params_view, DCC_CHANNEL_VOICE_STATUS_PRESENT_STATUS,
            offsetof(dcc_channel_voice_status_params_t, status),
            sizeof(params->status)
        ) ||
        (params->status != NULL &&
         (!dcc_endpoint_utf8_scalar_count(params->status, &status_scalars) ||
          status_scalars > 500U)))
        return DCC_ERR_INVALID_ARG;
    char path[88];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/voice-status",
        (unsigned long long)channel_id
    );
    dcc_rest_buffer_t json = {0};
    int first = 1;
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "{");
    if (status == DCC_OK) status = dcc_rest_json_append_nullable_string_member(
        &json, &first, "status", params->status
    );
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "}");
    dcc_endpoint_body_t body = { json.data, json.len, "application/json" };
    if (status == DCC_OK) status = submit_action(
        client, "dcc_rest_set_channel_voice_status", DCC_REST_PUT, path,
        &body, options, DCC_ENDPOINT_AUDIT_REASON_ALLOWED, out_request
    );
    dcc_rest_buffer_deinit(&json);
    return status;
}
