#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdlib.h>

static dcc_status_t dcc_message_list_query_build(
    const dcc_rest_message_list_query_t *query,
    dcc_rest_buffer_t *out
) {
    dcc_endpoint_record_view_t view;
    dcc_status_t status = dcc_endpoint_message_list_preflight(query, &view);
    if (status != DCC_OK || query == NULL) return status;
    if ((view.present & DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AROUND) != 0U)
        status = dcc_rest_query_append_u64_value(out, "around", query->around);
    if (status == DCC_OK && (view.present & DCC_REST_MESSAGE_LIST_QUERY_PRESENT_BEFORE) != 0U)
        status = dcc_rest_query_append_u64_value(out, "before", query->before);
    if (status == DCC_OK && (view.present & DCC_REST_MESSAGE_LIST_QUERY_PRESENT_AFTER) != 0U)
        status = dcc_rest_query_append_u64_value(out, "after", query->after);
    if (status == DCC_OK && (view.present & DCC_REST_MESSAGE_LIST_QUERY_PRESENT_LIMIT) != 0U)
        status = dcc_rest_query_append_u64_value(out, "limit", query->limit);
    return status;
}

dcc_status_t dcc_rest_get_channel_messages(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_message_list_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_CHANNEL_MESSAGES, DCC_REST_GET);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U) {
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    }
    dcc_endpoint_record_view_t view;
    status = dcc_endpoint_message_list_preflight(query, &view);
    if (status != DCC_OK) return status;
    dcc_rest_buffer_t query_text = {0};
    status = dcc_message_list_query_build(query, &query_text);
    char *base = NULL;
    char *path = NULL;
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(
        &base, DCC_REST_ROUTE_CHANNEL_MESSAGES, (unsigned long long)channel_id
    );
    if (status == DCC_OK) status = dcc_endpoint_path_with_query(base, &query_text, &path);
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_GET, path, NULL, &resolved, out_request
    );
    free(base);
    free(path);
    dcc_rest_buffer_deinit(&query_text);
    return status;
}

dcc_status_t dcc_rest_get_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    (void)DCC_ENDPOINT_PATH_PUBLIC;
    DCC_ENDPOINT_CONTRACT(DCC_ENDPOINT_AUTH_POLICY_BOT, DCC_ENDPOINT_AUDIT_REASON_DENIED,
        DCC_REST_ROUTE_CHANNEL_MESSAGE, DCC_REST_GET);
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U) {
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    }
    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(
        &path, DCC_REST_ROUTE_CHANNEL_MESSAGE,
        (unsigned long long)channel_id, (unsigned long long)message_id
    );
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_GET, path, NULL, &resolved, out_request
    );
    free(path);
    return status;
}
