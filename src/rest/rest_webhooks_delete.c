#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_delete_webhook(
    dcc_client_t *client, dcc_snowflake_t webhook_id,
    const char *webhook_token, const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_SENSITIVE;
    (void)DCC_ENDPOINT_ROUTE_KEY_OPAQUE;
    DCC_ENDPOINT_SENSITIVE_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_WEBHOOK_TOKEN_OR_BOT, DCC_ENDPOINT_AUDIT_REASON_ALLOWED,
        DCC_REST_ROUTE_WEBHOOK_OPTIONAL_TOKEN_NO_TOKEN, DCC_REST_DELETE, "dcc_rest_delete_webhook");
    (void)DCC_REST_ROUTE_WEBHOOK_OPTIONAL_TOKEN_WITH_TOKEN;
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || webhook_id == 0U ||
        (webhook_token != NULL && webhook_token[0] == '\0'))
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *token = NULL;
    char *path = NULL;
    if (webhook_token != NULL) status = dcc_rest_escape_path_segment(webhook_token, &token);
    if (status == DCC_OK) {
        status = token == NULL
            ? dcc_rest_alloc_formatted_path(&path,
                DCC_REST_ROUTE_WEBHOOK_OPTIONAL_TOKEN_NO_TOKEN,
                (unsigned long long)webhook_id)
            : dcc_rest_alloc_formatted_path(&path,
                DCC_REST_ROUTE_WEBHOOK_OPTIONAL_TOKEN_WITH_TOKEN,
                (unsigned long long)webhook_id, token);
    }
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_DELETE, path, NULL, &resolved, out_request);
    free(token);
    free(path);
    return status;
}
