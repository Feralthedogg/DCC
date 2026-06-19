#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_modify_auto_moderation_rule(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t rule_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[136];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/auto-moderation/rules/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)rule_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_auto_moderation_rule_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_auto_moderation_rule_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_auto_moderation_rule_body(params, 1, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_auto_moderation_rule(client, guild_id, params->rule_id, body, cb, user_data);
    }
    free(body);
    return status;
}
