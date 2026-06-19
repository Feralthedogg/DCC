#include "internal/runtime/dcc_task_group_internal.h"

#include <errno.h>
#include <stdlib.h>

uint8_t dcc_task_group_cancel_requested(dcc_task_group_t *group) {
    uint8_t requested = 0U;
    if (group != NULL && dcc_task_group_state_lock(group) == DCC_OK) {
        requested = group->cancel_requested;
        dcc_task_group_state_unlock(group);
    }
    return requested;
}

static void dcc_task_group_cancel_when_run(void *arg) {
    dcc_task_group_cancel_watch_t *watch = (dcc_task_group_cancel_watch_t *)arg;
    if (watch == NULL) {
        return;
    }

    while (!dcc_task_group_cancel_requested(watch->group)) {
        if (watch->predicate != NULL && watch->predicate(watch->user_data)) {
            (void)dcc_task_group_cancel(watch->group);
            break;
        }
        if (llam_sleep_ns(watch->poll_interval_ns) != 0) {
            break;
        }
    }
    free(watch);
}

dcc_status_t dcc_task_group_cancel(dcc_task_group_t *group) {
    if (group == NULL || group->group == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (llam_task_group_cancel(group->group) != 0) {
        return dcc_task_group_status_from_errno(errno);
    }
    if (dcc_task_group_state_lock(group) == DCC_OK) {
        group->cancel_requested = 1U;
        dcc_task_group_state_broadcast(group);
        dcc_task_group_state_unlock(group);
    }
    return DCC_OK;
}

dcc_status_t dcc_task_group_cancel_when(
    dcc_task_group_t *group,
    dcc_task_cancel_predicate_fn predicate,
    void *user_data,
    uint32_t poll_interval_ms
) {
    if (group == NULL || group->group == NULL || predicate == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_task_group_cancel_watch_t *watch = (dcc_task_group_cancel_watch_t *)calloc(1U, sizeof(*watch));
    if (watch == NULL) {
        return DCC_ERR_NOMEM;
    }
    watch->group = group;
    watch->predicate = predicate;
    watch->user_data = user_data;
    watch->poll_interval_ns = (uint64_t)(poll_interval_ms != 0U ? poll_interval_ms : 10U) * UINT64_C(1000000);

    dcc_task_options_t options = {
        .size = sizeof(options),
        .task_class = DCC_TASK_CLASS_LATENCY,
        .flags = DCC_TASK_F_LATENCY_CRITICAL,
    };
    dcc_status_t status = dcc_task_group_spawn(group, dcc_task_group_cancel_when_run, watch, &options);
    if (status != DCC_OK) {
        free(watch);
    }
    return status;
}
