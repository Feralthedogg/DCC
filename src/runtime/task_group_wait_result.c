#include "internal/runtime/dcc_task_group_internal.h"

#include <errno.h>

dcc_status_t dcc_task_group_wait_result(
    dcc_task_group_t *group,
    uint32_t timeout_ms,
    dcc_task_group_wait_result_t *out
) {
    if (group == NULL || group->group == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_task_group_wait_all_entries(group, timeout_ms);
    if (status == DCC_OK) {
        int rc;
        if (timeout_ms == 0U) {
            rc = llam_task_group_join(group->group);
        } else {
            rc = llam_task_group_join_until(group->group, dcc_task_group_deadline_from_timeout(timeout_ms));
        }
        status = rc == 0 ? DCC_OK : dcc_task_group_status_from_errno(errno);
    }

    dcc_status_t lock_status = dcc_task_group_state_lock(group);
    if (lock_status != DCC_OK) {
        return lock_status;
    }
    dcc_task_group_snapshot_locked(group, out);
    dcc_task_group_state_unlock(group);
    return status;
}
