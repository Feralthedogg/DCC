#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_user(
    dcc_client_t *client,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[64];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/users/%llu", (unsigned long long)user_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_current_user_connections(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method(client, DCC_REST_GET, "/users/@me/connections", NULL, cb, user_data);
}

dcc_status_t dcc_rest_get_current_user_guilds(
    dcc_client_t *client,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_with_query(client, DCC_REST_GET, "/users/@me/guilds", query, NULL, cb, user_data);
}

dcc_status_t dcc_rest_leave_guild(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/users/@me/guilds/%llu", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
