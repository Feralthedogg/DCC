#include "internal/objects/dcc_builder_abi_internal.h"

#include <string.h>

void dcc_application_command_builder_init(dcc_application_command_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
        builder->size = sizeof(*builder);
        builder->version = DCC_APPLICATION_COMMAND_BUILDER_VERSION;
    }
}

void dcc_application_command_option_builder_init(
    dcc_application_command_option_builder_t *builder
) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
        builder->size = sizeof(*builder);
        builder->version = DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION;
    }
}

static int dcc_command_setter_target(
    dcc_application_command_builder_t *builder,
    size_t offset,
    size_t width
) {
    dcc_builder_abi_view_t view;
    return dcc_application_command_builder_abi_validate(builder, &view) == DCC_OK &&
        dcc_builder_abi_field_covered(view.size, offset, width);
}

static void dcc_command_presence(
    dcc_application_command_builder_t *builder,
    uint64_t bit,
    int present
) {
    if (present) {
        builder->present |= bit;
    } else {
        builder->present &= ~bit;
    }
}

dcc_status_t dcc_application_command_builder_set_name(
    dcc_application_command_builder_t *builder,
    const char *name
) {
    if (!dcc_command_setter_target(
            builder, offsetof(dcc_application_command_builder_t, name), sizeof(builder->name)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->name = name;
    dcc_command_presence(builder, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME, name != NULL);
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_description(
    dcc_application_command_builder_t *builder,
    const char *description
) {
    if (!dcc_command_setter_target(
            builder, offsetof(dcc_application_command_builder_t, description),
            sizeof(builder->description)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->description = description;
    dcc_command_presence(
        builder, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION, description != NULL
    );
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_type(
    dcc_application_command_builder_t *builder,
    dcc_application_command_type_t type
) {
    if (!dcc_command_setter_target(
            builder, offsetof(dcc_application_command_builder_t, type), sizeof(builder->type)
        ) ||
        (type != DCC_APPLICATION_COMMAND_CHAT_INPUT &&
         type != DCC_APPLICATION_COMMAND_USER &&
         type != DCC_APPLICATION_COMMAND_MESSAGE &&
         type != DCC_APPLICATION_COMMAND_PRIMARY_ENTRY_POINT)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->type = (uint32_t)type;
    builder->present |= DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE;
    return DCC_OK;
}
