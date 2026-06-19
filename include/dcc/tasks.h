#ifndef DCC_TASKS_H
#define DCC_TASKS_H

#include <dcc/client.h>
#include <dcc/error.h>
#include <dcc/export.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_task_group dcc_task_group_t;

typedef void (*dcc_task_fn)(void *arg);

typedef enum dcc_task_class {
    DCC_TASK_CLASS_LATENCY = 0,
    DCC_TASK_CLASS_DEFAULT = 1,
    DCC_TASK_CLASS_BATCH = 2
} dcc_task_class_t;

typedef enum dcc_task_flag {
    DCC_TASK_F_PINNED = 1U << 0,
    DCC_TASK_F_NO_PREEMPT = 1U << 1,
    DCC_TASK_F_LATENCY_CRITICAL = 1U << 2
} dcc_task_flag_t;

typedef struct dcc_task_options {
    size_t size;
    dcc_task_class_t task_class;
    uint32_t flags;
    uint64_t deadline_ns;
} dcc_task_options_t;

typedef struct dcc_task_group_wait_result {
    size_t size;
    size_t task_count;
    size_t completed_count;
    size_t pending_count;
    size_t first_completed_index;
    size_t last_completed_index;
    uint8_t cancel_requested;
} dcc_task_group_wait_result_t;

typedef uint8_t (*dcc_task_cancel_predicate_fn)(void *user_data);

DCC_API dcc_status_t dcc_task_group_create(dcc_client_t *client, dcc_task_group_t **out);
DCC_API dcc_status_t dcc_task_group_spawn(
    dcc_task_group_t *group,
    dcc_task_fn fn,
    void *arg,
    const dcc_task_options_t *options
);
DCC_API dcc_status_t dcc_task_group_cancel(dcc_task_group_t *group);
DCC_API dcc_status_t dcc_task_group_cancel_when(
    dcc_task_group_t *group,
    dcc_task_cancel_predicate_fn predicate,
    void *user_data,
    uint32_t poll_interval_ms
);
DCC_API dcc_status_t dcc_task_group_wait(dcc_task_group_t *group, uint32_t timeout_ms);
DCC_API dcc_status_t dcc_task_group_wait_any(dcc_task_group_t *group, uint32_t timeout_ms, size_t *out_index);
DCC_API dcc_status_t dcc_task_group_wait_result(
    dcc_task_group_t *group,
    uint32_t timeout_ms,
    dcc_task_group_wait_result_t *out
);
DCC_API dcc_status_t dcc_task_group_wait_any_and_cancel(
    dcc_task_group_t *group,
    uint32_t timeout_ms,
    dcc_task_group_wait_result_t *out
);
DCC_API dcc_status_t dcc_task_group_cancel_and_wait(
    dcc_task_group_t *group,
    uint32_t timeout_ms,
    dcc_task_group_wait_result_t *out
);
DCC_API dcc_status_t dcc_task_group_destroy(dcc_task_group_t *group);

#ifdef __cplusplus
}
#endif

#endif
