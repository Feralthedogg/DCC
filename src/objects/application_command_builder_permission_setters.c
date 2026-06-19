#include <dcc/application_command.h>

dcc_status_t dcc_application_command_builder_set_default_member_permissions(
    dcc_application_command_builder_t *builder,
    uint64_t permissions
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->default_member_permissions = permissions;
    builder->default_member_permissions_null = 0;
    builder->has_default_member_permissions = 1;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_default_member_permissions_null(
    dcc_application_command_builder_t *builder
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->default_member_permissions = 0;
    builder->default_member_permissions_null = 1;
    builder->has_default_member_permissions = 1;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_dm_permission(
    dcc_application_command_builder_t *builder,
    uint8_t dm_permission
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->dm_permission = dm_permission ? 1U : 0U;
    builder->has_dm_permission = 1;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_nsfw(
    dcc_application_command_builder_t *builder,
    uint8_t nsfw
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->nsfw = nsfw ? 1U : 0U;
    builder->has_nsfw = 1;
    return DCC_OK;
}
