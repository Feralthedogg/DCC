#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_get_guild_command_permissions(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[168];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/guilds/%llu/commands/%llu/permissions",
        (unsigned long long)application_id,
        (unsigned long long)guild_id,
        (unsigned long long)command_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_edit_guild_command_permissions(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[168];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/guilds/%llu/commands/%llu/permissions",
        (unsigned long long)application_id,
        (unsigned long long)guild_id,
        (unsigned long long)command_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_all_guild_command_permissions(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[140];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/guilds/%llu/commands/permissions",
        (unsigned long long)application_id,
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_bulk_edit_guild_command_permissions(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[140];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/guilds/%llu/commands/permissions",
        (unsigned long long)application_id,
        (unsigned long long)guild_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, json_body, cb, user_data) : status;
}
