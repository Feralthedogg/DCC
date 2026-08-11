#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdlib.h>

static dcc_status_t dcc_webhook_message_base(
    char **out, const char *route, dcc_snowflake_t webhook_id,
    const char *webhook_token, dcc_snowflake_t message_id
) {
    if (webhook_id == 0U || message_id == 0U || webhook_token == NULL ||
        webhook_token[0] == '\0') return DCC_ERR_INVALID_ARG;
    char *token = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(webhook_token, &token);
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(
        out, route, (unsigned long long)webhook_id, token,
        (unsigned long long)message_id);
    free(token);
    return status;
}

static dcc_status_t dcc_webhook_message_query(
    const dcc_rest_webhook_message_query_t *query,
    dcc_rest_buffer_t *out
) {
    dcc_endpoint_record_view_t view;
    dcc_status_t status = dcc_endpoint_webhook_message_query_preflight(
        query, &view
    );
    if (status != DCC_OK || query == NULL) return status;
    return (view.present & DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID) != 0U
        ? dcc_rest_query_append_u64_value(out, "thread_id", query->thread_id)
        : DCC_OK;
}

dcc_status_t dcc_rest_get_webhook_message(
    dcc_client_t *client, dcc_snowflake_t webhook_id,
    const char *webhook_token, dcc_snowflake_t message_id,
    const dcc_rest_webhook_message_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_SENSITIVE;
    (void)DCC_ENDPOINT_ROUTE_KEY_OPAQUE;
    DCC_ENDPOINT_SENSITIVE_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_NONE, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_WEBHOOK_MESSAGE, DCC_REST_GET, "dcc_rest_get_webhook_message");
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || webhook_id == 0U ||
        message_id == 0U || webhook_token == NULL || webhook_token[0] == '\0')
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_endpoint_record_view_t view;
    status = dcc_endpoint_webhook_message_query_preflight(query, &view);
    if (status != DCC_OK) return status;
    dcc_rest_buffer_t text = {0};
    status = dcc_endpoint_allocation_probe();
    if (status == DCC_OK) status = dcc_webhook_message_query(query, &text);
    char *base = NULL;
    char *path = NULL;
    if (status == DCC_OK) status = dcc_webhook_message_base(&base,
        DCC_REST_ROUTE_WEBHOOK_MESSAGE, webhook_id, webhook_token, message_id);
    if (status == DCC_OK) status = dcc_endpoint_path_with_query(base, &text, &path);
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_GET, path, NULL, &resolved, out_request);
    free(base);
    free(path);
    dcc_rest_buffer_deinit(&text);
    return status;
}
