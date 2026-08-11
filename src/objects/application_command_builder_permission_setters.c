#include "internal/objects/dcc_builder_abi_internal.h"

static int dcc_command_permission_target(
    dcc_application_command_builder_t *builder,
    size_t first_offset,
    size_t first_width,
    size_t second_offset,
    size_t second_width
) {
    dcc_builder_abi_view_t view;
    return dcc_application_command_builder_abi_validate(builder, &view) == DCC_OK &&
        dcc_builder_abi_field_covered(view.size, first_offset, first_width) &&
        (second_width == 0U || dcc_builder_abi_field_covered(view.size, second_offset, second_width));
}

dcc_status_t dcc_application_command_builder_set_default_member_permissions(
    dcc_application_command_builder_t *builder,
    uint64_t permissions
) {
    if (!dcc_command_permission_target(
            builder,
            offsetof(dcc_application_command_builder_t, default_member_permissions),
            sizeof(builder->default_member_permissions),
            offsetof(dcc_application_command_builder_t, default_member_permissions_null),
            sizeof(builder->default_member_permissions_null)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->default_member_permissions = permissions;
    builder->default_member_permissions_null = 0;
    builder->present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DEFAULT_MEMBER_PERMISSIONS;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_default_member_permissions_null(
    dcc_application_command_builder_t *builder
) {
    if (!dcc_command_permission_target(
            builder,
            offsetof(dcc_application_command_builder_t, default_member_permissions),
            sizeof(builder->default_member_permissions),
            offsetof(dcc_application_command_builder_t, default_member_permissions_null),
            sizeof(builder->default_member_permissions_null)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->default_member_permissions = 0;
    builder->default_member_permissions_null = 1;
    builder->present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DEFAULT_MEMBER_PERMISSIONS;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_dm_permission(
    dcc_application_command_builder_t *builder,
    uint8_t dm_permission
) {
    if (!dcc_command_permission_target(
            builder, offsetof(dcc_application_command_builder_t, dm_permission),
            sizeof(builder->dm_permission), 0U, 0U
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->dm_permission = dm_permission ? 1U : 0U;
    builder->present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DM_PERMISSION;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_nsfw(
    dcc_application_command_builder_t *builder,
    uint8_t nsfw
) {
    if (!dcc_command_permission_target(
            builder, offsetof(dcc_application_command_builder_t, nsfw),
            sizeof(builder->nsfw), 0U, 0U
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->nsfw = nsfw ? 1U : 0U;
    builder->present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NSFW;
    return DCC_OK;
}
