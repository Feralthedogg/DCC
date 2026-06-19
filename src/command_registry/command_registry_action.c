#include "internal/command_registry/dcc_command_registry_internal.h"

const char *dcc_command_registry_action_string(dcc_command_registry_diff_action_t action) {
    switch (action) {
        case DCC_COMMAND_REGISTRY_CREATE:
            return "create";
        case DCC_COMMAND_REGISTRY_UPDATE:
            return "update";
        case DCC_COMMAND_REGISTRY_DELETE_STALE:
            return "delete_stale";
        case DCC_COMMAND_REGISTRY_NOOP:
        default:
            return "noop";
    }
}
