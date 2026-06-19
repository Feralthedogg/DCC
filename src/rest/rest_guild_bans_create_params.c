#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/rest/guilds/bans.h>

#include <stdlib.h>

static dcc_status_t dcc_rest_build_guild_ban_body(uint32_t delete_message_seconds, char **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    uint32_t capped_seconds = delete_message_seconds > 604800U ? 604800U : delete_message_seconds;
    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    if (status == DCC_OK && capped_seconds != 0) {
        status = dcc_rest_buffer_append_cstr(&body, "\"delete_message_seconds\":");
    }
    if (status == DCC_OK && capped_seconds != 0) {
        status = dcc_rest_buffer_append_u64_text(&body, capped_seconds);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out = body.data;
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_create_guild_ban_seconds(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    uint32_t delete_message_seconds,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_guild_ban_body(delete_message_seconds, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_guild_ban(client, guild_id, user_id, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_create_guild_ban_params(
    dcc_client_t *client,
    const dcc_guild_ban_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL || params->size < sizeof(*params) || params->guild_id == 0 || params->user_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_create_guild_ban_seconds(
        client,
        params->guild_id,
        params->user_id,
        params->delete_message_seconds,
        cb,
        user_data
    );
}
