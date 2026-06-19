#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_modify_guild_role_positions(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/roles", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_guild_role_positions_params(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_role_position_t *positions,
    size_t position_count,
    dcc_rest_cb cb,
    void *user_data
) {
    if (guild_id == 0 || positions == NULL || position_count == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "[");
    for (size_t i = 0; i < position_count && status == DCC_OK; ++i) {
        if (positions[i].role_id == 0) {
            status = DCC_ERR_INVALID_ARG;
            break;
        }
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(&body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "{\"id\":\"");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_u64_text(&body, positions[i].role_id);
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\",\"position\":");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_u64_text(&body, positions[i].position);
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "}");
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "]");
    }
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild_role_positions(client, guild_id, body.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
