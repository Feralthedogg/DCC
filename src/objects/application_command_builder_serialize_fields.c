#include "internal/objects/dcc_application_command_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

dcc_status_t dcc_application_command_builder_append_identity_json(
    const dcc_application_command_builder_t *builder,
    dcc_application_command_json_buffer_t *buffer,
    int *first
) {
    dcc_builder_abi_view_t view;
    if (dcc_application_command_builder_abi_validate(builder, &view) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = DCC_OK;
    if (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME)) {
        status = dcc_command_json_append_string_member(buffer, first, "name", builder->name);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON
        )) {
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
    if (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION)) {
        status = dcc_command_json_append_string_member(buffer, first, "description", builder->description);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON
        )) {
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
    if (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE)) {
        status = dcc_command_json_member_prefix(buffer, first, "type");
        if (status == DCC_OK) {
            status = dcc_command_json_append_u32(buffer, builder->type);
        }
    }
    return status;
}
