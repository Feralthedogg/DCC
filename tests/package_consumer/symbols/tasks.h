#ifndef DCC_PACKAGE_CONSUMER_SYMBOLS_TASKS_H
#define DCC_PACKAGE_CONSUMER_SYMBOLS_TASKS_H

#include <dcc/dcc.h>

typedef dcc_status_t (*dcc_package_task_group_create_fn)(dcc_client_t *, dcc_task_group_t **);
typedef dcc_status_t (*dcc_package_task_group_spawn_fn)(
        dcc_task_group_t *,
        dcc_task_fn,
        void *,
        const dcc_task_options_t *
    );
typedef dcc_status_t (*dcc_package_task_group_wait_fn)(dcc_task_group_t *, uint32_t);
typedef dcc_status_t (*dcc_package_task_group_wait_any_fn)(dcc_task_group_t *, uint32_t, size_t *);
typedef dcc_status_t (*dcc_package_task_group_wait_result_fn)(
        dcc_task_group_t *,
        uint32_t,
        dcc_task_group_wait_result_t *
    );
typedef dcc_status_t (*dcc_package_task_group_wait_any_and_cancel_fn)(
        dcc_task_group_t *,
        uint32_t,
        dcc_task_group_wait_result_t *
    );
typedef dcc_status_t (*dcc_package_task_group_cancel_and_wait_fn)(
        dcc_task_group_t *,
        uint32_t,
        dcc_task_group_wait_result_t *
    );
typedef dcc_status_t (*dcc_package_task_group_cancel_when_fn)(
        dcc_task_group_t *,
        dcc_task_cancel_predicate_fn,
        void *,
        uint32_t
    );
typedef dcc_status_t (*dcc_package_task_group_cancel_fn)(dcc_task_group_t *);
typedef dcc_status_t (*dcc_package_task_group_destroy_fn)(dcc_task_group_t *);

#endif
