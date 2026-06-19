#include "internal/rest/dcc_rest_guild_body_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_guild(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[64];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}
dcc_status_t dcc_rest_modify_guild(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[64];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}
dcc_status_t dcc_rest_create_guild(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method(client, DCC_REST_POST, "/guilds", json_body, cb, user_data);
}
dcc_status_t dcc_rest_create_guild_params(
    dcc_client_t *client,
    const dcc_guild_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_guild_body(params, 0, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_guild(client, body, cb, user_data);
    }
    free(body);
    return status;
}
dcc_status_t dcc_rest_modify_guild_params(
    dcc_client_t *client,
    const dcc_guild_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_guild_body(params, 1, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild(client, params->guild_id, body, cb, user_data);
    }
    free(body);
    return status;
}
dcc_status_t dcc_rest_delete_guild(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[64];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
dcc_status_t dcc_rest_get_guild_preview(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/preview", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}
