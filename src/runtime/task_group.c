#include "internal/runtime/dcc_task_group_internal.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

dcc_status_t dcc_task_group_create(dcc_client_t *client, dcc_task_group_t **out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!dcc_task_group_client_accepts_work(client)) {
        return DCC_ERR_STATE;
    }

    dcc_task_group_t *wrapper = (dcc_task_group_t *)calloc(1, sizeof(*wrapper));
    if (wrapper == NULL) {
        return DCC_ERR_NOMEM;
    }
    wrapper->first_completed_index = SIZE_MAX;
    wrapper->last_completed_index = SIZE_MAX;
#if defined(_WIN32)
    atomic_flag_clear_explicit(&wrapper->state_lock, memory_order_release);
#else
    if (pthread_mutex_init(&wrapper->state_mutex, NULL) != 0) {
        free(wrapper);
        return DCC_ERR_RUNTIME;
    }
    if (pthread_cond_init(&wrapper->state_cond, NULL) != 0) {
        (void)pthread_mutex_destroy(&wrapper->state_mutex);
        free(wrapper);
        return DCC_ERR_RUNTIME;
    }
    wrapper->state_initialized = 1U;
#endif
    wrapper->group = llam_task_group_create();
    if (wrapper->group == NULL) {
        dcc_status_t status = dcc_task_group_status_from_errno(errno);
#if !defined(_WIN32)
        (void)pthread_cond_destroy(&wrapper->state_cond);
        (void)pthread_mutex_destroy(&wrapper->state_mutex);
#endif
        free(wrapper);
        return status;
    }
    wrapper->client = client;
    *out = wrapper;
    return DCC_OK;
}

dcc_status_t dcc_task_group_destroy(dcc_task_group_t *group) {
    if (group == NULL) {
        return DCC_OK;
    }
    if (group->group != NULL && llam_task_group_destroy(group->group) != 0) {
        return dcc_task_group_status_from_errno(errno);
    }
    for (size_t i = 0; i < group->entry_count; ++i) {
        free(group->entries[i]);
    }
    free(group->entries);
#if !defined(_WIN32)
    if (group->state_initialized) {
        (void)pthread_cond_destroy(&group->state_cond);
        (void)pthread_mutex_destroy(&group->state_mutex);
    }
#endif
    free(group);
    return DCC_OK;
}
