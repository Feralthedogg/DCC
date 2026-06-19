#ifndef DCC_TASK_GROUP_ENTRIES_INTERNAL_H
#define DCC_TASK_GROUP_ENTRIES_INTERNAL_H

#include "internal/runtime/dcc_task_group_state_internal.h"

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_task_group_entries_reserve_locked(dcc_task_group_t *group, size_t need);
dcc_status_t dcc_task_group_add_entry_locked(dcc_task_group_t *group, dcc_task_group_entry_t *entry);
void dcc_task_group_remove_last_entry_locked(dcc_task_group_t *group, dcc_task_group_entry_t *entry);
void dcc_task_group_mark_entry_complete(dcc_task_group_entry_t *entry);
void dcc_task_group_entry_run(void *arg);

#ifdef __cplusplus
}
#endif

#endif
