#include "internal/command_registry/dcc_command_registry_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static atomic_bool dcc_command_registry_fail_next_growth;

void dcc_command_registry_test_fail_next_growth(void) {
    atomic_store_explicit(
        &dcc_command_registry_fail_next_growth,
        true,
        memory_order_release
    );
}

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

dcc_status_t dcc_command_registry_add_prepare(
    dcc_command_registry_add_transaction_t *transaction,
    dcc_command_registry_t *registry,
    const dcc_application_command_builder_t *command
) {
    if (transaction == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(transaction, 0, sizeof(*transaction));
    dcc_builder_abi_view_t view;
    if (registry == NULL || command == NULL ||
        dcc_application_command_builder_abi_validate(command, &view) != DCC_OK ||
        dcc_application_command_builder_validate_create(command) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_command_registry_state_t *state = (dcc_command_registry_state_t *)registry->state;
    if (state != NULL && dcc_command_registry_has_duplicate(state, command)) {
        return DCC_ERR_STATE;
    }

    dcc_status_t status = dcc_command_registry_builder_copy(&transaction->command, command);
    if (status != DCC_OK) {
        return status;
    }

    dcc_command_registry_state_t *target = state;
    if (target == NULL) {
        transaction->new_state = (dcc_command_registry_state_t *)calloc(
            1U,
            sizeof(*transaction->new_state)
        );
        if (transaction->new_state == NULL) {
            dcc_command_registry_builder_deinit(&transaction->command);
            return DCC_ERR_NOMEM;
        }
        target = transaction->new_state;
    }
    if (target->entry_count == SIZE_MAX) {
        dcc_command_registry_add_abort(transaction);
        return DCC_ERR_NOMEM;
    }
    if (target->entry_count == target->entry_cap) {
        if (atomic_exchange_explicit(
                &dcc_command_registry_fail_next_growth,
                false,
                memory_order_acq_rel
            )) {
            dcc_command_registry_add_abort(transaction);
            return DCC_ERR_NOMEM;
        }
        if (target->entry_cap > SIZE_MAX / 2U) {
            dcc_command_registry_add_abort(transaction);
            return DCC_ERR_NOMEM;
        }
        size_t next_cap = target->entry_cap != 0U ? target->entry_cap * 2U : 4U;
        if (next_cap > SIZE_MAX / sizeof(*target->entries)) {
            dcc_command_registry_add_abort(transaction);
            return DCC_ERR_NOMEM;
        }
        transaction->staged_entries = (dcc_command_registry_entry_t *)malloc(
            next_cap * sizeof(*state->entries)
        );
        if (transaction->staged_entries == NULL) {
            dcc_command_registry_add_abort(transaction);
            return DCC_ERR_NOMEM;
        }
        if (target->entry_count != 0U) {
            memcpy(
                transaction->staged_entries,
                target->entries,
                target->entry_count * sizeof(*target->entries)
            );
        }
        transaction->staged_entry_cap = next_cap;
    }

    transaction->registry = registry;
    transaction->state = state;
    transaction->prepared = 1U;
    return DCC_OK;
}

void dcc_command_registry_add_commit(
    dcc_command_registry_add_transaction_t *transaction
) {
    if (transaction == NULL || !transaction->prepared) {
        return;
    }
    dcc_command_registry_state_t *state = transaction->state;
    if (state == NULL) {
        state = transaction->new_state;
        transaction->registry->state = state;
        transaction->new_state = NULL;
    }
    if (transaction->staged_entries != NULL) {
        free(state->entries);
        state->entries = transaction->staged_entries;
        state->entry_cap = transaction->staged_entry_cap;
        transaction->staged_entries = NULL;
    }
    if (transaction->registry->size == 0U) {
        transaction->registry->size = sizeof(*transaction->registry);
    }

    dcc_command_registry_entry_t *entry = &state->entries[state->entry_count++];
    memset(entry, 0, sizeof(*entry));
    entry->size = sizeof(*entry);
    entry->command = transaction->command;
    memset(&transaction->command, 0, sizeof(transaction->command));
    transaction->prepared = 0U;
}

void dcc_command_registry_add_abort(
    dcc_command_registry_add_transaction_t *transaction
) {
    if (transaction == NULL) {
        return;
    }
    dcc_command_registry_builder_deinit(&transaction->command);
    free(transaction->staged_entries);
    free(transaction->new_state);
    memset(transaction, 0, sizeof(*transaction));
}

dcc_status_t dcc_command_registry_add_builder(
    dcc_command_registry_t *registry,
    const dcc_application_command_builder_t *command
) {
    dcc_command_registry_add_transaction_t transaction;
    dcc_status_t status = dcc_command_registry_add_prepare(
        &transaction,
        registry,
        command
    );
    if (status == DCC_OK) {
        dcc_command_registry_add_commit(&transaction);
    }
    return status;
}
