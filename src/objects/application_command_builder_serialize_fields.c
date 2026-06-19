#include "internal/objects/dcc_application_command_builder_internal.h"

dcc_status_t dcc_application_command_builder_append_identity_json(
    const dcc_application_command_builder_t *builder,
    dcc_application_command_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = DCC_OK;
    if (builder->has_name) {
        status = dcc_command_json_append_string_member(buffer, first, "name", builder->name);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->name_localizations_json != NULL) {
        status = dcc_command_json_append_raw_member(
            buffer,
            first,
            "name_localizations",
            builder->name_localizations_json
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_description) {
        status = dcc_command_json_append_string_member(buffer, first, "description", builder->description);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->description_localizations_json != NULL) {
        status = dcc_command_json_append_raw_member(
            buffer,
            first,
            "description_localizations",
            builder->description_localizations_json
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_type) {
        status = dcc_command_json_member_prefix(buffer, first, "type");
        if (status == DCC_OK) {
            status = dcc_command_json_append_u32(buffer, builder->type);
        }
    }
    return status;
}
