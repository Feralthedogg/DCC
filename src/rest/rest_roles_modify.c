#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_role_body_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_modify_guild_role(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t role_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/roles/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)role_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_guild_role_params(
    dcc_client_t *client,
    const dcc_role_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_role_body(params, 1, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild_role(client, params->guild_id, params->role_id, body, cb, user_data);
    }
    free(body);
    return status;
}
