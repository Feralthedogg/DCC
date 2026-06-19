#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_active_threads(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/threads/active", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_public_archived_threads(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/threads/archived/public",
        (unsigned long long)channel_id
    );
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_private_archived_threads(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/threads/archived/private",
        (unsigned long long)channel_id
    );
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_joined_private_archived_threads(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/users/@me/threads/archived/private",
        (unsigned long long)channel_id
    );
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}
