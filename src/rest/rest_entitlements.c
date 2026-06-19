#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_collections_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_entitlements(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/applications/%llu/entitlements", (unsigned long long)application_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_entitlements_page(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t user_id,
    const dcc_snowflake_t *sku_ids,
    size_t sku_id_count,
    dcc_snowflake_t before_id,
    dcc_snowflake_t after_id,
    uint8_t limit,
    dcc_snowflake_t guild_id,
    uint8_t exclude_ended,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    uint8_t effective_limit = limit != 0 ? limit : 100U;
    dcc_status_t status = dcc_rest_query_append_u64(&query, "user_id", user_id);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_snowflake_csv(&query, "sku_ids", sku_ids, sku_id_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "before_id", before_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "after_id", after_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "limit", effective_limit);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "guild_id", guild_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_bool(&query, "exclude_ended", exclude_ended);
    }
    if (status == DCC_OK) {
        status = dcc_rest_get_entitlements(client, application_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}
