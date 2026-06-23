#include "internal/command_registry/dcc_command_registry_internal.h"

#include <stdlib.h>
#include <string.h>

static int dcc_command_registry_has_duplicate(
    const dcc_command_registry_state_t *state,
    const dcc_application_command_builder_t *command
) {
    uint32_t type = dcc_command_registry_builder_type(command);
    for (size_t i = 0; i < state->entry_count; ++i) {
        const dcc_application_command_builder_t *existing = &state->entries[i].command;
        if (existing->name != NULL &&
            command->name != NULL &&
            strcmp(existing->name, command->name) == 0 &&
            dcc_command_registry_builder_type(existing) == type) {
            return 1;
        }
    }
    return 0;
}

dcc_status_t dcc_command_registry_add_builder(
    dcc_command_registry_t *registry,
    const dcc_application_command_builder_t *command
) {
    if (registry == NULL ||
        command == NULL ||
        !command->has_name ||
        command->name == NULL ||
        command->name[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_command_registry_state_t *state = dcc_command_registry_state_get(registry, 1);
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    if (dcc_command_registry_has_duplicate(state, command)) {
        return DCC_ERR_STATE;
    }
    if (state->entry_count == state->entry_cap) {
        if (state->entry_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        size_t next_cap = state->entry_cap != 0U ? state->entry_cap * 2U : 4U;
        if (next_cap > SIZE_MAX / sizeof(*state->entries)) {
            return DCC_ERR_NOMEM;
        }
        dcc_command_registry_entry_t *entries = (dcc_command_registry_entry_t *)realloc(
            state->entries,
            next_cap * sizeof(*state->entries)
        );
        if (entries == NULL) {
            return DCC_ERR_NOMEM;
        }
        state->entries = entries;
        state->entry_cap = next_cap;
    }

    dcc_application_command_builder_t command_copy;
    dcc_status_t status = dcc_command_registry_builder_copy(&command_copy, command);
    if (status != DCC_OK) {
        return status;
    }

    dcc_command_registry_entry_t *entry = &state->entries[state->entry_count++];
    memset(entry, 0, sizeof(*entry));
    entry->size = sizeof(*entry);
    entry->command = command_copy;
    return DCC_OK;
}
