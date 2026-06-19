#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_guild_scheduled_events(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/scheduled-events", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_guild_scheduled_events_with_user_count(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_get_guild_scheduled_events(client, guild_id, "with_user_count=true", cb, user_data);
}

dcc_status_t dcc_rest_get_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/scheduled-events/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)event_id
    );
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_guild_scheduled_event_with_user_count(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_get_guild_scheduled_event(client, guild_id, event_id, "with_user_count=true", cb, user_data);
}

dcc_status_t dcc_rest_create_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/scheduled-events", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/scheduled-events/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)event_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_delete_guild_scheduled_event(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t event_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/scheduled-events/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)event_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
