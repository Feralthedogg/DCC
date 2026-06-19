#include "internal/runtime/dcc_task_group_internal.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

dcc_status_t dcc_task_group_entries_reserve_locked(dcc_task_group_t *group, size_t need) {
    if (need <= group->entry_cap) {
        return DCC_OK;
    }

    size_t next_cap = group->entry_cap != 0U ? group->entry_cap : 4U;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*group->entries)) {
        return DCC_ERR_NOMEM;
    }

    dcc_task_group_entry_t **next = (dcc_task_group_entry_t **)realloc(
        group->entries,
        next_cap * sizeof(*group->entries)
    );
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    group->entries = next;
    group->entry_cap = next_cap;
    return DCC_OK;
}

dcc_status_t dcc_task_group_add_entry_locked(
    dcc_task_group_t *group,
    dcc_task_group_entry_t *entry
) {
    if (group == NULL || entry == NULL || group->entry_count == SIZE_MAX) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_task_group_entries_reserve_locked(group, group->entry_count + 1U);
    if (status != DCC_OK) {
        return status;
    }

    entry->owner = group;
    entry->index = group->entry_count;
    entry->completed = 0U;
    entry->completion_order = 0U;
    group->entries[group->entry_count++] = entry;
    return DCC_OK;
}

void dcc_task_group_remove_last_entry_locked(
    dcc_task_group_t *group,
    dcc_task_group_entry_t *entry
) {
    if (group == NULL || entry == NULL || group->entry_count == 0U) {
        return;
    }
    if (group->entries[group->entry_count - 1U] == entry) {
        group->entries[--group->entry_count] = NULL;
    }
}

void dcc_task_group_mark_entry_complete(dcc_task_group_entry_t *entry) {
    if (entry == NULL || entry->owner == NULL) {
        return;
    }

    dcc_task_group_t *group = entry->owner;
    if (dcc_task_group_state_lock(group) != DCC_OK) {
        return;
    }
    if (!entry->completed) {
        entry->completed = 1U;
        if (group->next_completion_order < UINT64_MAX) {
            group->next_completion_order++;
        }
        entry->completion_order = group->next_completion_order;
        if (group->completed_count < group->entry_count) {
            group->completed_count++;
        }
        if (group->first_completed_index == SIZE_MAX) {
            group->first_completed_index = entry->index;
        }
        group->last_completed_index = entry->index;
        dcc_task_group_state_broadcast(group);
    }
    dcc_task_group_state_unlock(group);
}

void dcc_task_group_entry_run(void *arg) {
    dcc_task_group_entry_t *entry = (dcc_task_group_entry_t *)arg;
    if (entry != NULL && entry->fn != NULL) {
        entry->fn(entry->arg);
    }
    dcc_task_group_mark_entry_complete(entry);
}
