#include "internal/command_registry/dcc_command_registry_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_command_registry_state_t *dcc_command_registry_state_get(
    dcc_command_registry_t *registry,
    int create
) {
    if (registry == NULL) {
        return NULL;
    }
    if (registry->size == 0) {
        dcc_command_registry_init(registry);
    }
    if (registry->state == NULL && create) {
        registry->state = calloc(1, sizeof(dcc_command_registry_state_t));
    }
    return (dcc_command_registry_state_t *)registry->state;
}

const dcc_command_registry_state_t *dcc_command_registry_state_const(
    const dcc_command_registry_t *registry
) {
    if (registry == NULL) {
        return NULL;
    }
    return (const dcc_command_registry_state_t *)registry->state;
}

void dcc_command_registry_init(dcc_command_registry_t *registry) {
    if (registry == NULL) {
        return;
    }
    memset(registry, 0, sizeof(*registry));
    registry->size = sizeof(*registry);
}

dcc_status_t dcc_command_registry_clear(dcc_command_registry_t *registry) {
    dcc_command_registry_state_t *state = dcc_command_registry_state_get(registry, 0);
    if (registry == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (state == NULL) {
        return DCC_OK;
    }
    for (size_t i = 0U; i < state->entry_count; ++i) {
        dcc_command_registry_builder_deinit(&state->entries[i].command);
    }
    free(state->entries);
    state->entries = NULL;
    state->entry_count = 0;
    state->entry_cap = 0;
    return DCC_OK;
}

void dcc_command_registry_deinit(dcc_command_registry_t *registry) {
    if (registry == NULL) {
        return;
    }
    (void)dcc_command_registry_clear(registry);
    free(registry->state);
    registry->state = NULL;
    registry->size = 0;
}

size_t dcc_command_registry_count(const dcc_command_registry_t *registry) {
    const dcc_command_registry_state_t *state = dcc_command_registry_state_const(registry);
    return state != NULL ? state->entry_count : 0;
}
