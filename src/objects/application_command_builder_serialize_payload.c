#include "internal/objects/dcc_application_command_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

dcc_status_t dcc_application_command_builder_append_payload_json(
    const dcc_application_command_builder_t *builder,
    dcc_application_command_json_buffer_t *buffer,
    int *first
) {
    dcc_builder_abi_view_t view;
    if (dcc_application_command_builder_abi_validate(builder, &view) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
#define HAS(bit_) dcc_builder_abi_view_has(&view, (bit_))
    dcc_status_t status = DCC_OK;
    if (HAS(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON)) {
        status = dcc_command_json_append_raw_member(buffer, first, "options", builder->options_json);
        if (status != DCC_OK) {
            return status;
        }
    } else if (HAS(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS) &&
        builder->options_count != 0U) {
        if (builder->options == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        status = dcc_command_json_member_prefix(buffer, first, "options");
        if (status == DCC_OK) {
            status = dcc_application_command_option_builder_append_array_json(
                builder->options,
                builder->options_count,
                buffer
            );
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DEFAULT_MEMBER_PERMISSIONS)) {
        status = dcc_command_json_member_prefix(buffer, first, "default_member_permissions");
        if (status == DCC_OK) {
            status = builder->default_member_permissions_null
                ? dcc_command_json_append_cstr(buffer, "null")
                : dcc_command_json_append_u64_string(buffer, builder->default_member_permissions);
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DM_PERMISSION)) {
        status = dcc_command_json_member_prefix(buffer, first, "dm_permission");
        if (status == DCC_OK) {
            status = dcc_command_json_append_cstr(buffer, builder->dm_permission ? "true" : "false");
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NSFW)) {
        status = dcc_command_json_member_prefix(buffer, first, "nsfw");
        if (status == DCC_OK) {
            status = dcc_command_json_append_cstr(buffer, builder->nsfw ? "true" : "false");
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON)) {
        status = dcc_command_json_append_raw_member(
            buffer,
            first,
            "integration_types",
            builder->integration_types_json
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON)) {
        status = dcc_command_json_append_raw_member(buffer, first, "contexts", builder->contexts_json);
    }
    #undef HAS
    return status;
}
