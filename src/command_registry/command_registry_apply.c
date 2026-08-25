#include "internal/command_registry/dcc_command_registry_internal.h"
#include "internal/dcc_windows_internal.h"

#include <dcc/rest.h>

#if !defined(_WIN32)
#include <pthread.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct dcc_command_registry_owned_diff {
    dcc_command_registry_diff_action_t action;
    dcc_snowflake_t remote_id;
    char *json;
    size_t plan_index;
} dcc_command_registry_owned_diff_t;

#if defined(_WIN32)
typedef CRITICAL_SECTION dcc_command_registry_mutex_t;
typedef CONDITION_VARIABLE dcc_command_registry_condition_t;
typedef DWORD dcc_command_registry_thread_id_t;
#else
typedef pthread_mutex_t dcc_command_registry_mutex_t;
typedef pthread_cond_t dcc_command_registry_condition_t;
typedef pthread_t dcc_command_registry_thread_id_t;
#endif

struct dcc_command_registry_operation {
    dcc_command_registry_mutex_t mutex;
    dcc_command_registry_condition_t condition;
    size_t references;
    dcc_client_t *client;
    dcc_snowflake_t application_id;
    dcc_snowflake_t guild_id;
    dcc_command_registry_owned_diff_t *diffs;
    size_t diff_count;
    uint8_t dry_run;
    uint8_t cancel_requested;
    uint8_t terminal;
    uint8_t callback_delivered;
    dcc_rest_request_t *active_request;
    dcc_rest_priority_t priority;
    dcc_rest_auth_mode_t auth_mode;
    char *auth_token;
    dcc_command_registry_operation_result_fn callback;
    void *user_data;
    dcc_command_registry_thread_id_t callback_thread;
    uint8_t in_callback;
    dcc_rest_result_t *failed_result;
    dcc_command_registry_operation_result_t result;
};

static int dcc_command_registry_mutex_init(dcc_command_registry_mutex_t *mutex) {
#if defined(_WIN32)
    InitializeCriticalSection(mutex);
    return 0;
#else
    return pthread_mutex_init(mutex, NULL);
#endif
}

static void dcc_command_registry_mutex_destroy(dcc_command_registry_mutex_t *mutex) {
#if defined(_WIN32)
    DeleteCriticalSection(mutex);
#else
    (void)pthread_mutex_destroy(mutex);
#endif
}

static void dcc_command_registry_mutex_lock(dcc_command_registry_mutex_t *mutex) {
#if defined(_WIN32)
    EnterCriticalSection(mutex);
#else
    (void)pthread_mutex_lock(mutex);
#endif
}

static void dcc_command_registry_mutex_unlock(dcc_command_registry_mutex_t *mutex) {
#if defined(_WIN32)
    LeaveCriticalSection(mutex);
#else
    (void)pthread_mutex_unlock(mutex);
#endif
}

static int dcc_command_registry_condition_init(
    dcc_command_registry_condition_t *condition) {
#if defined(_WIN32)
    InitializeConditionVariable(condition);
    return 0;
#else
    return pthread_cond_init(condition, NULL);
#endif
}

static void dcc_command_registry_condition_destroy(
    dcc_command_registry_condition_t *condition) {
#if defined(_WIN32)
    (void)condition;
#else
    (void)pthread_cond_destroy(condition);
#endif
}

static void dcc_command_registry_condition_broadcast(
    dcc_command_registry_condition_t *condition) {
#if defined(_WIN32)
    WakeAllConditionVariable(condition);
#else
    (void)pthread_cond_broadcast(condition);
#endif
}

static int dcc_command_registry_condition_wait(
    dcc_command_registry_condition_t *condition,
    dcc_command_registry_mutex_t *mutex,
    uint32_t timeout_ms) {
#if defined(_WIN32)
    DWORD timeout = timeout_ms == 0U ? INFINITE : (DWORD)timeout_ms;
    if (SleepConditionVariableCS(condition, mutex, timeout)) return 0;
    return GetLastError() == ERROR_TIMEOUT ? 1 : -1;
#else
    if (timeout_ms == 0U) return pthread_cond_wait(condition, mutex);
    struct timespec deadline;
    clock_gettime(CLOCK_REALTIME, &deadline);
    deadline.tv_sec += (time_t)(timeout_ms / 1000U);
    deadline.tv_nsec += (long)(timeout_ms % 1000U) * 1000000L;
    if (deadline.tv_nsec >= 1000000000L) {
        ++deadline.tv_sec;
        deadline.tv_nsec -= 1000000000L;
    }
    return pthread_cond_timedwait(condition, mutex, &deadline);
#endif
}

