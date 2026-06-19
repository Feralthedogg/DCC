#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/rest.h>

#include <stdio.h>
#include <string.h>

dcc_status_t dcc_rest_set_guild_member_timeout(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *communication_disabled_until,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"communication_disabled_until\":");
    if (status == DCC_OK) {
        if (communication_disabled_until != NULL) {
            status = dcc_rest_buffer_append_json_string(&body, communication_disabled_until);
        } else {
            status = dcc_rest_buffer_append_cstr(&body, "null");
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild_member(client, guild_id, user_id, body.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_clear_guild_member_timeout(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_set_guild_member_timeout(client, guild_id, user_id, NULL, cb, user_data);
}

dcc_status_t dcc_rest_move_guild_member(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char body[64];
    if (channel_id != 0) {
        int written = snprintf(body, sizeof(body), "{\"channel_id\":\"%llu\"}", (unsigned long long)channel_id);
        if (written < 0 || (size_t)written >= sizeof(body)) {
            return DCC_ERR_NOMEM;
        }
    } else {
        memcpy(body, "{\"channel_id\":null}", sizeof("{\"channel_id\":null}"));
    }
    return dcc_rest_modify_guild_member(client, guild_id, user_id, body, cb, user_data);
}
