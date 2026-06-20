#include "internal/runtime/dcc_task_group_internal.h"

#include <errno.h>
#include <limits.h>
#include <time.h>

#if !defined(_WIN32)
static void dcc_task_group_timespec_add_ms(struct timespec *ts, uint64_t ms) {
    uint64_t sec = ms / 1000U;
    uint64_t nsec = (ms % 1000U) * UINT64_C(1000000);
    if (sec > (uint64_t)LONG_MAX - (uint64_t)ts->tv_sec) {
        ts->tv_sec = (time_t)LONG_MAX;
        ts->tv_nsec = 999999999L;
        return;
    }
    ts->tv_sec += (time_t)sec;
    ts->tv_nsec += (long)nsec;
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

dcc_status_t dcc_task_group_make_deadline_ms(uint64_t wait_ms, struct timespec *out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (clock_gettime(CLOCK_REALTIME, out) != 0) {
        return dcc_task_group_status_from_errno(errno);
    }
    dcc_task_group_timespec_add_ms(out, wait_ms);
    return DCC_OK;
}

dcc_status_t dcc_task_group_state_lock(dcc_task_group_t *group) {
    if (group == NULL || !group->state_initialized) {
        return DCC_ERR_STATE;
    }
    return dcc_task_group_wait_status_from_errno(pthread_mutex_lock(&group->state_mutex));
}

void dcc_task_group_state_unlock(dcc_task_group_t *group) {
    (void)pthread_mutex_unlock(&group->state_mutex);
}

void dcc_task_group_state_broadcast(dcc_task_group_t *group) {
    if (group != NULL && group->state_initialized) {
        (void)pthread_cond_broadcast(&group->state_cond);
    }
}

dcc_status_t dcc_task_group_wait_all_entries(dcc_task_group_t *group, uint32_t timeout_ms) {
    dcc_status_t status = dcc_task_group_state_lock(group);
    if (status != DCC_OK) {
        return status;
    }

    struct timespec deadline;
    if (timeout_ms != 0U) {
        status = dcc_task_group_make_deadline_ms(timeout_ms, &deadline);
        if (status != DCC_OK) {
            dcc_task_group_state_unlock(group);
            return status;
        }
    }

    while (group->completed_count < group->entry_count && status == DCC_OK) {
        int rc = timeout_ms == 0U
            ? pthread_cond_wait(&group->state_cond, &group->state_mutex)
            : pthread_cond_timedwait(&group->state_cond, &group->state_mutex, &deadline);
        if (rc == ETIMEDOUT) {
            status = group->completed_count >= group->entry_count ? DCC_OK : DCC_ERR_TIMEOUT;
        } else {
            status = dcc_task_group_wait_status_from_errno(rc);
        }
    }

    dcc_task_group_state_unlock(group);
    return status;
}
#else
dcc_status_t dcc_task_group_state_lock(dcc_task_group_t *group) {
    if (group == NULL) {
        return DCC_ERR_STATE;
    }
    while (atomic_flag_test_and_set_explicit(&group->state_lock, memory_order_acquire)) {
    }
    return DCC_OK;
}

void dcc_task_group_state_unlock(dcc_task_group_t *group) {
    atomic_flag_clear_explicit(&group->state_lock, memory_order_release);
}

void dcc_task_group_state_broadcast(dcc_task_group_t *group) {
    (void)group;
}

void dcc_task_group_sleep_poll(void) {
    Sleep(1U);
}

dcc_status_t dcc_task_group_wait_all_entries(dcc_task_group_t *group, uint32_t timeout_ms) {
    uint64_t deadline = timeout_ms == 0U ? UINT64_MAX : dcc_task_group_deadline_from_timeout(timeout_ms);

    for (;;) {
        dcc_status_t status = dcc_task_group_state_lock(group);
        if (status != DCC_OK) {
            return status;
        }
        if (group->completed_count >= group->entry_count) {
            dcc_task_group_state_unlock(group);
            return DCC_OK;
        }
        dcc_task_group_state_unlock(group);

        if (timeout_ms != 0U && llam_now_ns() >= deadline) {
            return DCC_ERR_TIMEOUT;
        }
        dcc_task_group_sleep_poll();
    }
}
#endif

void dcc_task_group_snapshot_locked(
    const dcc_task_group_t *group,
    dcc_task_group_wait_result_t *out
) {
    out->size = sizeof(*out);
    out->task_count = group->entry_count;
    out->completed_count = group->completed_count;
    out->pending_count = group->entry_count >= group->completed_count
        ? group->entry_count - group->completed_count
        : 0U;
    out->first_completed_index = group->first_completed_index;
    out->last_completed_index = group->last_completed_index;
    out->cancel_requested = group->cancel_requested;
}
