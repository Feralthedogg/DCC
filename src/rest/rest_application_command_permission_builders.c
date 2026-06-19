#include "internal/rest/dcc_rest_application_command_permission_builders_internal.h"
#include "internal/rest/dcc_rest_application_command_permission_json_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_build_guild_command_permissions_body(
    const dcc_guild_command_permissions_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->application_id == 0 ||
        params->guild_id == 0 ||
        params->command_id == 0 ||
        (params->permission_count != 0 && params->permissions == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"permissions\":");
    if (status == DCC_OK) {
        status = dcc_rest_append_command_permissions_array(&body, params->permissions, params->permission_count);
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

dcc_status_t dcc_rest_build_bulk_guild_command_permissions_body(
    const dcc_bulk_guild_command_permissions_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->application_id == 0 ||
        params->guild_id == 0 ||
        (params->command_count != 0 && params->commands == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "[");
    for (size_t i = 0; i < params->command_count && status == DCC_OK; ++i) {
        const dcc_guild_command_permissions_update_params_t *command = &params->commands[i];
        if (command->command_id == 0 || (command->permission_count != 0 && command->permissions == NULL)) {
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
            status = dcc_rest_json_append_u64_string_member(&body, &first, "id", command->command_id);
        }
        if (status == DCC_OK) {
            status = dcc_rest_json_member_sep(&body, &first);
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\"permissions\":");
        }
        if (status == DCC_OK) {
            status = dcc_rest_append_command_permissions_array(
                &body,
                command->permissions,
                command->permission_count
            );
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
        body.data = NULL;
        body.len = 0;
        body.cap = 0;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
