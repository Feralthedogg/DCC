#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_application_command_builders_internal.h"
#include "internal/rest/dcc_rest_request_core_internal.h"

dcc_status_t dcc_rest_edit_guild_command(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[152];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/guilds/%llu/commands/%llu",
        (unsigned long long)application_id,
        (unsigned long long)guild_id,
        (unsigned long long)command_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_edit_guild_command_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[152];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/applications/%llu/guilds/%llu/commands/%llu",
        (unsigned long long)application_id,
        (unsigned long long)guild_id,
        (unsigned long long)command_id
    );
    return status == DCC_OK
        ? dcc_rest_request_application_command_builder_json(client, DCC_REST_PATCH, path, command, cb, user_data)
        : status;
}
