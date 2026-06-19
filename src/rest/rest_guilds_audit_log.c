#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_guild_audit_log(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/audit-logs", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_guild_audit_log_page(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    uint32_t action_type,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    uint32_t limit,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64(&query, "user_id", user_id);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "action_type", action_type);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "before", before);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "after", after);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "limit", limit);
    }
    if (status == DCC_OK) {
        status = dcc_rest_get_guild_audit_log(client, guild_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}
