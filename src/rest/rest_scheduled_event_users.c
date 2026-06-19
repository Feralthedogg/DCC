#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdio.h>

dcc_status_t dcc_rest_get_guild_scheduled_event_users(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[136];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/scheduled-events/%llu/users",
        (unsigned long long)guild_id,
        (unsigned long long)event_id
    );
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_guild_scheduled_event_users_page(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    uint8_t limit,
    dcc_snowflake_t before,
    dcc_snowflake_t after,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    uint64_t effective_limit = limit != 0 ? (uint64_t)limit : 100ULL;
    char limit_text[4];
    int written = snprintf(limit_text, sizeof(limit_text), "%u", (unsigned)effective_limit);
    if (written < 0 || (size_t)written >= sizeof(limit_text)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(&query, "with_member=true&limit=");
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&query, limit_text);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "after", after);
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_u64(&query, "before", before);
    }
    if (status == DCC_OK) {
        status = dcc_rest_get_guild_scheduled_event_users(client, guild_id, event_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}
