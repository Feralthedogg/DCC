#include "internal/command_registry/dcc_command_registry_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

uint32_t dcc_command_registry_builder_type(const dcc_application_command_builder_t *command) {
    dcc_builder_abi_view_t view;
    if (dcc_application_command_builder_abi_validate(command, &view) != DCC_OK ||
        !dcc_builder_abi_view_has(&view, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE)) {
        return (uint32_t)DCC_APPLICATION_COMMAND_CHAT_INPUT;
    }
    return command->type;
}
