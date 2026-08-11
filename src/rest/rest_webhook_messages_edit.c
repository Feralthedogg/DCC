#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_modify_webhook_message(
    dcc_client_t *client, dcc_snowflake_t webhook_id,
    const char *webhook_token, dcc_snowflake_t message_id,
    const dcc_rest_webhook_message_edit_t *edit,
    const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || webhook_id == 0U || message_id == 0U ||
        webhook_token == NULL || webhook_token[0] == '\0')
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_endpoint_webhook_edit_view_t view;
    status = dcc_endpoint_webhook_message_edit_preflight(edit, &view);
    if (status != DCC_OK) return status;
    dcc_endpoint_body_t body = {0};
    status = dcc_endpoint_build_message_body(view.payload, &body);
    dcc_rest_buffer_t query = {0};
    if (status == DCC_OK && (view.record.present & DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_THREAD_ID) != 0U)
        status = dcc_rest_query_append_u64_value(&query, "thread_id", view.thread_id);
    if (status == DCC_OK && (view.record.present & DCC_REST_WEBHOOK_MESSAGE_EDIT_PRESENT_WITH_COMPONENTS) != 0U)
        status = dcc_rest_query_append_bool(&query, "with_components", view.with_components);
    char *token = NULL;
    char *base = NULL;
    char *path = NULL;
    if (status == DCC_OK) status = dcc_rest_escape_path_segment(webhook_token, &token);
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(
        &base, DCC_REST_ROUTE_WEBHOOK_MESSAGE,
        (unsigned long long)webhook_id, token, (unsigned long long)message_id);
    if (status == DCC_OK) status = dcc_endpoint_path_with_query(base, &query, &path);
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_PATCH, path, &body, &resolved, out_request);
    free(token);
    free(base);
    free(path);
    dcc_rest_buffer_deinit(&query);
    dcc_endpoint_body_deinit(&body);
    return status;
}
