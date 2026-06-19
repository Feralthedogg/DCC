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
