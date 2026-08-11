#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"

#include <stdlib.h>

static dcc_status_t dcc_message_action_submit(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_rest_call_options_t *resolved,
    dcc_rest_request_t **out_request
) {
    return dcc_endpoint_submit(client, method, path, NULL, resolved, out_request);
}

dcc_status_t dcc_rest_delete_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
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
    if (status == DCC_OK) {
        status = dcc_message_action_submit(
            client, DCC_REST_DELETE, path, &resolved, out_request
        );
    }
    free(path);
    return status;
}

dcc_status_t dcc_rest_crosspost_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U) {
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    }
    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(
        &path, DCC_REST_ROUTE_CHANNEL_MESSAGE_CROSSPOST,
        (unsigned long long)channel_id, (unsigned long long)message_id
    );
    if (status == DCC_OK) {
        status = dcc_message_action_submit(
            client, DCC_REST_POST, path, &resolved, out_request
        );
    }
    free(path);
    return status;
}

dcc_status_t dcc_rest_bulk_delete_messages(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_snowflake_t *message_ids,
    size_t message_count,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U ||
        message_count < 2U || message_count > 100U || message_ids == NULL) {
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    }
    for (size_t i = 0U; i < message_count; ++i) {
        if (message_ids[i] == 0U) return DCC_ERR_INVALID_ARG;
    }
    dcc_rest_buffer_t json = {0};
    status = dcc_rest_buffer_append_cstr(&json, "{\"messages\":[");
    for (size_t i = 0U; status == DCC_OK && i < message_count; ++i) {
        if (i != 0U) status = dcc_rest_buffer_append_cstr(&json, ",");
        if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "\"");
        if (status == DCC_OK) status = dcc_rest_buffer_append_u64_text(&json, message_ids[i]);
        if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "\"");
    }
    if (status == DCC_OK) status = dcc_rest_buffer_append_cstr(&json, "]}");
    char *path = NULL;
    if (status == DCC_OK) {
        status = dcc_rest_alloc_formatted_path(
            &path, DCC_REST_ROUTE_CHANNEL_MESSAGE_BULK_DELETE,
            (unsigned long long)channel_id
        );
    }
    dcc_endpoint_body_t body = {json.data, json.len, "application/json"};
    if (status == DCC_OK) {
        status = dcc_endpoint_submit(
            client, DCC_REST_POST, path, &body, &resolved, out_request
        );
    }
    free(path);
    dcc_rest_buffer_deinit(&json);
    return status;
}
