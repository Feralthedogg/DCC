#include "internal/objects/dcc_application_command_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

dcc_status_t dcc_application_command_builder_validate_create(
    const dcc_application_command_builder_t *builder
) {
    dcc_builder_abi_view_t view;
    if (dcc_application_command_builder_abi_validate(builder, &view) != DCC_OK ||
        !dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME) ||
        builder->name == NULL || builder->name[0] == '\0' ||
        (dcc_builder_abi_view_has(
             &view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DEFAULT_MEMBER_PERMISSIONS
         ) && builder->default_member_permissions_null > 1U) ||
        (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DM_PERMISSION) &&
         builder->dm_permission > 1U) ||
        (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NSFW) &&
         builder->nsfw > 1U)) {
        return DCC_ERR_INVALID_ARG;
    }

    uint32_t type = dcc_builder_abi_view_has(
            &view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE
        )
        ? builder->type
        : (uint32_t)DCC_APPLICATION_COMMAND_CHAT_INPUT;
    if (type < (uint32_t)DCC_APPLICATION_COMMAND_CHAT_INPUT ||
        type > (uint32_t)DCC_APPLICATION_COMMAND_PRIMARY_ENTRY_POINT) {
        return DCC_ERR_INVALID_ARG;
    }

    if (type == (uint32_t)DCC_APPLICATION_COMMAND_CHAT_INPUT ||
        type == (uint32_t)DCC_APPLICATION_COMMAND_PRIMARY_ENTRY_POINT) {
        return dcc_builder_abi_view_has(
                   &view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION
               ) && builder->description != NULL && builder->description[0] != '\0'
            ? DCC_OK
            : DCC_ERR_INVALID_ARG;
    }

    return !dcc_builder_abi_view_has(
               &view,
               DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION |
                   DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON |
                   DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON |
                   DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS
           )
        ? DCC_OK
        : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_application_command_builder_append_json(
    const dcc_application_command_builder_t *builder,
    dcc_application_command_json_buffer_t *buffer
) {
    dcc_builder_abi_view_t view;
    if (buffer == NULL ||
        dcc_application_command_builder_abi_validate(builder, &view) != DCC_OK ||
        (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME) &&
            builder->name == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION) &&
            builder->description == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON) &&
            builder->name_localizations_json == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON) &&
            builder->description_localizations_json == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON) &&
            builder->options_json == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS) &&
            builder->options_count != 0U && builder->options == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON) &&
            builder->integration_types_json == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON) &&
            builder->contexts_json == NULL)) {
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
