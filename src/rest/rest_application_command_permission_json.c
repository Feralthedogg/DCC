#include "internal/rest/dcc_rest_application_command_permission_json_internal.h"

#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_append_command_permission_object(
    dcc_rest_buffer_t *body,
    const dcc_application_command_permission_t *permission
) {
    if (body == NULL || permission == NULL || permission->id == 0 || permission->type == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(body, &first, "id", permission->id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(body, &first, "type", permission->type);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(body, &first, "permission", permission->permission);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

dcc_status_t dcc_rest_append_command_permissions_array(
    dcc_rest_buffer_t *body,
    const dcc_application_command_permission_t *permissions,
    size_t permission_count
) {
    if (body == NULL || (permission_count != 0 && permissions == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "[");
    for (size_t i = 0; i < permission_count && status == DCC_OK; ++i) {
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_append_command_permission_object(body, &permissions[i]);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "]");
    }
    return status;
}
