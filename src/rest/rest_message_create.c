#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_create_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_message_payload_t *payload,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_CHANNEL_MESSAGES, DCC_REST_POST);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U) {
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    }
    status = dcc_endpoint_message_payload_preflight(payload);
    if (status != DCC_OK) return status;
    dcc_endpoint_body_t body = {0};
    status = dcc_endpoint_build_message_body(payload, &body);
    char *path = NULL;
    if (status == DCC_OK) {
        status = dcc_rest_alloc_formatted_path(
            &path, DCC_REST_ROUTE_CHANNEL_MESSAGES,
            (unsigned long long)channel_id
        );
    }
    if (status == DCC_OK) {
        status = dcc_endpoint_submit(
            client, DCC_REST_POST, path, &body, &resolved, out_request
        );
    }
    free(path);
    dcc_endpoint_body_deinit(&body);
    return status;
}
