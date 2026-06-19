#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

#include <string.h>

dcc_status_t dcc_rest_build_application_role_connection_metadata_body(
    const dcc_application_role_connection_metadata_params_t *metadata,
    size_t metadata_count,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (metadata_count != 0 && metadata == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "[");
    for (size_t i = 0; i < metadata_count && status == DCC_OK; ++i) {
        const dcc_application_role_connection_metadata_params_t *item = &metadata[i];
        if (item->size < sizeof(*item) ||
            item->type < DCC_APPLICATION_ROLE_CONNECTION_METADATA_INTEGER_LESS_THAN_OR_EQUAL ||
            item->type > DCC_APPLICATION_ROLE_CONNECTION_METADATA_BOOLEAN_NOT_EQUAL ||
            !dcc_rest_application_role_connection_key_valid(item->key) ||
            item->name == NULL ||
            item->name[0] == '\0' ||
            strlen(item->name) > 100U ||
            item->description == NULL ||
            item->description[0] == '\0' ||
            strlen(item->description) > 200U) {
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
            status = dcc_rest_json_append_u64_member(&body, &first, "type", (uint64_t)item->type);
        }
        if (status == DCC_OK) {
            status = dcc_rest_json_append_string_member(&body, &first, "key", item->key);
        }
        if (status == DCC_OK) {
            status = dcc_rest_json_append_string_member(&body, &first, "name", item->name);
        }
        if (status == DCC_OK && item->name_localizations_json != NULL && item->name_localizations_json[0] != '\0') {
            status = dcc_rest_json_append_raw_member(
                &body,
                &first,
                "name_localizations",
                item->name_localizations_json
            );
        }
        if (status == DCC_OK) {
            status = dcc_rest_json_append_string_member(&body, &first, "description", item->description);
        }
        if (status == DCC_OK &&
            item->description_localizations_json != NULL &&
            item->description_localizations_json[0] != '\0') {
            status = dcc_rest_json_append_raw_member(
                &body,
                &first,
                "description_localizations",
                item->description_localizations_json
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
        return DCC_OK;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
