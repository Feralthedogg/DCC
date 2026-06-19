#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_guild_member(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/members/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)user_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_list_guild_members(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/members", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_list_guild_members_page(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    uint16_t limit,
    dcc_snowflake_t after,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64(&query, "after", after);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "limit", limit);
    }
    if (status == DCC_OK) {
        status = dcc_rest_list_guild_members(client, guild_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}

dcc_status_t dcc_rest_search_guild_members(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/members/search", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_search_guild_members_page(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *search_query,
    uint16_t limit,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64(&query, "limit", limit);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_string(&query, "query", search_query);
    }
    if (status == DCC_OK) {
        status = dcc_rest_search_guild_members(client, guild_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}
