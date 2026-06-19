#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_guild_integrations(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/integrations", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}
dcc_status_t dcc_rest_modify_guild_integration(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/integrations/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)integration_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}
dcc_status_t dcc_rest_delete_guild_integration(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/integrations/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)integration_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
dcc_status_t dcc_rest_sync_guild_integration(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t integration_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[128];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/integrations/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)integration_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, NULL, cb, user_data) : status;
}
