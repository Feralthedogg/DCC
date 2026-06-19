#include "internal/rest/dcc_rest_guild_current_member_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_modify_current_guild_member(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/members/@me", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}
dcc_status_t dcc_rest_modify_current_guild_member_params(
    dcc_client_t *client,
    const dcc_current_guild_member_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_current_guild_member_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_current_guild_member(client, params->guild_id, body, cb, user_data);
    }
    free(body);
    return status;
}
dcc_status_t dcc_rest_set_current_guild_member_nickname(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[96];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/members/@me/nick", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}
dcc_status_t dcc_rest_set_current_guild_member_nickname_params(
    dcc_client_t *client,
    const dcc_guild_member_nickname_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL || params->size < sizeof(*params) || params->guild_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    char *body = NULL;
    dcc_status_t status = dcc_rest_build_member_nickname_body(params->nick, &body);
    if (status == DCC_OK) {
        status = dcc_rest_set_current_guild_member_nickname(client, params->guild_id, body, cb, user_data);
    }
    free(body);
    return status;
}
