#include "internal/rest/dcc_rest_endpoint_internal.h"
#include "internal/rest/dcc_rest_endpoint_routes_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_poll_answer_voters(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    uint32_t answer_id,
    const dcc_rest_id_page_t *page,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
) {
    dcc_rest_call_options_t resolved;
    dcc_status_t status = dcc_endpoint_prepare(options, out_request, &resolved);
    if (status != DCC_OK || client == NULL || channel_id == 0U ||
        message_id == 0U || answer_id == 0U) {
        return status != DCC_OK ? status : DCC_ERR_INVALID_ARG;
    }
    dcc_rest_buffer_t query = {0};
    const uint64_t allowed = DCC_REST_ID_PAGE_PRESENT_AFTER |
        DCC_REST_ID_PAGE_PRESENT_LIMIT;
    dcc_endpoint_record_view_t view;
    status = dcc_endpoint_id_page_preflight(page, allowed, &view);
    if (status != DCC_OK) return status;
    if (page != NULL && (view.present & DCC_REST_ID_PAGE_PRESENT_AFTER) != 0U)
        status = dcc_rest_query_append_u64_value(&query, "after", page->after);
    if (status == DCC_OK && page != NULL && (view.present & DCC_REST_ID_PAGE_PRESENT_LIMIT) != 0U)
        status = dcc_rest_query_append_u64_value(&query, "limit", page->limit);
    char *base = NULL;
    char *path = NULL;
    if (status == DCC_OK) status = dcc_rest_alloc_formatted_path(
        &base, DCC_REST_ROUTE_CHANNEL_MESSAGE_POLL_ANSWER,
        (unsigned long long)channel_id, (unsigned long long)message_id,
        (unsigned)answer_id
    );
    if (status == DCC_OK) status = dcc_endpoint_path_with_query(base, &query, &path);
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_GET, path, NULL, &resolved, out_request
    );
    free(base);
    free(path);
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_rest_end_poll(
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
        &path, DCC_REST_ROUTE_CHANNEL_MESSAGE_POLL_EXPIRE,
        (unsigned long long)channel_id, (unsigned long long)message_id
    );
    if (status == DCC_OK) status = dcc_endpoint_submit(
        client, DCC_REST_POST, path, NULL, &resolved, out_request
    );
    free(path);
    return status;
}
