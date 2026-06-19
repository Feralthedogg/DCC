#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

dcc_status_t dcc_rest_get_public_archived_threads_page(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    uint64_t before_timestamp,
    uint16_t limit,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64(&query, "before", before_timestamp);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "limit", limit);
    }
    if (status == DCC_OK) {
        status = dcc_rest_get_public_archived_threads(client, channel_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_rest_get_private_archived_threads_page(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    uint64_t before_timestamp,
    uint16_t limit,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64(&query, "before", before_timestamp);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "limit", limit);
    }
    if (status == DCC_OK) {
        status = dcc_rest_get_private_archived_threads(client, channel_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_rest_get_joined_private_archived_threads_page(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t before_id,
    uint16_t limit,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64(&query, "before", before_id);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "limit", limit);
    }
    if (status == DCC_OK) {
        status = dcc_rest_get_joined_private_archived_threads(client, channel_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}
