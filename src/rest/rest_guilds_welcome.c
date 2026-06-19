#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_welcome_screen_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_guild_welcome_screen(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/welcome-screen", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}
dcc_status_t dcc_rest_modify_guild_welcome_screen(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/welcome-screen", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}
dcc_status_t dcc_rest_modify_guild_welcome_screen_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_welcome_screen_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_welcome_screen_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild_welcome_screen(client, guild_id, body, cb, user_data);
    }
    free(body);
    return status;
}
