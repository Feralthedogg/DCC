#include "internal/rest/dcc_rest_builders_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_create_guild_emoji_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_emoji_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_emoji_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_guild_emoji(client, guild_id, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_modify_guild_emoji_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_emoji_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL || params->size < sizeof(*params) || params->emoji_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    char *body = NULL;
    dcc_status_t status = dcc_rest_build_emoji_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild_emoji(client, guild_id, params->emoji_id, body, cb, user_data);
    }
    free(body);
    return status;
}
