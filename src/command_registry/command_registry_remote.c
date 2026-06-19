#include "internal/command_registry/dcc_command_registry_internal.h"

#include <stddef.h>
#include <string.h>

static int dcc_command_registry_remote_has_field(const dcc_command_registry_remote_entry_t *remote, size_t end) {
    return remote != NULL && remote->size >= end;
}

const char *dcc_command_registry_remote_name(const dcc_command_registry_remote_entry_t *remote) {
    return dcc_command_registry_remote_has_field(
        remote,
        offsetof(dcc_command_registry_remote_entry_t, name) + sizeof(remote->name)
    ) ? remote->name : NULL;
}

uint32_t dcc_command_registry_remote_type(const dcc_command_registry_remote_entry_t *remote) {
    return dcc_command_registry_remote_has_field(
        remote,
        offsetof(dcc_command_registry_remote_entry_t, type) + sizeof(remote->type)
    ) ? remote->type : (uint32_t)DCC_APPLICATION_COMMAND_CHAT_INPUT;
}

dcc_snowflake_t dcc_command_registry_remote_id(const dcc_command_registry_remote_entry_t *remote) {
    return dcc_command_registry_remote_has_field(
        remote,
        offsetof(dcc_command_registry_remote_entry_t, id) + sizeof(remote->id)
    ) ? remote->id : 0;
}

const char *dcc_command_registry_remote_json(const dcc_command_registry_remote_entry_t *remote) {
    return dcc_command_registry_remote_has_field(
        remote,
        offsetof(dcc_command_registry_remote_entry_t, canonical_json) + sizeof(remote->canonical_json)
    ) ? remote->canonical_json : NULL;
}

static int dcc_command_registry_remote_matches(
    const dcc_command_registry_remote_entry_t *remote,
    const dcc_application_command_builder_t *command
) {
    const char *name = dcc_command_registry_remote_name(remote);
    return name != NULL &&
        command->name != NULL &&
        strcmp(name, command->name) == 0 &&
        dcc_command_registry_remote_type(remote) == dcc_command_registry_builder_type(command);
}

size_t dcc_command_registry_find_remote(
    const dcc_command_registry_remote_entry_t *remote,
    size_t remote_count,
    const dcc_application_command_builder_t *command
) {
    for (size_t i = 0; i < remote_count; ++i) {
        if (dcc_command_registry_remote_matches(&remote[i], command)) {
            return i;
        }
    }
    return SIZE_MAX;
}

int dcc_command_registry_remote_has_local(
    const dcc_command_registry_state_t *state,
    const dcc_command_registry_remote_entry_t *remote
) {
    if (state == NULL) {
        return 0;
    }
    const char *remote_name = dcc_command_registry_remote_name(remote);
    uint32_t remote_type = dcc_command_registry_remote_type(remote);
    if (remote_name == NULL) {
        return 0;
    }
    for (size_t i = 0; i < state->entry_count; ++i) {
        const dcc_application_command_builder_t *command = &state->entries[i].command;
        if (command->name != NULL &&
            strcmp(command->name, remote_name) == 0 &&
            dcc_command_registry_builder_type(command) == remote_type) {
            return 1;
        }
    }
    return 0;
}
