#include <dcc/application_command.h>

#include <string.h>

void dcc_application_command_builder_init(dcc_application_command_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
    }
}

dcc_status_t dcc_application_command_builder_set_name(
    dcc_application_command_builder_t *builder,
    const char *name
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->name = name;
    builder->has_name = name != NULL;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_description(
    dcc_application_command_builder_t *builder,
    const char *description
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->description = description;
    builder->has_description = description != NULL;
    return DCC_OK;
}

dcc_status_t dcc_application_command_builder_set_type(
    dcc_application_command_builder_t *builder,
    dcc_application_command_type_t type
) {
    if (builder == NULL ||
        (type != DCC_APPLICATION_COMMAND_CHAT_INPUT &&
         type != DCC_APPLICATION_COMMAND_USER &&
         type != DCC_APPLICATION_COMMAND_MESSAGE &&
         type != DCC_APPLICATION_COMMAND_PRIMARY_ENTRY_POINT)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->type = (uint32_t)type;
    builder->has_type = 1;
    return DCC_OK;
}
