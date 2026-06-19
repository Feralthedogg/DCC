#ifndef DCC_TASK_GROUP_STATE_INTERNAL_H
#define DCC_TASK_GROUP_STATE_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"

#include <dcc/tasks.h>

#include <llam/runtime.h>

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef struct dcc_task_group_entry dcc_task_group_entry_t;
typedef struct dcc_task_group_cancel_watch dcc_task_group_cancel_watch_t;

struct dcc_task_group {
    dcc_client_t *client;
    llam_task_group_t *group;
    dcc_task_group_entry_t **entries;
    size_t entry_count;
    size_t entry_cap;
    size_t completed_count;
    uint64_t next_completion_order;
    size_t first_completed_index;
    size_t last_completed_index;
    uint8_t cancel_requested;
#if !defined(_WIN32)
    pthread_mutex_t state_mutex;
    pthread_cond_t state_cond;
    uint8_t state_initialized;
#else
    atomic_flag state_lock;
#endif
};

struct dcc_task_group_entry {
    dcc_task_group_t *owner;
    dcc_task_fn fn;
    void *arg;
    size_t index;
    uint8_t completed;
    uint64_t completion_order;
};

struct dcc_task_group_cancel_watch {
    dcc_task_group_t *group;
    dcc_task_cancel_predicate_fn predicate;
    void *user_data;
    uint64_t poll_interval_ns;
};

#endif
