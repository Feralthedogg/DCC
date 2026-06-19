#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

#include <string.h>

dcc_status_t dcc_rest_build_application_role_connection_body(
    const dcc_application_role_connection_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        (params->platform_name != NULL && strlen(params->platform_name) > 50U) ||
        (params->platform_username != NULL && strlen(params->platform_username) > 100U) ||
        (params->metadata_field_count != 0 && params->metadata_fields == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK && params->platform_name != NULL && params->platform_name[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "platform_name", params->platform_name);
    }
    if (status == DCC_OK && params->platform_username != NULL && params->platform_username[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "platform_username", params->platform_username);
    }
    if (status == DCC_OK && params->metadata_field_count != 0) {
        status = dcc_rest_json_member_sep(&body, &first);
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "\"metadata\":{");
        }
        int metadata_first = 1;
        for (size_t i = 0; i < params->metadata_field_count && status == DCC_OK; ++i) {
            const dcc_rest_multipart_field_t *field = &params->metadata_fields[i];
            if (!dcc_rest_application_role_connection_key_valid(field->name) ||
                field->value == NULL ||
                strlen(field->value) > 100U) {
                status = DCC_ERR_INVALID_ARG;
                break;
            }
            status = dcc_rest_json_append_string_member(&body, &metadata_first, field->name, field->value);
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&body, "}");
        }
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
