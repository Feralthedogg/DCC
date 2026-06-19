#include "internal/objects/dcc_application_command_builder_internal.h"

dcc_status_t dcc_application_command_builder_append_json(
    const dcc_application_command_builder_t *builder,
    dcc_application_command_json_buffer_t *buffer
) {
    if (builder == NULL ||
        buffer == NULL ||
        (builder->has_name && builder->name == NULL) ||
        (builder->has_description && builder->description == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_command_json_append_cstr(buffer, "{");
    if (status != DCC_OK) {
        return status;
    }

    int first = 1;
    status = dcc_application_command_builder_append_identity_json(builder, buffer, &first);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_application_command_builder_append_payload_json(builder, buffer, &first);
    if (status != DCC_OK) {
        return status;
    }

    return dcc_command_json_append_cstr(buffer, "}");
}
