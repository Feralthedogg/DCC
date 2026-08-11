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
    if (query == NULL) return DCC_OK;
    if (query->size < sizeof(*query) ||
        query->version != DCC_REST_WEBHOOK_MESSAGE_QUERY_VERSION ||
        (query->present & ~DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID) != 0U ||
        ((query->present & DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID) != 0U &&
            query->thread_id == 0U)) return DCC_ERR_INVALID_ARG;
    return (query->present & DCC_REST_WEBHOOK_MESSAGE_QUERY_PRESENT_THREAD_ID) != 0U
        ? dcc_rest_query_append_u64_value(out, "thread_id", query->thread_id)
        : DCC_OK;
}

dcc_status_t dcc_rest_get_webhook_message(
    dcc_client_t *client, dcc_snowflake_t webhook_id,
    const char *webhook_token, dcc_snowflake_t message_id,
    const dcc_rest_webhook_message_query_t *query,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_rest_buffer_t text = {0};
    status = dcc_webhook_message_query(query, &text);
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
