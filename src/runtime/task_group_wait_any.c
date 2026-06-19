#include "internal/runtime/dcc_task_group_internal.h"

#include <errno.h>
#include <stdint.h>

dcc_status_t dcc_task_group_wait_any(dcc_task_group_t *group, uint32_t timeout_ms, size_t *out_index) {
    if (group == NULL || group->group == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (out_index != NULL) {
        *out_index = SIZE_MAX;
    }

#if !defined(_WIN32)
    dcc_status_t status = dcc_task_group_state_lock(group);
    if (status != DCC_OK) {
        return status;
    }
    if (group->entry_count == 0U) {
        dcc_task_group_state_unlock(group);
        return DCC_ERR_STATE;
    }

    struct timespec deadline;
    if (timeout_ms != 0U) {
        status = dcc_task_group_make_deadline_ms(timeout_ms, &deadline);
        if (status != DCC_OK) {
            dcc_task_group_state_unlock(group);
            return status;
        }
    }

    while (group->completed_count == 0U && !group->cancel_requested && status == DCC_OK) {
        int rc = timeout_ms == 0U
            ? pthread_cond_wait(&group->state_cond, &group->state_mutex)
            : pthread_cond_timedwait(&group->state_cond, &group->state_mutex, &deadline);
        if (rc == ETIMEDOUT) {
            status = group->completed_count != 0U ? DCC_OK : DCC_ERR_TIMEOUT;
        } else {
            status = dcc_task_group_wait_status_from_errno(rc);
        }
    }

    if (status == DCC_OK && group->completed_count == 0U && group->cancel_requested) {
        status = DCC_ERR_CANCELED;
    }
    if (status == DCC_OK && out_index != NULL) {
        *out_index = group->first_completed_index;
    }
    dcc_task_group_state_unlock(group);
    return status;
#else
    uint64_t deadline = timeout_ms == 0U ? UINT64_MAX : dcc_task_group_deadline_from_timeout(timeout_ms);
    for (;;) {
        dcc_status_t status = dcc_task_group_state_lock(group);
        if (status != DCC_OK) {
            return status;
        }
        if (group->entry_count == 0U) {
            dcc_task_group_state_unlock(group);
            return DCC_ERR_STATE;
        }
        if (group->completed_count != 0U) {
            if (out_index != NULL) {
                *out_index = group->first_completed_index;
            }
            dcc_task_group_state_unlock(group);
            return DCC_OK;
        }
        if (group->cancel_requested) {
            dcc_task_group_state_unlock(group);
            return DCC_ERR_CANCELED;
        }
        dcc_task_group_state_unlock(group);
        if (timeout_ms != 0U && llam_now_ns() >= deadline) {
            return DCC_ERR_TIMEOUT;
        }
        dcc_task_group_sleep_poll();
    }
#endif
}
