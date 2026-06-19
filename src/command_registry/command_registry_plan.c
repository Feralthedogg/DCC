#include "internal/command_registry/dcc_command_registry_internal.h"

#include <string.h>

dcc_status_t dcc_command_registry_build_plan(
    const dcc_command_registry_t *registry,
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_remote_entry_t *remote,
    size_t remote_count,
    dcc_command_registry_plan_t *out
) {
    if (registry == NULL || out == NULL || (remote_count != 0 && remote == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    memset(out, 0, sizeof(*out));
    out->size = sizeof(*out);
    out->guild_id = dcc_command_registry_options_guild_id(options);
    out->delete_stale = dcc_command_registry_options_delete_stale(options);
    out->dry_run = dcc_command_registry_options_dry_run(options);
    dcc_status_t status = dcc_command_registry_plan_state_init(out);
    if (status != DCC_OK) {
        return status;
    }

    const dcc_command_registry_state_t *state = dcc_command_registry_state_const(registry);
    size_t local_count = state != NULL ? state->entry_count : 0;
    for (size_t i = 0; i < local_count; ++i) {
        const dcc_application_command_builder_t *command = &state->entries[i].command;
        char *local_json = NULL;
        status = dcc_application_command_builder_build_json(command, &local_json);
        if (status != DCC_OK) {
            dcc_command_registry_plan_deinit(out);
            return status;
        }

        size_t remote_index = dcc_command_registry_find_remote(remote, remote_count, command);
        if (remote_index == SIZE_MAX) {
            status = dcc_command_registry_plan_push(
                out,
                DCC_COMMAND_REGISTRY_CREATE,
                command->name,
                dcc_command_registry_builder_type(command),
                0,
                i,
                SIZE_MAX,
                local_json,
                NULL
            );
        } else {
            const char *remote_json = dcc_command_registry_remote_json(&remote[remote_index]);
            dcc_command_registry_diff_action_t action =
                remote_json != NULL && strcmp(local_json, remote_json) == 0
                    ? DCC_COMMAND_REGISTRY_NOOP
                    : DCC_COMMAND_REGISTRY_UPDATE;
            status = dcc_command_registry_plan_push(
                out,
                action,
                command->name,
                dcc_command_registry_builder_type(command),
                dcc_command_registry_remote_id(&remote[remote_index]),
                i,
                remote_index,
                local_json,
                remote_json
            );
        }
        if (status != DCC_OK) {
            dcc_command_registry_plan_deinit(out);
            return status;
        }
    }

    for (size_t i = 0; i < remote_count; ++i) {
        if (dcc_command_registry_remote_has_local(state, &remote[i])) {
            continue;
        }
        const char *name = dcc_command_registry_remote_name(&remote[i]);
        status = dcc_command_registry_plan_push(
            out,
            out->delete_stale ? DCC_COMMAND_REGISTRY_DELETE_STALE : DCC_COMMAND_REGISTRY_NOOP,
            name != NULL ? name : "",
            dcc_command_registry_remote_type(&remote[i]),
            dcc_command_registry_remote_id(&remote[i]),
            SIZE_MAX,
            i,
            NULL,
            dcc_command_registry_remote_json(&remote[i])
        );
        if (status != DCC_OK) {
            dcc_command_registry_plan_deinit(out);
            return status;
        }
    }

    return DCC_OK;
}
