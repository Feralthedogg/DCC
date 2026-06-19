#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_add_guild_member(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *json_body,
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
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_add_guild_member_params(
    dcc_client_t *client,
    const dcc_guild_member_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_guild_member_body(params, 1, &body);
    if (status == DCC_OK) {
        status = dcc_rest_add_guild_member(client, params->guild_id, params->user_id, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_modify_guild_member(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *json_body,
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
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_guild_member_params(
    dcc_client_t *client,
    const dcc_guild_member_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_guild_member_body(params, 0, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild_member(client, params->guild_id, params->user_id, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_remove_guild_member(
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
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
