#include "internal/objects/dcc_application_command_builder_internal.h"

dcc_status_t dcc_application_command_builder_append_payload_json(
    const dcc_application_command_builder_t *builder,
    dcc_application_command_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = DCC_OK;
    if (builder->options_json != NULL) {
        status = dcc_command_json_append_raw_member(buffer, first, "options", builder->options_json);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_default_member_permissions) {
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
    if (builder->has_dm_permission) {
        status = dcc_command_json_member_prefix(buffer, first, "dm_permission");
        if (status == DCC_OK) {
            status = dcc_command_json_append_cstr(buffer, builder->dm_permission ? "true" : "false");
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_nsfw) {
        status = dcc_command_json_member_prefix(buffer, first, "nsfw");
        if (status == DCC_OK) {
            status = dcc_command_json_append_cstr(buffer, builder->nsfw ? "true" : "false");
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->integration_types_json != NULL) {
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
    if (builder->contexts_json != NULL) {
        status = dcc_command_json_append_raw_member(buffer, first, "contexts", builder->contexts_json);
    }
    return status;
}
