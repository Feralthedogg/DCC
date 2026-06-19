#include "internal/runtime/dcc_task_group_internal.h"

dcc_status_t dcc_task_group_wait_any_and_cancel(
    dcc_task_group_t *group,
    uint32_t timeout_ms,
    dcc_task_group_wait_result_t *out
) {
    if (group == NULL || group->group == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_task_group_wait_any(group, timeout_ms, NULL);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_task_group_cancel(group);
    if (status != DCC_OK) {
        return status;
    }
    return out != NULL
        ? dcc_task_group_wait_result(group, timeout_ms, out)
        : dcc_task_group_wait(group, timeout_ms);
}

dcc_status_t dcc_task_group_cancel_and_wait(
    dcc_task_group_t *group,
    uint32_t timeout_ms,
    dcc_task_group_wait_result_t *out
) {
    dcc_status_t status = dcc_task_group_cancel(group);
    if (status != DCC_OK) {
        return status;
    }
    return out != NULL
        ? dcc_task_group_wait_result(group, timeout_ms, out)
        : dcc_task_group_wait(group, timeout_ms);
}
