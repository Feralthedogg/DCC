#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdlib.h>

static dcc_status_t dcc_pin_submit(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_rest_call_options_t *resolved,
    dcc_rest_request_t **out_request
) {
    return dcc_endpoint_submit(client, method, path, NULL, resolved, out_request);
}

dcc_status_t dcc_rest_pin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(&path, DCC_REST_ROUTE_CHANNEL_MESSAGE_PIN,
        (unsigned long long)channel_id, (unsigned long long)message_id);
    if (status == DCC_OK) status = dcc_pin_submit(
        client, DCC_REST_PUT, path, &resolved, out_request);
    free(path);
    return status;
}

dcc_status_t dcc_rest_unpin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(&path, DCC_REST_ROUTE_CHANNEL_MESSAGE_PIN,
        (unsigned long long)channel_id, (unsigned long long)message_id);
    if (status == DCC_OK) status = dcc_pin_submit(
        client, DCC_REST_DELETE, path, &resolved, out_request);
    free(path);
    return status;
}

dcc_status_t dcc_rest_get_channel_pins(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_pin_page_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    dcc_endpoint_record_view_t view;
    status = dcc_endpoint_pin_page_preflight(query, &view);
    if (status != DCC_OK) return status;
    dcc_rest_buffer_t text = {0};
    if (query != NULL && (view.present & DCC_REST_PIN_PAGE_PRESENT_BEFORE) != 0U)
        status = dcc_rest_query_append_string(&text, "before", query->before);
    if (status == DCC_OK && query != NULL && (view.present & DCC_REST_PIN_PAGE_PRESENT_LIMIT) != 0U)
        status = dcc_rest_query_append_u64_value(&text, "limit", query->limit);
    char *base = NULL;
    char *path = NULL;
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(
        &base, DCC_REST_ROUTE_CHANNEL_MESSAGE_PINS, (unsigned long long)channel_id);
    if (status == DCC_OK) status = dcc_endpoint_path_with_query(base, &text, &path);
    if (status == DCC_OK) status = dcc_pin_submit(
        client, DCC_REST_GET, path, &resolved, out_request);
    free(base);
    free(path);
    dcc_rest_buffer_deinit(&text);
    return status;
}

dcc_status_t dcc_rest_legacy_pin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(&path, DCC_REST_ROUTE_LEGACY_CHANNEL_PIN,
        (unsigned long long)channel_id, (unsigned long long)message_id);
    if (status == DCC_OK) status = dcc_pin_submit(
        client, DCC_REST_PUT, path, &resolved, out_request);
    free(path);
    return status;
}

dcc_status_t dcc_rest_legacy_unpin_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U || message_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(&path, DCC_REST_ROUTE_LEGACY_CHANNEL_PIN,
        (unsigned long long)channel_id, (unsigned long long)message_id);
    if (status == DCC_OK) status = dcc_pin_submit(
        client, DCC_REST_DELETE, path, &resolved, out_request);
    free(path);
    return status;
}

dcc_status_t dcc_rest_get_legacy_channel_pins(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U)
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(&path, DCC_REST_ROUTE_LEGACY_CHANNEL_PINS,
        (unsigned long long)channel_id);
    if (status == DCC_OK) status = dcc_pin_submit(
        client, DCC_REST_GET, path, &resolved, out_request);
    free(path);
    return status;
}
