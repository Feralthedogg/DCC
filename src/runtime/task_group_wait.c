#include "internal/runtime/dcc_task_group_internal.h"

#include <errno.h>

dcc_status_t dcc_task_group_wait(dcc_task_group_t *group, uint32_t timeout_ms) {
    if (group == NULL || group->group == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_task_group_wait_all_entries(group, timeout_ms);
    if (status != DCC_OK) {
        return status;
    }

    int rc;
    if (timeout_ms == 0U) {
        rc = llam_task_group_join(group->group);
    } else {
        rc = llam_task_group_join_until(group->group, dcc_task_group_deadline_from_timeout(timeout_ms));
    }
    if (rc != 0) {
        return dcc_task_group_status_from_errno(errno);
    }
    return DCC_OK;
}
