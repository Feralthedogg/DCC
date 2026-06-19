#include <dcc/dcc.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <llam/runtime.h>

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct task_smoke_wait {
    dcc_client_t *client;
    dcc_status_t status;
} task_smoke_wait_t;

typedef struct task_smoke_state {
    atomic_uint ran;
    atomic_uint sum;
} task_smoke_state_t;

typedef struct task_smoke_arg {
    task_smoke_state_t *state;
    unsigned value;
    useconds_t delay_us;
    int use_llam_sleep;
    atomic_uint *release_gate;
    atomic_uint *started;
    atomic_uint *sleep_canceled;
} task_smoke_arg_t;

static void *task_smoke_client_wait(void *arg) {
    task_smoke_wait_t *state = (task_smoke_wait_t *)arg;
    state->status = dcc_client_wait(state->client);
    return NULL;
}

static void task_smoke_run(void *arg) {
    task_smoke_arg_t *task = (task_smoke_arg_t *)arg;
    if (task->started != NULL) {
        atomic_store_explicit(task->started, 1U, memory_order_release);
    }
    while (task->release_gate != NULL &&
           atomic_load_explicit(task->release_gate, memory_order_acquire) == 0U) {
        usleep(1000U);
    }
    if (task->use_llam_sleep) {
        if (llam_sleep_ns((uint64_t)task->delay_us * UINT64_C(1000)) != 0 && task->sleep_canceled != NULL) {
            atomic_fetch_add_explicit(task->sleep_canceled, 1U, memory_order_release);
        }
    } else if (task->delay_us != 0U) {
        usleep(task->delay_us);
    }
    atomic_fetch_add_explicit(&task->state->sum, task->value, memory_order_relaxed);
    atomic_fetch_add_explicit(&task->state->ran, 1U, memory_order_release);
}

static uint8_t task_smoke_cancel_predicate(void *arg) {
    atomic_uint *flag = (atomic_uint *)arg;
    return flag != NULL && atomic_load_explicit(flag, memory_order_acquire) != 0U ? 1U : 0U;
}

static int task_smoke_fail(dcc_client_t *client, pthread_t *wait_thread, dcc_task_group_t *group, const char *message) {
    fprintf(stderr, "%s\n", message);
    if (group != NULL) {
        (void)dcc_task_group_cancel(group);
        (void)dcc_task_group_wait(group, 2000);
        (void)dcc_task_group_destroy(group);
    }
    if (client != NULL) {
        (void)dcc_client_stop(client);
    }
    if (wait_thread != NULL) {
        (void)pthread_join(*wait_thread, NULL);
    }
    if (client != NULL) {
        dcc_client_destroy(client);
    }
    return 1;
}

