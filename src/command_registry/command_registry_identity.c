#include "internal/command_registry/dcc_command_registry_internal.h"

uint32_t dcc_command_registry_builder_type(const dcc_application_command_builder_t *command) {
    return command != NULL && command->has_type ? command->type : (uint32_t)DCC_APPLICATION_COMMAND_CHAT_INPUT;
}