static dcc_command_registry_thread_id_t dcc_command_registry_thread_self(void) {
#if defined(_WIN32)
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

static int dcc_command_registry_thread_equal(
    dcc_command_registry_thread_id_t lhs,
    dcc_command_registry_thread_id_t rhs) {
#if defined(_WIN32)
    return lhs == rhs;
#else
    return pthread_equal(lhs, rhs);
#endif
}

static void dcc_command_registry_operation_free(
    dcc_command_registry_operation_t *operation) {
    if (operation == NULL) return;
    for (size_t i = 0U; i < operation->diff_count; ++i)
        free(operation->diffs[i].json);
    free(operation->diffs);
    if (operation->auth_token != NULL) {
        volatile char *bytes = (volatile char *)operation->auth_token;
        for (size_t i = 0U; operation->auth_token[i] != '\0'; ++i) bytes[i] = 0;
        free(operation->auth_token);
    }
    dcc_rest_result_free(operation->failed_result);
    dcc_command_registry_condition_destroy(&operation->condition);
    dcc_command_registry_mutex_destroy(&operation->mutex);
    free(operation);
}

static void dcc_command_registry_operation_release(
    dcc_command_registry_operation_t *operation) {
    int release = 0;
    dcc_command_registry_mutex_lock(&operation->mutex);
    if (--operation->references == 0U) release = 1;
    dcc_command_registry_mutex_unlock(&operation->mutex);
    if (release) dcc_command_registry_operation_free(operation);
}

static dcc_status_t dcc_command_registry_leg_path(
    const dcc_command_registry_operation_t *operation,
    const dcc_command_registry_owned_diff_t *diff,
    char *path, size_t path_size, dcc_rest_method_t *method) {
    int written = -1;
    switch (diff->action) {
    case DCC_COMMAND_REGISTRY_CREATE:
        *method = DCC_REST_POST;
        written = operation->guild_id != 0U
            ? snprintf(path, path_size, "/applications/%llu/guilds/%llu/commands",
                       (unsigned long long)operation->application_id,
                       (unsigned long long)operation->guild_id)
            : snprintf(path, path_size, "/applications/%llu/commands",
                       (unsigned long long)operation->application_id);
        break;
    case DCC_COMMAND_REGISTRY_UPDATE:
        if (diff->remote_id == 0U) return DCC_ERR_INVALID_ARG;
        *method = DCC_REST_PATCH;
        written = operation->guild_id != 0U
            ? snprintf(path, path_size,
                       "/applications/%llu/guilds/%llu/commands/%llu",
                       (unsigned long long)operation->application_id,
                       (unsigned long long)operation->guild_id,
                       (unsigned long long)diff->remote_id)
            : snprintf(path, path_size, "/applications/%llu/commands/%llu",
                       (unsigned long long)operation->application_id,
                       (unsigned long long)diff->remote_id);
        break;
    case DCC_COMMAND_REGISTRY_DELETE_STALE:
        if (diff->remote_id == 0U) return DCC_ERR_INVALID_ARG;
        *method = DCC_REST_DELETE;
        written = operation->guild_id != 0U
            ? snprintf(path, path_size,
                       "/applications/%llu/guilds/%llu/commands/%llu",
                       (unsigned long long)operation->application_id,
                       (unsigned long long)operation->guild_id,
                       (unsigned long long)diff->remote_id)
            : snprintf(path, path_size, "/applications/%llu/commands/%llu",
                       (unsigned long long)operation->application_id,
                       (unsigned long long)diff->remote_id);
        break;
    default:
        return DCC_ERR_INVALID_ARG;
    }
    return written < 0 || (size_t)written >= path_size
        ? DCC_ERR_INVALID_ARG : DCC_OK;
}

static void dcc_command_registry_complete(
    dcc_command_registry_operation_t *operation, dcc_status_t status,
    size_t failed_index, dcc_command_registry_diff_action_t failed_action,
    const dcc_rest_result_t *failed_result) {
    if (failed_result != NULL)
        (void)dcc_rest_result_clone(failed_result, &operation->failed_result);
    dcc_command_registry_mutex_lock(&operation->mutex);
    operation->result.status = status;
    operation->result.failed_plan_index = failed_index;
    operation->result.failed_action = failed_action;
    operation->result.failed_rest_result = operation->failed_result;
    operation->terminal = 1U;
    dcc_command_registry_condition_broadcast(&operation->condition);
    dcc_command_registry_operation_result_fn callback = operation->callback;
    void *user_data = operation->user_data;
    if (callback != NULL) {
        operation->in_callback = 1U;
        operation->callback_thread = dcc_command_registry_thread_self();
    }
    dcc_command_registry_mutex_unlock(&operation->mutex);
    if (callback != NULL) callback(operation->client, &operation->result, user_data);
    dcc_command_registry_mutex_lock(&operation->mutex);
    operation->in_callback = 0U;
    operation->callback_delivered = 1U;
    dcc_command_registry_condition_broadcast(&operation->condition);
    dcc_command_registry_mutex_unlock(&operation->mutex);
}

static void *dcc_command_registry_operation_main(void *opaque) {
    dcc_command_registry_operation_t *operation = opaque;
    if (operation->dry_run || operation->diff_count == 0U) {
        dcc_command_registry_complete(operation, DCC_OK, SIZE_MAX,
                                      DCC_COMMAND_REGISTRY_NOOP, NULL);
        dcc_command_registry_operation_release(operation);
        return NULL;
    }
    for (size_t i = 0U; i < operation->diff_count; ++i) {
        dcc_command_registry_mutex_lock(&operation->mutex);
        int canceled = operation->cancel_requested != 0U;
        dcc_command_registry_mutex_unlock(&operation->mutex);
        if (canceled) {
            dcc_command_registry_complete(operation, DCC_ERR_CANCELED, i,
                                          operation->diffs[i].action, NULL);
            dcc_command_registry_operation_release(operation);
            return NULL;
        }
        char path[192];
        dcc_rest_method_t method = DCC_REST_GET;
        dcc_status_t status = dcc_command_registry_leg_path(
            operation, &operation->diffs[i], path, sizeof(path), &method);
        dcc_rest_request_t *request = NULL;
        dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
        options.priority = operation->priority;
        options.auth_mode = operation->auth_mode;
        options.auth_token = operation->auth_token;
        dcc_rest_request_desc_t desc = DCC_REST_REQUEST_DESC_INIT;
        desc.method = method;
        desc.path = path;
        desc.options = &options;
        if (operation->diffs[i].action != DCC_COMMAND_REGISTRY_DELETE_STALE) {
            desc.content_type = "application/json";
            desc.body = operation->diffs[i].json;
            desc.body_len = strlen(operation->diffs[i].json);
        }
        if (status == DCC_OK) status = dcc_rest_submit(operation->client, &desc, &request);
        if (status != DCC_OK) {
            dcc_command_registry_complete(operation, status,
                                          operation->diffs[i].plan_index,
                                          operation->diffs[i].action, NULL);
            dcc_command_registry_operation_release(operation);
            return NULL;
        }
        dcc_command_registry_mutex_lock(&operation->mutex);
        operation->active_request = request;
        ++operation->result.submitted_count;
        canceled = operation->cancel_requested != 0U;
        dcc_command_registry_mutex_unlock(&operation->mutex);
        if (canceled) (void)dcc_rest_request_cancel(request);
        const dcc_rest_result_t *result = NULL;
        status = dcc_rest_request_wait(request, 0U, &result);
        if (status == DCC_OK) status = dcc_rest_result_status(result);
        dcc_command_registry_mutex_lock(&operation->mutex);
        operation->active_request = NULL;
        dcc_command_registry_mutex_unlock(&operation->mutex);
        if (status != DCC_OK) {
            dcc_status_t final_status = canceled || status == DCC_ERR_CANCELED
                ? DCC_ERR_CANCELED : status;
            dcc_command_registry_complete(operation, final_status,
                                          operation->diffs[i].plan_index,
                                          operation->diffs[i].action, result);
            dcc_rest_request_destroy(request);
            dcc_command_registry_operation_release(operation);
            return NULL;
        }
        ++operation->result.succeeded_count;
        if (operation->diffs[i].action == DCC_COMMAND_REGISTRY_CREATE)
            ++operation->result.create_completed;
        else if (operation->diffs[i].action == DCC_COMMAND_REGISTRY_UPDATE)
            ++operation->result.update_completed;
        else
            ++operation->result.delete_stale_completed;
        dcc_rest_request_destroy(request);
    }
    dcc_command_registry_complete(operation, DCC_OK, SIZE_MAX,
                                  DCC_COMMAND_REGISTRY_NOOP, NULL);
    dcc_command_registry_operation_release(operation);
    return NULL;
}

#if defined(_WIN32)
static DWORD WINAPI dcc_command_registry_operation_main_windows(LPVOID opaque) {
    (void)dcc_command_registry_operation_main(opaque);
    return 0U;
}
#endif

dcc_status_t dcc_command_registry_apply(
    dcc_client_t *client, dcc_snowflake_t application_id,
    const dcc_command_registry_plan_t *plan,
    const dcc_command_registry_operation_options_t *operation_options,
    dcc_command_registry_operation_t **out_operation) {
    if (out_operation != NULL) *out_operation = NULL;
    if (client == NULL || application_id == 0U || plan == NULL ||
        plan->size < sizeof(*plan) || (plan->diff_count != 0U && plan->diffs == NULL))
        return DCC_ERR_INVALID_ARG;
    dcc_command_registry_operation_options_t options =
        DCC_COMMAND_REGISTRY_OPERATION_OPTIONS_INIT;
    if (operation_options != NULL) {
        if (operation_options->size < sizeof(*operation_options) ||
            operation_options->version != DCC_COMMAND_REGISTRY_OPERATION_OPTIONS_VERSION)
            return DCC_ERR_INVALID_ARG;
        options = *operation_options;
    }
    if (options.auth_mode != DCC_REST_AUTH_DEFAULT &&
        options.auth_mode != DCC_REST_AUTH_BOT &&
        options.auth_mode != DCC_REST_AUTH_BEARER)
        return DCC_ERR_INVALID_ARG;
    if (options.auth_mode == DCC_REST_AUTH_BEARER &&
        (options.auth_token == NULL || options.auth_token[0] == '\0'))
        return DCC_ERR_INVALID_ARG;
    dcc_command_registry_operation_t *operation = calloc(1U, sizeof(*operation));
    if (operation == NULL) return DCC_ERR_NOMEM;
    if (dcc_command_registry_mutex_init(&operation->mutex) != 0) {
        free(operation);
        return DCC_ERR_RUNTIME;
    }
    if (dcc_command_registry_condition_init(&operation->condition) != 0) {
        dcc_command_registry_mutex_destroy(&operation->mutex);
        free(operation);
        return DCC_ERR_RUNTIME;
    }
    operation->client = client;
    operation->application_id = application_id;
    operation->guild_id = plan->guild_id;
    operation->dry_run = plan->dry_run;
    operation->priority = options.priority;
    operation->auth_mode = options.auth_mode;
    operation->callback = options.callback;
    operation->user_data = options.user_data;
    operation->result = (dcc_command_registry_operation_result_t)
        DCC_COMMAND_REGISTRY_OPERATION_RESULT_INIT;
    operation->result.noop_count = plan->noop_count;
    if (options.auth_token != NULL) {
        operation->auth_token = strdup(options.auth_token);
        if (operation->auth_token == NULL) {
            dcc_command_registry_operation_free(operation); return DCC_ERR_NOMEM;
        }
    }
    operation->diffs = calloc(plan->diff_count, sizeof(*operation->diffs));
    if (plan->diff_count != 0U && operation->diffs == NULL) {
        dcc_command_registry_operation_free(operation); return DCC_ERR_NOMEM;
    }
    for (size_t i = 0U; i < plan->diff_count; ++i) {
        const dcc_command_registry_diff_t *diff = &plan->diffs[i];
        if (diff->action == DCC_COMMAND_REGISTRY_NOOP) continue;
        dcc_command_registry_owned_diff_t *owned = &operation->diffs[operation->diff_count++];
        owned->action = diff->action;
        owned->remote_id = diff->remote_id;
        owned->plan_index = i;
        if (diff->action != DCC_COMMAND_REGISTRY_DELETE_STALE) {
            if (diff->local_json == NULL) {
                dcc_command_registry_operation_free(operation); return DCC_ERR_INVALID_ARG;
            }
            owned->json = strdup(diff->local_json);
            if (owned->json == NULL) {
                dcc_command_registry_operation_free(operation); return DCC_ERR_NOMEM;
            }
        }
    }
    operation->result.planned_count = operation->diff_count;
    operation->references = out_operation != NULL ? 2U : 1U;
    if (out_operation != NULL) *out_operation = operation;
#if defined(_WIN32)
    HANDLE thread = CreateThread(NULL, 0U,
                                 dcc_command_registry_operation_main_windows,
                                 operation, 0U, NULL);
    if (thread == NULL) {
#else
    pthread_t thread;
    if (pthread_create(&thread, NULL, dcc_command_registry_operation_main,
                       operation) != 0) {
#endif
        if (out_operation != NULL) *out_operation = NULL;
        operation->references = 1U;
        dcc_command_registry_operation_free(operation);
        return DCC_ERR_RUNTIME;
    }
#if defined(_WIN32)
    CloseHandle(thread);
#else
    (void)pthread_detach(thread);
#endif
    return DCC_OK;
}

dcc_status_t dcc_command_registry_operation_wait(
    dcc_command_registry_operation_t *operation, uint32_t timeout_ms,
    const dcc_command_registry_operation_result_t **out_result) {
    if (out_result != NULL) *out_result = NULL;
    if (operation == NULL || out_result == NULL) return DCC_ERR_INVALID_ARG;
    dcc_command_registry_mutex_lock(&operation->mutex);
    if (operation->in_callback &&
        dcc_command_registry_thread_equal(operation->callback_thread, dcc_command_registry_thread_self())) {
        dcc_command_registry_mutex_unlock(&operation->mutex); return DCC_ERR_STATE;
    }
    int rc = 0;
    while (!operation->callback_delivered && rc == 0)
        rc = dcc_command_registry_condition_wait(
            &operation->condition, &operation->mutex, timeout_ms);
    if (!operation->callback_delivered) {
        dcc_command_registry_mutex_unlock(&operation->mutex); return DCC_ERR_TIMEOUT;
    }
    *out_result = &operation->result;
    dcc_command_registry_mutex_unlock(&operation->mutex);
    return DCC_OK;
}

dcc_status_t dcc_command_registry_operation_cancel(
    dcc_command_registry_operation_t *operation) {
    if (operation == NULL) return DCC_ERR_INVALID_ARG;
    dcc_command_registry_mutex_lock(&operation->mutex);
    operation->cancel_requested = 1U;
    dcc_rest_request_t *request = operation->active_request;
    dcc_command_registry_mutex_unlock(&operation->mutex);
    return request != NULL ? dcc_rest_request_cancel(request) : DCC_OK;
}

uint8_t dcc_command_registry_operation_completed(
    const dcc_command_registry_operation_t *operation) {
    if (operation == NULL) return 0U;
    dcc_command_registry_mutex_lock(
        (dcc_command_registry_mutex_t *)&operation->mutex);
    uint8_t completed = operation->terminal;
    dcc_command_registry_mutex_unlock(
        (dcc_command_registry_mutex_t *)&operation->mutex);
    return completed;
}

void dcc_command_registry_operation_destroy(
    dcc_command_registry_operation_t *operation) {
    if (operation != NULL) dcc_command_registry_operation_release(operation);
}
