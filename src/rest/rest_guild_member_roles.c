#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_add_guild_member_role(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[144];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/members/%llu/roles/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)user_id,
        (unsigned long long)role_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_add_guild_member_role_params(
    dcc_client_t *client,
    const dcc_guild_member_role_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->guild_id == 0 ||
        params->user_id == 0 ||
        params->role_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_add_guild_member_role(
        client,
        params->guild_id,
        params->user_id,
        params->role_id,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_remove_guild_member_role(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t role_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[144];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/members/%llu/roles/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)user_id,
        (unsigned long long)role_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
