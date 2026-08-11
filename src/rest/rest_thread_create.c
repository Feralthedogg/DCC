#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_threads_internal.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t create_thread_common(
    dcc_client_t *client, const char *operation, const char *path,
    const dcc_thread_params_t *params, dcc_rest_thread_body_mode_t mode,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare_policy(
        options, out_request, &resolved, DCC_ENDPOINT_AUTH_POLICY_BOT,
        DCC_ENDPOINT_AUDIT_REASON_ALLOWED
    );
    if (status != DCC_OK || client == NULL)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_endpoint_body_t body = {0};
    status = dcc_rest_build_thread_body(params, mode, &body);
    if (status == DCC_OK) status = dcc_endpoint_submit_named(
        client, operation, DCC_REST_POST, path, &body, &resolved,
        DCC_ENDPOINT_PATH_PUBLIC, out_request
    );
    dcc_endpoint_body_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_create_thread(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_thread_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_THREAD_CREATE, DCC_REST_POST);
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    if (out_request != NULL) *out_request = NULL;
    if (channel_id == 0U) return DCC_ERR_INVALID_ARG;
    char path[80];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/threads",
        (unsigned long long)channel_id
    );
    return status == DCC_OK ? create_thread_common(
        client, "dcc_rest_create_thread", path, params,
        DCC_REST_THREAD_BODY_CREATE, options, out_request
    ) : status;
}

dcc_status_t dcc_rest_create_forum_thread(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    const dcc_thread_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_THREAD_CREATE, DCC_REST_POST);
    if (out_request != NULL) *out_request = NULL;
    if (channel_id == 0U) return DCC_ERR_INVALID_ARG;
    char path[80];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/threads",
        (unsigned long long)channel_id
    );
    return status == DCC_OK ? create_thread_common(
        client, "dcc_rest_create_forum_thread", path, params,
        DCC_REST_THREAD_BODY_CREATE_FORUM, options, out_request
    ) : status;
}

dcc_status_t dcc_rest_create_thread_from_message(
    dcc_client_t *client, dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id, const dcc_thread_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_DPP_THREAD_CREATE_WITH_MESSAGE, DCC_REST_POST);
    if (out_request != NULL) *out_request = NULL;
    if (channel_id == 0U || message_id == 0U) return DCC_ERR_INVALID_ARG;
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path, sizeof(path), "/channels/%llu/messages/%llu/threads",
        (unsigned long long)channel_id, (unsigned long long)message_id
    );
    return status == DCC_OK ? create_thread_common(
        client, "dcc_rest_create_thread_from_message", path, params,
        DCC_REST_THREAD_BODY_CREATE_FROM_MESSAGE, options, out_request
    ) : status;
}
