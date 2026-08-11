#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_template_body_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_create_guild_template(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/templates", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_create_guild_template_params(
    dcc_client_t *client,
    const dcc_template_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL || params->size < sizeof(*params) || params->guild_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_template_body(params, DCC_REST_TEMPLATE_BODY_TEMPLATE, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_guild_template(client, params->guild_id, body, cb, user_data);
    }
    free(body);
    return status;
}
