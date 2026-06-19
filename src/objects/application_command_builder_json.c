#include "internal/objects/dcc_application_command_builder_internal.h"

#include <stdlib.h>

dcc_status_t dcc_application_command_builder_build_json(
    const dcc_application_command_builder_t *builder,
    char **out_json
) {
    if (builder == NULL || out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;

    dcc_application_command_json_buffer_t buffer = {0};
    dcc_status_t status = dcc_application_command_builder_append_json(builder, &buffer);
    if (status != DCC_OK) {
        dcc_command_json_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_build_array_json(
    const dcc_application_command_builder_t *builders,
    size_t builder_count,
    char **out_json
) {
    if (out_json == NULL || (builder_count != 0 && builders == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;

    dcc_application_command_json_buffer_t buffer = {0};
    dcc_status_t status = dcc_command_json_append_cstr(&buffer, "[");
    for (size_t i = 0; status == DCC_OK && i < builder_count; ++i) {
        if (i != 0) {
            status = dcc_command_json_append_cstr(&buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_application_command_builder_append_json(&builders[i], &buffer);
        }
    }
    if (status == DCC_OK) {
        status = dcc_command_json_append_cstr(&buffer, "]");
    }
    if (status != DCC_OK) {
        dcc_command_json_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    return DCC_OK;
}

void dcc_application_command_builder_json_free(char *json) {
    free(json);
}
