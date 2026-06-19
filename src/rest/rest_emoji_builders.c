#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_build_emoji_body(
    const dcc_emoji_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->name == NULL ||
        params->name[0] == '\0' ||
        (params->role_count != 0 && params->roles == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
    }
    if (status == DCC_OK && params->image != NULL && params->image[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "image", params->image);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_member_sep(&body, &first);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "\"roles\":[");
    }
    for (size_t i = 0; i < params->role_count && status == DCC_OK; ++i) {
        if (params->roles[i] == 0) {
            status = DCC_ERR_INVALID_ARG;
            break;
        }
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(&body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\"");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_u64_text(&body, params->roles[i]);
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\"");
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "]}");
    }
    if (status == DCC_OK) {
        *out = body.data;
        return DCC_OK;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
