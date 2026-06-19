#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_role_body_internal.h"

#include <string.h>

dcc_status_t dcc_rest_build_role_body(const dcc_role_params_t *params, uint8_t include_id, char **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->guild_id == 0 ||
        (include_id != 0 && params->role_id == 0) ||
        (params->name != NULL && strlen(params->name) > 100U)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK && include_id != 0) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "id", params->role_id);
    }
    if (status == DCC_OK && params->name != NULL && params->name[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
    }
    if (status == DCC_OK && params->color != 0U) {
        status = dcc_rest_json_append_u64_member(&body, &first, "color", params->color);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(&body, &first, "position", params->position);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "permissions", params->permissions);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(&body, &first, "hoist", params->hoist);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(&body, &first, "mentionable", params->mentionable);
    }
    if (status == DCC_OK && params->icon != NULL && params->icon[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "icon", params->icon);
    }
    if (status == DCC_OK && params->unicode_emoji != NULL && params->unicode_emoji[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "unicode_emoji", params->unicode_emoji);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out = body.data;
        return DCC_OK;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
