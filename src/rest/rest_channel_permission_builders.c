#include "internal/rest/dcc_rest_channel_permission_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_build_channel_permission_overwrite_body(
    const dcc_channel_permission_overwrite_t *overwrite,
    uint8_t include_id,
    dcc_rest_buffer_t *body
) {
    if (overwrite == NULL ||
        body == NULL ||
        (include_id != 0 && overwrite->id == 0) ||
        overwrite->type > DCC_CHANNEL_OVERWRITE_MEMBER) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    int first = 1;
    if (status == DCC_OK && include_id != 0) {
        status = dcc_rest_json_append_u64_string_member(body, &first, "id", overwrite->id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(body, &first, "allow", overwrite->allow);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(body, &first, "deny", overwrite->deny);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(body, &first, "type", overwrite->type);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

dcc_status_t dcc_rest_build_channel_permission_overwrites_array(
    const dcc_channel_permission_overwrite_t *overwrites,
    size_t overwrite_count,
    dcc_rest_buffer_t *body
) {
    if (body == NULL || (overwrite_count != 0 && overwrites == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "[");
    for (size_t i = 0; i < overwrite_count && status == DCC_OK; ++i) {
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_build_channel_permission_overwrite_body(&overwrites[i], 1, body);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "]");
    }
    return status;
}
