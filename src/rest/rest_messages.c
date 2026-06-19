#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_channel_messages(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/messages", (unsigned long long)channel_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_channel_messages_page(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t around,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    uint64_t limit,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    uint64_t capped_limit = limit > 100ULL ? 100ULL : limit;
    dcc_status_t status = dcc_rest_query_append_u64(&query, "after", after);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "around", around);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "before", before);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "limit", capped_limit);
    }
    if (status == DCC_OK) {
        status = dcc_rest_get_channel_messages(client, channel_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_rest_get_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/messages/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)message_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}
