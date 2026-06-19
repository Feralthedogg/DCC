#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_modify_guild_channel_positions(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/channels", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}
static dcc_status_t dcc_rest_build_channel_positions_body(
    const dcc_channel_positions_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->guild_id == 0 ||
        params->position_count == 0 ||
        params->positions == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "[");
    for (size_t i = 0; i < params->position_count && status == DCC_OK; ++i) {
        const dcc_channel_position_t *position = &params->positions[i];
        if (position->channel_id == 0) {
            status = DCC_ERR_INVALID_ARG;
            break;
        }
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(&body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "{");
        }
        int first = 1;
        if (status == DCC_OK) {
            status = dcc_rest_json_append_u64_string_member(&body, &first, "id", position->channel_id);
        }
        if (status == DCC_OK) {
            status = dcc_rest_json_append_u64_member(&body, &first, "position", position->position);
        }
        if (status == DCC_OK && position->parent_id != 0) {
            status = dcc_rest_json_append_u64_string_member(&body, &first, "parent_id", position->parent_id);
        }
        if (status == DCC_OK && position->lock_permissions) {
            status = dcc_rest_json_append_bool_member(&body, &first, "lock_permissions", 1);
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "}");
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "]");
    }
    if (status == DCC_OK) {
        *out = body.data;
        return DCC_OK;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
dcc_status_t dcc_rest_modify_guild_channel_positions_params(
    dcc_client_t *client,
    const dcc_channel_positions_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_channel_positions_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_guild_channel_positions(client, params->guild_id, body, cb, user_data);
    }
    free(body);
    return status;
}
