#ifndef DCC_TASK_GROUP_SYNC_INTERNAL_H
#define DCC_TASK_GROUP_SYNC_INTERNAL_H

#include "internal/runtime/dcc_task_group_state_internal.h"

#include <dcc/error.h>
#include <dcc/tasks.h>

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_WIN32)
dcc_status_t dcc_task_group_make_deadline_ms(uint64_t wait_ms, struct timespec *out);
#else
void dcc_task_group_sleep_poll(void);
#endif

dcc_status_t dcc_task_group_state_lock(dcc_task_group_t *group);
void dcc_task_group_state_unlock(dcc_task_group_t *group);
void dcc_task_group_state_broadcast(dcc_task_group_t *group);
dcc_status_t dcc_task_group_wait_all_entries(dcc_task_group_t *group, uint32_t timeout_ms);
void dcc_task_group_snapshot_locked(const dcc_task_group_t *group, dcc_task_group_wait_result_t *out);

#ifdef __cplusplus
}
#endif

#endif