int main(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t status = dcc_client_create(&options, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(status));
        return 1;
    }

    dcc_task_group_t *group = (dcc_task_group_t *)1;
    if (dcc_task_group_create(NULL, &group) != DCC_ERR_INVALID_ARG || group != NULL ||
        dcc_task_group_create(client, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_task_group_create(client, &group) != DCC_ERR_STATE || group != NULL) {
        dcc_client_destroy(client);
        fprintf(stderr, "task group invalid create handling failed\n");
        return 1;
    }

    status = dcc_client_start(client);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_client_start failed: %s\n", dcc_status_string(status));
        dcc_client_destroy(client);
        return 1;
    }

    task_smoke_wait_t wait_state = {
        .client = client,
        .status = DCC_ERR_RUNTIME,
    };
    pthread_t wait_thread;
    if (pthread_create(&wait_thread, NULL, task_smoke_client_wait, &wait_state) != 0) {
        return task_smoke_fail(client, NULL, NULL, "failed to start client wait thread");
    }

    status = dcc_task_group_create(client, &group);
    if (status != DCC_OK || group == NULL) {
        char message[128];
        snprintf(message, sizeof(message), "dcc_task_group_create failed: %s", dcc_status_string(status));
        return task_smoke_fail(client, &wait_thread, NULL, message);
    }

    task_smoke_state_t state;
    atomic_init(&state.ran, 0U);
    atomic_init(&state.sum, 0U);
    task_smoke_arg_t args[] = {
        {.state = &state, .value = 7U, .delay_us = 1000U},
        {.state = &state, .value = 11U, .delay_us = 1000U},
    };
    dcc_task_options_t task_options = {
        .size = sizeof(task_options),
        .task_class = DCC_TASK_CLASS_BATCH,
        .flags = DCC_TASK_F_LATENCY_CRITICAL,
    };
    if (dcc_task_group_spawn(group, task_smoke_run, &args[0], NULL) != DCC_OK ||
        dcc_task_group_spawn(group, task_smoke_run, &args[1], &task_options) != DCC_OK ||
        dcc_task_group_spawn(group, NULL, NULL, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_task_group_cancel_when(NULL, task_smoke_cancel_predicate, NULL, 1U) != DCC_ERR_INVALID_ARG ||
        dcc_task_group_cancel_when(group, NULL, NULL, 1U) != DCC_ERR_INVALID_ARG ||
        dcc_task_group_wait_any(NULL, 1U, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_task_group_wait_result(group, 1U, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_task_group_wait_any_and_cancel(NULL, 1U, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_task_group_cancel_and_wait(NULL, 1U, NULL) != DCC_ERR_INVALID_ARG) {
        return task_smoke_fail(client, &wait_thread, group, "task group spawn handling failed");
    }

    size_t first_index = (size_t)99U;
    status = dcc_task_group_wait_any(group, 2000, &first_index);
    if (status != DCC_OK || first_index != 0U) {
        char message[160];
        snprintf(
            message,
            sizeof(message),
            "task group wait_any failed: status=%s first=%zu",
            dcc_status_string(status),
            first_index
        );
        return task_smoke_fail(client, &wait_thread, group, message);
    }

    dcc_task_group_wait_result_t result;
    status = dcc_task_group_wait_result(group, 2000, &result);
    if (status != DCC_OK ||
        result.size != sizeof(result) ||
        result.task_count != 2U ||
        result.completed_count != 2U ||
        result.pending_count != 0U ||
        result.first_completed_index != 0U ||
        result.last_completed_index == (size_t)-1 ||
        result.cancel_requested != 0U ||
        atomic_load_explicit(&state.ran, memory_order_acquire) != 2U ||
        atomic_load_explicit(&state.sum, memory_order_relaxed) != 18U) {
        char message[160];
        snprintf(
            message,
            sizeof(message),
            "task group wait_result failed: status=%s ran=%u sum=%u tasks=%zu completed=%zu pending=%zu first=%zu",
            dcc_status_string(status),
            atomic_load_explicit(&state.ran, memory_order_relaxed),
            atomic_load_explicit(&state.sum, memory_order_relaxed),
            result.task_count,
            result.completed_count,
            result.pending_count,
            result.first_completed_index
        );
        return task_smoke_fail(client, &wait_thread, group, message);
    }

    status = dcc_task_group_destroy(group);
    group = NULL;
    if (status != DCC_OK) {
        char message[128];
        snprintf(message, sizeof(message), "task group destroy failed: %s", dcc_status_string(status));
        return task_smoke_fail(client, &wait_thread, NULL, message);
    }

    status = dcc_task_group_create(client, &group);
    if (status != DCC_OK || group == NULL) {
        return task_smoke_fail(client, &wait_thread, NULL, "failed to create timeout task group");
    }
    atomic_uint timeout_release;
    atomic_init(&timeout_release, 0U);
    task_smoke_arg_t slow_arg = {
        .state = &state,
        .value = 0U,
        .release_gate = &timeout_release,
    };
    if (dcc_task_group_wait_any(group, 1U, NULL) != DCC_ERR_STATE ||
        dcc_task_group_spawn(group, task_smoke_run, &slow_arg, NULL) != DCC_OK) {
        return task_smoke_fail(client, &wait_thread, group, "task group timeout setup failed");
    }
    first_index = 0U;
    status = dcc_task_group_wait_any(group, 1U, &first_index);
    if (status != DCC_ERR_TIMEOUT || first_index != (size_t)-1) {
        char message[192];
        snprintf(
            message,
            sizeof(message),
            "task group wait_any timeout failed: status=%s first=%zu",
            dcc_status_string(status),
            first_index
        );
        return task_smoke_fail(client, &wait_thread, group, message);
    }
    atomic_store_explicit(&timeout_release, 1U, memory_order_release);
    status = dcc_task_group_wait_result(group, 2000, &result);
    if (status != DCC_OK ||
        result.task_count != 1U ||
        result.completed_count != 1U ||
        result.pending_count != 0U ||
        result.first_completed_index != 0U ||
        result.cancel_requested != 0U) {
        return task_smoke_fail(client, &wait_thread, group, "task group timeout release result failed");
    }
    status = dcc_task_group_destroy(group);
    group = NULL;
    if (status != DCC_OK) {
        return task_smoke_fail(client, &wait_thread, NULL, "task group timeout cleanup destroy failed");
    }

    status = dcc_task_group_create(client, &group);
    if (status != DCC_OK || group == NULL) {
        return task_smoke_fail(client, &wait_thread, NULL, "failed to create cancel task group");
    }
    atomic_uint canceled_sleep;
    atomic_init(&canceled_sleep, 0U);
    task_smoke_arg_t cancel_arg = {
        .state = &state,
        .value = 0U,
        .delay_us = 5000000U,
        .use_llam_sleep = 1,
        .sleep_canceled = &canceled_sleep,
    };
    if (dcc_task_group_spawn(group, task_smoke_run, &cancel_arg, NULL) != DCC_OK) {
        return task_smoke_fail(client, &wait_thread, group, "task group cancel setup failed");
    }
    status = dcc_task_group_cancel_and_wait(group, 2000, &result);
    if (status != DCC_OK ||
        result.task_count != 1U ||
        result.completed_count != 1U ||
        result.pending_count != 0U ||
        result.cancel_requested != 1U ||
        atomic_load_explicit(&canceled_sleep, memory_order_acquire) == 0U) {
        char message[192];
        snprintf(
            message,
            sizeof(message),
            "task group cancel result failed: status=%s tasks=%zu completed=%zu pending=%zu cancel=%u sleep_cancel=%u",
            dcc_status_string(status),
            result.task_count,
            result.completed_count,
            result.pending_count,
            (unsigned)result.cancel_requested,
            atomic_load_explicit(&canceled_sleep, memory_order_relaxed)
        );
        return task_smoke_fail(client, &wait_thread, group, message);
    }
    status = dcc_task_group_destroy(group);
    group = NULL;
    if (status != DCC_OK) {
        return task_smoke_fail(client, &wait_thread, NULL, "task group cancel cleanup destroy failed");
    }

    status = dcc_task_group_create(client, &group);
    if (status != DCC_OK || group == NULL) {
        return task_smoke_fail(client, &wait_thread, NULL, "failed to create cancel predicate task group");
    }
    atomic_uint predicate_flag;
    atomic_uint predicate_started;
    atomic_uint predicate_canceled_sleep;
    atomic_init(&predicate_flag, 0U);
    atomic_init(&predicate_started, 0U);
    atomic_init(&predicate_canceled_sleep, 0U);
    task_smoke_arg_t predicate_arg = {
        .state = &state,
        .value = 0U,
        .delay_us = 5000000U,
        .use_llam_sleep = 1,
        .started = &predicate_started,
        .sleep_canceled = &predicate_canceled_sleep,
    };
    if (dcc_task_group_spawn(group, task_smoke_run, &predicate_arg, NULL) != DCC_OK ||
        dcc_task_group_cancel_when(group, task_smoke_cancel_predicate, &predicate_flag, 1U) != DCC_OK) {
        return task_smoke_fail(client, &wait_thread, group, "task group predicate cancel setup failed");
    }
    for (unsigned i = 0; i < 1000U &&
            atomic_load_explicit(&predicate_started, memory_order_acquire) == 0U;
         ++i) {
        usleep(1000U);
    }
    if (atomic_load_explicit(&predicate_started, memory_order_acquire) == 0U) {
        return task_smoke_fail(client, &wait_thread, group, "task group predicate task did not start");
    }
    atomic_store_explicit(&predicate_flag, 1U, memory_order_release);
    status = dcc_task_group_wait_result(group, 2000, &result);
    if (status != DCC_OK ||
        result.task_count != 2U ||
        result.completed_count != 2U ||
        result.pending_count != 0U ||
        result.cancel_requested != 1U ||
        atomic_load_explicit(&predicate_canceled_sleep, memory_order_acquire) == 0U) {
        char message[224];
        snprintf(
            message,
            sizeof(message),
            "task group predicate cancel failed: status=%s tasks=%zu completed=%zu pending=%zu cancel=%u sleep_cancel=%u",
            dcc_status_string(status),
            result.task_count,
            result.completed_count,
            result.pending_count,
            (unsigned)result.cancel_requested,
            atomic_load_explicit(&predicate_canceled_sleep, memory_order_relaxed)
        );
        return task_smoke_fail(client, &wait_thread, group, message);
    }
    status = dcc_task_group_destroy(group);
    group = NULL;
    if (status != DCC_OK) {
        return task_smoke_fail(client, &wait_thread, NULL, "task group predicate cancel cleanup destroy failed");
    }

    status = dcc_task_group_create(client, &group);
    if (status != DCC_OK || group == NULL) {
        return task_smoke_fail(client, &wait_thread, NULL, "failed to create race task group");
    }
    task_smoke_state_t race_state;
    atomic_init(&race_state.ran, 0U);
    atomic_init(&race_state.sum, 0U);
    atomic_uint race_canceled_sleep;
    atomic_init(&race_canceled_sleep, 0U);
    task_smoke_arg_t race_args[] = {
        {.state = &race_state, .value = 3U, .delay_us = 1000U},
        {
            .state = &race_state,
            .value = 5U,
            .delay_us = 5000000U,
            .use_llam_sleep = 1,
            .sleep_canceled = &race_canceled_sleep,
        },
    };
    if (dcc_task_group_spawn(group, task_smoke_run, &race_args[0], NULL) != DCC_OK ||
        dcc_task_group_spawn(group, task_smoke_run, &race_args[1], NULL) != DCC_OK) {
        return task_smoke_fail(client, &wait_thread, group, "task group race setup failed");
    }
    status = dcc_task_group_wait_any_and_cancel(group, 2000, &result);
    if (status != DCC_OK ||
        result.task_count != 2U ||
        result.completed_count != 2U ||
        result.pending_count != 0U ||
        result.first_completed_index != 0U ||
        result.cancel_requested != 1U ||
        atomic_load_explicit(&race_canceled_sleep, memory_order_acquire) == 0U) {
        char message[224];
        snprintf(
            message,
            sizeof(message),
            "task group race result failed: status=%s tasks=%zu completed=%zu pending=%zu first=%zu cancel=%u sleep_cancel=%u",
            dcc_status_string(status),
            result.task_count,
            result.completed_count,
            result.pending_count,
            result.first_completed_index,
            (unsigned)result.cancel_requested,
            atomic_load_explicit(&race_canceled_sleep, memory_order_relaxed)
        );
        return task_smoke_fail(client, &wait_thread, group, message);
    }
    status = dcc_task_group_destroy(group);
    group = NULL;
    if (status != DCC_OK) {
        return task_smoke_fail(client, &wait_thread, NULL, "task group race cleanup destroy failed");
    }

    dcc_runtime_stats_t stats = {
        .size = sizeof(stats),
    };
    status = dcc_client_runtime_stats(client, &stats);
    if (status != DCC_OK || stats.tasks_spawned < 5U) {
        return task_smoke_fail(client, &wait_thread, NULL, "task group runtime stats did not include group tasks");
    }

    (void)dcc_client_stop(client);
    (void)pthread_join(wait_thread, NULL);
    dcc_client_destroy(client);

    if (wait_state.status != DCC_OK) {
        fprintf(stderr, "client wait failed: %s\n", dcc_status_string(wait_state.status));
        return 1;
    }

    return 0;
}
#endif
