#include "internal/runtime/dcc_task_group_internal.h"

#include <errno.h>
#include <stdlib.h>

dcc_status_t dcc_task_group_spawn(
    dcc_task_group_t *group,
    dcc_task_fn fn,
    void *arg,
    const dcc_task_options_t *options
) {
    if (group == NULL || group->group == NULL || fn == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!dcc_task_group_client_accepts_work(group->client)) {
        return DCC_ERR_STATE;
    }

    llam_spawn_opts_t opts;
    dcc_status_t status = dcc_task_group_build_opts(options, &opts);
    if (status != DCC_OK) {
        return status;
    }

    dcc_task_group_entry_t *entry = (dcc_task_group_entry_t *)calloc(1U, sizeof(*entry));
    if (entry == NULL) {
        return DCC_ERR_NOMEM;
    }
    entry->fn = fn;
    entry->arg = arg;

    status = dcc_task_group_state_lock(group);
    if (status != DCC_OK) {
        free(entry);
        return status;
    }
    status = dcc_task_group_add_entry_locked(group, entry);
    if (status != DCC_OK) {
        dcc_task_group_state_unlock(group);
        free(entry);
        return status;
    }

    llam_task_t *task = llam_task_group_spawn_ex(
        group->group,
        dcc_task_group_entry_run,
        entry,
        &opts,
        LLAM_SPAWN_OPTS_CURRENT_SIZE
    );
    if (task == NULL) {
        status = dcc_task_group_status_from_errno(errno);
        dcc_task_group_remove_last_entry_locked(group, entry);
        dcc_task_group_state_unlock(group);
        free(entry);
        return status;
    }
    dcc_task_group_state_unlock(group);
    group->client->runtime.tasks_spawned++;
    return DCC_OK;
}
