#include <dcc/client.h>
#include <dcc/rest/core/async.h>
#include <dcc/rest/request.h>

#include "internal/rest/dcc_rest_intercept_internal.h"
#include "internal/rest/dcc_rest_async_drain_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef enum request_intercept_mode {
    REQUEST_INTERCEPT_SUCCESS,
    REQUEST_INTERCEPT_NO_RESPONSE,
    REQUEST_INTERCEPT_INVALID_RESPONSE,
    REQUEST_INTERCEPT_TRANSPORT_FAILURE,
    REQUEST_INTERCEPT_RATE_LIMIT_ONCE,
    REQUEST_INTERCEPT_RATE_LIMIT_ALWAYS
} request_intercept_mode_t;

typedef struct request_intercept_state {
    atomic_int mode;
    atomic_uint delay_ms;
    atomic_uint calls;
    atomic_uint entered;
    atomic_uint observed_delay_ms;
    atomic_uint cooperative_hold;
    atomic_uint cooperative_release;
    char method[16];
    char path[128];
    char content_type[128];
    unsigned char body[128];
    size_t body_len;
} request_intercept_state_t;

typedef struct request_callback_state {
    atomic_uint called;
    atomic_uint stage;
    atomic_int recursive_wait;
    dcc_rest_request_t **request_slot;
    dcc_rest_result_t *clone;
    uint8_t destroy_in_callback;
} request_callback_state_t;

typedef struct request_runner {
    dcc_client_t *client;
    dcc_status_t status;
} request_runner_t;

typedef struct request_waiter {
    dcc_rest_request_t *request;
    dcc_status_t status;
    const dcc_rest_result_t *result;
} request_waiter_t;

typedef struct request_canceller {
    dcc_rest_request_t *request;
    dcc_status_t status;
} request_canceller_t;

typedef enum request_control_action {
    REQUEST_CONTROL_CANCEL,
    REQUEST_CONTROL_DESTROY
} request_control_action_t;

typedef struct request_start_gate {
    atomic_uint ready;
    atomic_uint release;
} request_start_gate_t;

typedef struct request_control {
    dcc_rest_request_t *request;
    request_control_action_t action;
    request_start_gate_t *start_gate;
    atomic_uint returned;
    dcc_status_t status;
    uint64_t elapsed_ms;
} request_control_t;

typedef struct request_client_destroyer {
    dcc_client_t *client;
    request_start_gate_t *start_gate;
    atomic_uint returned;
} request_client_destroyer_t;

typedef struct request_delivery_gate {
    atomic_uint callback_entered;
    atomic_uint callback_release;
    atomic_uint callback_called;
    atomic_uint observer_entered;
    atomic_uint observer_release;
    atomic_uint observer_called;
    atomic_uint stage;
    atomic_uint order_ok;
    atomic_int result_status;
} request_delivery_gate_t;

static uint64_t request_now_ms(void) {
    struct timespec now;
    (void)clock_gettime(CLOCK_MONOTONIC, &now);
    return (uint64_t)now.tv_sec * UINT64_C(1000) +
        (uint64_t)now.tv_nsec / UINT64_C(1000000);
}

static void request_sleep_ms(uint32_t ms) {
    struct timespec delay = {
        .tv_sec = (time_t)(ms / 1000U),
        .tv_nsec = (long)(ms % 1000U) * 1000000L,
    };
    while (nanosleep(&delay, &delay) != 0 && errno == EINTR) {
    }
}

static void *request_runner_main(void *arg) {
    request_runner_t *runner = (request_runner_t *)arg;
    runner->status = dcc_client_wait(runner->client);
    return NULL;
}

static void *request_waiter_main(void *arg) {
    request_waiter_t *waiter = (request_waiter_t *)arg;
    waiter->status = dcc_rest_request_wait(
        waiter->request,
        3000U,
        &waiter->result
    );
    return NULL;
}

static void *request_canceller_main(void *arg) {
    request_canceller_t *canceller = (request_canceller_t *)arg;
    canceller->status = dcc_rest_request_cancel(canceller->request);
    return NULL;
}

static void *request_control_main(void *arg) {
    request_control_t *control = (request_control_t *)arg;
    if (control->start_gate != NULL) {
        atomic_fetch_add_explicit(
            &control->start_gate->ready,
            1U,
            memory_order_acq_rel
        );
        while (atomic_load_explicit(
                &control->start_gate->release,
                memory_order_acquire
            ) == 0U) {
            request_sleep_ms(1U);
        }
    }
    uint64_t started_ms = request_now_ms();
    if (control->action == REQUEST_CONTROL_DESTROY) {
        dcc_rest_request_destroy(control->request);
        control->status = DCC_OK;
    } else {
        control->status = dcc_rest_request_cancel(control->request);
    }
    control->elapsed_ms = request_now_ms() - started_ms;
    atomic_store_explicit(&control->returned, 1U, memory_order_release);
    return NULL;
}

static void *request_client_destroyer_main(void *arg) {
    request_client_destroyer_t *destroyer = (request_client_destroyer_t *)arg;
    if (destroyer->start_gate != NULL) {
        atomic_fetch_add_explicit(
            &destroyer->start_gate->ready,
            1U,
            memory_order_acq_rel
        );
        while (atomic_load_explicit(
                &destroyer->start_gate->release,
                memory_order_acquire
            ) == 0U) {
            request_sleep_ms(1U);
        }
    }
    dcc_client_destroy(destroyer->client);
    atomic_store_explicit(&destroyer->returned, 1U, memory_order_release);
    return NULL;
}

static dcc_status_t request_intercept(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    dcc_rest_cb callback,
    void *callback_user_data,
    void *intercept_user_data
) {
    request_intercept_state_t *state =
        (request_intercept_state_t *)intercept_user_data;
    unsigned call_index = atomic_fetch_add_explicit(
        &state->calls,
        1U,
        memory_order_acq_rel
    );
    (void)snprintf(state->method, sizeof(state->method), "%s", method);
    (void)snprintf(state->path, sizeof(state->path), "%s", path);
    (void)snprintf(
        state->content_type,
        sizeof(state->content_type),
        "%s",
        content_type != NULL ? content_type : ""
    );
    state->body_len = body_len < sizeof(state->body) ? body_len : sizeof(state->body);
    if (state->body_len != 0U) {
        memcpy(state->body, body, state->body_len);
    }
    atomic_store_explicit(&state->entered, 1U, memory_order_release);
    while (atomic_load_explicit(&state->cooperative_hold, memory_order_acquire) != 0U &&
           atomic_load_explicit(&state->cooperative_release, memory_order_acquire) == 0U) {
        dcc_rest_sleep_ms(1U);
    }
    uint32_t delay_ms = atomic_load_explicit(&state->delay_ms, memory_order_acquire);
    atomic_store_explicit(&state->observed_delay_ms, delay_ms, memory_order_release);
    if (delay_ms != 0U) {
        request_sleep_ms(delay_ms);
    }

    request_intercept_mode_t mode = (request_intercept_mode_t)atomic_load_explicit(
        &state->mode,
        memory_order_acquire
    );
    if (mode == REQUEST_INTERCEPT_NO_RESPONSE) {
        return DCC_OK;
    }
    if (mode == REQUEST_INTERCEPT_TRANSPORT_FAILURE) {
        return DCC_ERR_NETWORK;
    }

    static const char success_body[] =
        "{\"code\":42,\"message\":\"kept\",\"ok\":true}";
    static const char limited_body[] =
        "{\"code\":20028,\"message\":\"rate limited\"}";
    int limited = mode == REQUEST_INTERCEPT_RATE_LIMIT_ALWAYS ||
        (mode == REQUEST_INTERCEPT_RATE_LIMIT_ONCE && call_index == 0U);
    dcc_rest_response_t response = {
        .size = sizeof(response),
        .status = mode == REQUEST_INTERCEPT_INVALID_RESPONSE
            ? 0U
            : limited ? 429U : 200U,
        .error = limited ? DCC_ERR_RATE_LIMITED : DCC_OK,
        .body = limited ? limited_body : success_body,
        .body_len = limited ? sizeof(limited_body) - 1U : sizeof(success_body) - 1U,
    };
    callback(client, &response, callback_user_data);

    /* A second response must never replace the first valid synchronous one. */
    dcc_rest_response_t duplicate = response;
    duplicate.status = 503U;
    duplicate.error = DCC_ERR_NETWORK;
    callback(client, &duplicate, callback_user_data);
    return DCC_OK;
}

static void request_result_callback(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
) {
    (void)client;
    request_callback_state_t *state = (request_callback_state_t *)user_data;
    atomic_fetch_add_explicit(&state->called, 1U, memory_order_acq_rel);
    if (state->clone == NULL) {
        (void)dcc_rest_result_clone(result, &state->clone);
    }
    if (state->request_slot != NULL && *state->request_slot != NULL) {
        const dcc_rest_result_t *recursive_result = (const dcc_rest_result_t *)(uintptr_t)1U;
        dcc_status_t recursive = dcc_rest_request_wait(
            *state->request_slot,
            1U,
            &recursive_result
        );
        atomic_store_explicit(&state->recursive_wait, recursive, memory_order_release);
        if (recursive_result != NULL) {
            atomic_store_explicit(&state->recursive_wait, DCC_ERR_RUNTIME, memory_order_release);
        }
        if (state->destroy_in_callback) {
            dcc_rest_request_destroy(*state->request_slot);
            *state->request_slot = NULL;
        }
    }
    atomic_store_explicit(&state->stage, 1U, memory_order_release);
}

static void request_error_observer(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    (void)client;
    request_callback_state_t *state = (request_callback_state_t *)user_data;
    if (error != NULL) {
        atomic_fetch_add_explicit(&state->called, 1U, memory_order_acq_rel);
        if (atomic_load_explicit(&state->stage, memory_order_acquire) == 1U) {
            atomic_store_explicit(&state->stage, 2U, memory_order_release);
        }
    }
}

static void request_gated_result_callback(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
) {
    (void)client;
    request_delivery_gate_t *gate = (request_delivery_gate_t *)user_data;
    atomic_store_explicit(
        &gate->result_status,
        result != NULL ? dcc_rest_result_status(result) : DCC_ERR_RUNTIME,
        memory_order_release
    );
    atomic_fetch_add_explicit(&gate->callback_called, 1U, memory_order_acq_rel);
    atomic_store_explicit(&gate->stage, 1U, memory_order_release);
    atomic_store_explicit(&gate->callback_entered, 1U, memory_order_release);
    while (atomic_load_explicit(&gate->callback_release, memory_order_acquire) == 0U) {
        request_sleep_ms(1U);
    }
}

static void request_gated_error_observer(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    (void)client;
    request_delivery_gate_t *gate = (request_delivery_gate_t *)user_data;
    if (error == NULL) {
        return;
    }
    atomic_fetch_add_explicit(&gate->observer_called, 1U, memory_order_acq_rel);
    if (atomic_load_explicit(&gate->stage, memory_order_acquire) != 1U) {
        atomic_store_explicit(&gate->order_ok, 0U, memory_order_release);
    }
    atomic_store_explicit(&gate->stage, 2U, memory_order_release);
    atomic_store_explicit(&gate->observer_entered, 1U, memory_order_release);
    while (atomic_load_explicit(&gate->observer_release, memory_order_acquire) == 0U) {
        request_sleep_ms(1U);
    }
}

static void request_delivery_gate_init(
    request_delivery_gate_t *gate,
    uint8_t gate_callback,
    uint8_t gate_observer
) {
    memset(gate, 0, sizeof(*gate));
    atomic_init(&gate->callback_entered, 0U);
    atomic_init(&gate->callback_release, gate_callback ? 0U : 1U);
    atomic_init(&gate->callback_called, 0U);
    atomic_init(&gate->observer_entered, 0U);
    atomic_init(&gate->observer_release, gate_observer ? 0U : 1U);
    atomic_init(&gate->observer_called, 0U);
    atomic_init(&gate->stage, 0U);
    atomic_init(&gate->order_ok, 1U);
    atomic_init(&gate->result_status, DCC_ERR_STATE);
}

static int request_wait_for_atomic(atomic_uint *value, uint32_t timeout_ms) {
    uint64_t started_ms = request_now_ms();
    while (atomic_load_explicit(value, memory_order_acquire) == 0U &&
           request_now_ms() - started_ms < timeout_ms) {
        request_sleep_ms(1U);
    }
    return atomic_load_explicit(value, memory_order_acquire) != 0U;
}

static int request_wait_for_atomic_at_least(
    atomic_uint *value,
    unsigned expected,
    uint32_t timeout_ms
) {
    uint64_t started_ms = request_now_ms();
    while (atomic_load_explicit(value, memory_order_acquire) < expected &&
           request_now_ms() - started_ms < timeout_ms) {
        request_sleep_ms(1U);
    }
    return atomic_load_explicit(value, memory_order_acquire) >= expected;
}

static void request_callback_state_init(request_callback_state_t *state) {
    memset(state, 0, sizeof(*state));
    atomic_init(&state->called, 0U);
    atomic_init(&state->stage, 0U);
    atomic_init(&state->recursive_wait, DCC_OK);
}

static int request_initializer_and_null_contract(void) {
    dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
    dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
    dcc_rest_request_t *request = (dcc_rest_request_t *)(void *)&description;
    const dcc_rest_result_t *result = (const dcc_rest_result_t *)(void *)&options;

    dcc_rest_call_options_init(&options);
    dcc_rest_request_desc_init(&description);
    if (options.size != sizeof(options) ||
        options.version != DCC_REST_CALL_OPTIONS_VERSION ||
        options.priority != DCC_REST_PRIORITY_NORMAL ||
        options.callback != NULL || options.user_data != NULL ||
        description.size != sizeof(description) ||
        description.version != DCC_REST_REQUEST_DESC_VERSION ||
        description.method != DCC_REST_GET ||
        description.path != NULL || description.content_type != NULL ||
        description.body != NULL || description.body_len != 0U ||
        description.options.size != sizeof(description.options) ||
        description.options.version != DCC_REST_CALL_OPTIONS_VERSION ||
        description.options.priority != DCC_REST_PRIORITY_NORMAL ||
        description.options.callback != NULL ||
        description.options.user_data != NULL) {
        return 1;
    }
    dcc_rest_call_options_init(NULL);
    dcc_rest_request_desc_init(NULL);
    if (dcc_rest_submit(NULL, &description, &request) != DCC_ERR_INVALID_ARG ||
        request != NULL ||
        dcc_rest_request_wait(NULL, 1U, &result) != DCC_ERR_INVALID_ARG ||
        result != NULL ||
        dcc_rest_request_cancel(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_rest_request_completed(NULL) != 0U) {
        return 1;
    }
    dcc_rest_request_destroy(NULL);
    return 0;
}

static int request_expect_rejected(
    dcc_client_t *client,
    const dcc_rest_request_desc_t *description
) {
    dcc_rest_request_t *request = (dcc_rest_request_t *)(uintptr_t)1U;
    return dcc_rest_submit(client, description, &request) == DCC_ERR_INVALID_ARG &&
        request == NULL
        ? 0
        : 1;
}

static int request_validation_contract(dcc_client_t *client) {
    dcc_rest_request_desc_t valid = DCC_REST_REQUEST_DESC_INIT;
    valid.path = "/valid";
    dcc_rest_request_desc_t bad = valid;

    if (request_expect_rejected(client, NULL) != 0) return 1;
    bad.size = sizeof(bad) - 1U;
    if (request_expect_rejected(client, &bad) != 0) return 1;
    bad = valid; bad.version++;
    if (request_expect_rejected(client, &bad) != 0) return 1;
    bad = valid; bad.method = (dcc_rest_method_t)99;
    if (request_expect_rejected(client, &bad) != 0) return 1;
    bad = valid; bad.path = NULL;
    if (request_expect_rejected(client, &bad) != 0) return 1;
    bad = valid; bad.path = "";
    if (request_expect_rejected(client, &bad) != 0) return 1;
    bad = valid; bad.body_len = 1U;
    if (request_expect_rejected(client, &bad) != 0) return 1;
    bad = valid; bad.options.size = sizeof(bad.options) - 1U;
    if (request_expect_rejected(client, &bad) != 0) return 1;
    bad = valid; bad.options.version++;
    if (request_expect_rejected(client, &bad) != 0) return 1;
    bad = valid; bad.options.priority = (dcc_rest_priority_t)99;
    if (request_expect_rejected(client, &bad) != 0) return 1;
    bad = valid; bad.options.user_data = &bad;
    if (request_expect_rejected(client, &bad) != 0) return 1;
    bad = valid;
    bad.size = sizeof(bad) + 64U;
    bad.options.size = sizeof(bad.options) + 64U;
    dcc_rest_request_t *request = (dcc_rest_request_t *)(uintptr_t)1U;
    if (dcc_rest_submit(client, &bad, &request) != DCC_ERR_STATE || request != NULL) {
        return 1;
    }
    return 0;
}

static int request_delayed_copy_wait_contract(
    dcc_client_t *client,
    request_intercept_state_t *intercept
) {
    char path[64] = "/copied-path";
    char content_type[64] = "application/octet-stream";
    unsigned char body[] = {'A', 0U, 'B', 0xffU, 'C'};
    const unsigned char expected_body[] = {'A', 0U, 'B', 0xffU, 'C'};
    request_callback_state_t callback;
    request_callback_state_init(&callback);
    dcc_rest_request_t *request = NULL;
    callback.request_slot = &request;

    atomic_store_explicit(&intercept->mode, REQUEST_INTERCEPT_SUCCESS, memory_order_release);
    atomic_store_explicit(&intercept->delay_ms, 250U, memory_order_release);
    atomic_store_explicit(&intercept->calls, 0U, memory_order_release);
    atomic_store_explicit(&intercept->entered, 0U, memory_order_release);

    dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
    description.method = DCC_REST_POST;
    description.path = path;
    description.content_type = content_type;
    description.body = body;
    description.body_len = sizeof(body);
    description.options.callback = request_result_callback;
    description.options.user_data = &callback;

    uint64_t started_ms = request_now_ms();
    dcc_status_t status = dcc_rest_submit(client, &description, &request);
    uint64_t submit_ms = request_now_ms() - started_ms;
    memset(path, 'x', sizeof(path));
    memset(content_type, 'y', sizeof(content_type));
    memset(body, 'z', sizeof(body));

    const dcc_rest_result_t *result = (const dcc_rest_result_t *)(uintptr_t)1U;
    dcc_status_t timeout_status = request != NULL
        ? dcc_rest_request_wait(request, 10U, &result)
        : DCC_ERR_STATE;
    request_waiter_t waiter_a = {.request = request, .status = DCC_ERR_STATE};
    request_waiter_t waiter_b = {.request = request, .status = DCC_ERR_STATE};
    pthread_t thread_a;
    pthread_t thread_b;
    int thread_a_ok = request != NULL && pthread_create(&thread_a, NULL, request_waiter_main, &waiter_a) == 0;
    int thread_b_ok = request != NULL && pthread_create(&thread_b, NULL, request_waiter_main, &waiter_b) == 0;
    if (thread_a_ok) (void)pthread_join(thread_a, NULL);
    if (thread_b_ok) (void)pthread_join(thread_b, NULL);

    const dcc_rest_result_t *repeated = NULL;
    dcc_status_t repeated_status = request != NULL
        ? dcc_rest_request_wait(request, 1U, &repeated)
        : DCC_ERR_STATE;
    int ok = status == DCC_OK && submit_ms < 100U && timeout_status == DCC_ERR_TIMEOUT &&
        result == NULL && thread_a_ok && thread_b_ok && waiter_a.status == DCC_OK &&
        waiter_b.status == DCC_OK && waiter_a.result == waiter_b.result &&
        repeated_status == DCC_OK && repeated == waiter_a.result &&
        dcc_rest_request_wait(request, 1U, NULL) == DCC_ERR_INVALID_ARG &&
        dcc_rest_request_completed(request) &&
        atomic_load_explicit(&callback.called, memory_order_acquire) == 1U &&
        atomic_load_explicit(&callback.recursive_wait, memory_order_acquire) == DCC_ERR_STATE &&
        repeated != NULL && repeated->http_status == 200U &&
        dcc_rest_result_status(repeated) == DCC_OK && repeated->discord_code == 42 &&
        repeated->discord_message != NULL && strcmp(repeated->discord_message, "kept") == 0 &&
        strcmp(intercept->method, "POST") == 0 &&
        strcmp(intercept->path, "/copied-path") == 0 &&
        strcmp(intercept->content_type, "application/octet-stream") == 0 &&
        intercept->body_len == sizeof(expected_body) &&
        memcmp(intercept->body, expected_body, sizeof(expected_body)) == 0 &&
        callback.clone != NULL;

    if (!ok) {
        fprintf(
            stderr,
            "detail submit=%s ms=%llu timeout=%s result=%p threads=%d/%d "
            "wait=%s/%s repeat=%s completed=%u called=%u recursive=%s "
            "http=%u mapped=%s code=%d message=%s method=%s path=%s type=%s "
            "body=%zu clone=%p delay=%u/%u\n",
            dcc_status_string(status),
            (unsigned long long)submit_ms,
            dcc_status_string(timeout_status),
            (void *)result,
            thread_a_ok,
            thread_b_ok,
            dcc_status_string(waiter_a.status),
            dcc_status_string(waiter_b.status),
            dcc_status_string(repeated_status),
            dcc_rest_request_completed(request),
            atomic_load_explicit(&callback.called, memory_order_acquire),
            dcc_status_string((dcc_status_t)atomic_load_explicit(&callback.recursive_wait, memory_order_acquire)),
            repeated != NULL ? repeated->http_status : 0U,
            repeated != NULL ? dcc_status_string(dcc_rest_result_status(repeated)) : "null",
            repeated != NULL ? repeated->discord_code : 0,
            repeated != NULL && repeated->discord_message != NULL ? repeated->discord_message : "null",
            intercept->method,
            intercept->path,
            intercept->content_type,
            intercept->body_len,
            (void *)callback.clone,
            atomic_load_explicit(&intercept->delay_ms, memory_order_acquire),
            atomic_load_explicit(&intercept->observed_delay_ms, memory_order_acquire)
        );
    }

    dcc_rest_request_destroy(request);
    if (callback.clone == NULL || callback.clone->body_len == 0U) {
        ok = 0;
    }
    dcc_rest_result_free(callback.clone);
    return ok ? 0 : 1;
}

static void request_intercept_reset(
    request_intercept_state_t *intercept,
    request_intercept_mode_t mode,
    uint32_t delay_ms
) {
    atomic_store_explicit(&intercept->mode, mode, memory_order_release);
    atomic_store_explicit(&intercept->delay_ms, delay_ms, memory_order_release);
    atomic_store_explicit(&intercept->calls, 0U, memory_order_release);
    atomic_store_explicit(&intercept->entered, 0U, memory_order_release);
    atomic_store_explicit(&intercept->observed_delay_ms, 0U, memory_order_release);
    atomic_store_explicit(&intercept->cooperative_hold, 0U, memory_order_release);
    atomic_store_explicit(&intercept->cooperative_release, 1U, memory_order_release);
}

static int request_wait_for_entered(request_intercept_state_t *intercept) {
    uint64_t started = request_now_ms();
    while (atomic_load_explicit(&intercept->entered, memory_order_acquire) == 0U &&
           request_now_ms() - started < 1000U) {
        request_sleep_ms(1U);
    }
    return atomic_load_explicit(&intercept->entered, memory_order_acquire) != 0U;
}

static dcc_status_t request_submit_test(
    dcc_client_t *client,
    const char *path,
    request_callback_state_t *callback,
    dcc_rest_request_t **out_request
) {
    dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
    description.path = path;
    if (callback != NULL) {
        description.options.callback = request_result_callback;
        description.options.user_data = callback;
    }
    return dcc_rest_submit(client, &description, out_request);
}

static dcc_status_t request_submit_gated(
    dcc_client_t *client,
    const char *path,
    request_delivery_gate_t *gate,
    dcc_rest_request_t **out_request
) {
    dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
    description.path = path;
    description.options.callback = request_gated_result_callback;
    description.options.user_data = gate;
    return dcc_rest_submit(client, &description, out_request);
}

static void request_callback_state_deinit(request_callback_state_t *state) {
    dcc_rest_result_free(state->clone);
    state->clone = NULL;
}

static int request_nonblocking_cancel_destroy_contract(
    dcc_client_t *client,
    request_intercept_state_t *intercept
) {
    int ok = 1;
    dcc_rest_request_t *blocker = NULL;
    dcc_rest_request_t *pending = NULL;
    request_delivery_gate_t cancel_gate;
    request_delivery_gate_init(&cancel_gate, 1U, 0U);
    request_intercept_reset(intercept, REQUEST_INTERCEPT_SUCCESS, 300U);
    if (dcc_client_on_error(client, request_gated_error_observer, &cancel_gate) != DCC_OK ||
        request_submit_test(client, "/nonblocking-cancel-blocker", NULL, &blocker) != DCC_OK ||
        !request_wait_for_entered(intercept) ||
        request_submit_gated(client, "/nonblocking-cancel", &cancel_gate, &pending) != DCC_OK) {
        ok = 0;
    }
    /* The blocker owns the only slot. Its completion consumes this injected
     * rejection while trying to run the canceled job; the queue must retain
     * and retry that job without falling back to caller-stack delivery. */
    dcc_rest_test_fail_next_worker_spawn(client);

    request_control_t cancel_control = {
        .request = pending,
        .action = REQUEST_CONTROL_CANCEL,
        .status = DCC_ERR_STATE,
    };
    atomic_init(&cancel_control.returned, 0U);
    pthread_t cancel_thread;
    int cancel_started = ok && pthread_create(
        &cancel_thread,
        NULL,
        request_control_main,
        &cancel_control
    ) == 0;
    int callback_entered = cancel_started && request_wait_for_atomic(
        &cancel_gate.callback_entered,
        1500U
    );
    int cancel_returned_before_release = callback_entered &&
        atomic_load_explicit(&cancel_control.returned, memory_order_acquire) != 0U;
    atomic_store_explicit(&cancel_gate.callback_release, 1U, memory_order_release);
    atomic_store_explicit(&cancel_gate.observer_release, 1U, memory_order_release);
    if (cancel_started) {
        (void)pthread_join(cancel_thread, NULL);
    }

    const dcc_rest_result_t *pending_result = NULL;
    const dcc_rest_result_t *blocker_result = NULL;
    dcc_status_t pending_wait_status = pending != NULL
        ? dcc_rest_request_wait(pending, 2000U, &pending_result)
        : DCC_ERR_STATE;
    dcc_status_t blocker_wait_status = blocker != NULL
        ? dcc_rest_request_wait(blocker, 2000U, &blocker_result)
        : DCC_ERR_STATE;
    dcc_status_t cancel_drain_status = dcc_rest_async_wait(client, 2000U);
    if (!cancel_started || !callback_entered || !cancel_returned_before_release ||
        cancel_control.status != DCC_OK ||
        pending == NULL || pending_wait_status != DCC_OK ||
        pending_result == NULL ||
        dcc_rest_result_status(pending_result) != DCC_ERR_CANCELED ||
        blocker == NULL || blocker_wait_status != DCC_OK ||
        blocker_result == NULL || dcc_rest_result_status(blocker_result) != DCC_OK ||
        cancel_drain_status != DCC_OK ||
        atomic_load_explicit(&cancel_gate.callback_called, memory_order_acquire) != 1U ||
        atomic_load_explicit(&cancel_gate.observer_called, memory_order_acquire) != 1U ||
        atomic_load_explicit(&cancel_gate.stage, memory_order_acquire) != 2U ||
        atomic_load_explicit(&cancel_gate.order_ok, memory_order_acquire) != 1U ||
        atomic_load_explicit(&cancel_gate.result_status, memory_order_acquire) != DCC_ERR_CANCELED ||
        atomic_load_explicit(&intercept->calls, memory_order_acquire) != 1U) {
        fprintf(
            stderr,
            "nonblocking cancel detail started=%d entered=%d returned=%d status=%s "
            "elapsed=%llu callback=%u observer=%u stage=%u order=%u result=%s calls=%u\n",
            cancel_started,
            callback_entered,
            cancel_returned_before_release,
            dcc_status_string(cancel_control.status),
            (unsigned long long)cancel_control.elapsed_ms,
            atomic_load_explicit(&cancel_gate.callback_called, memory_order_acquire),
            atomic_load_explicit(&cancel_gate.observer_called, memory_order_acquire),
            atomic_load_explicit(&cancel_gate.stage, memory_order_acquire),
            atomic_load_explicit(&cancel_gate.order_ok, memory_order_acquire),
            dcc_status_string((dcc_status_t)atomic_load_explicit(
                &cancel_gate.result_status,
                memory_order_acquire
            )),
            atomic_load_explicit(&intercept->calls, memory_order_acquire)
        );
        ok = 0;
    }
    dcc_rest_request_destroy(pending);
    dcc_rest_request_destroy(blocker);

    blocker = NULL;
    dcc_rest_request_t *destroyed_pending = NULL;
    request_delivery_gate_t destroy_gate;
    request_delivery_gate_init(&destroy_gate, 0U, 1U);
    request_intercept_reset(intercept, REQUEST_INTERCEPT_SUCCESS, 300U);
    if (dcc_client_on_error(client, request_gated_error_observer, &destroy_gate) != DCC_OK ||
        request_submit_test(client, "/nonblocking-destroy-blocker", NULL, &blocker) != DCC_OK ||
        !request_wait_for_entered(intercept) ||
        request_submit_gated(
            client,
            "/nonblocking-destroy",
            &destroy_gate,
            &destroyed_pending
        ) != DCC_OK) {
        ok = 0;
    }

    request_control_t destroy_control = {
        .request = destroyed_pending,
        .action = REQUEST_CONTROL_DESTROY,
        .status = DCC_ERR_STATE,
    };
    atomic_init(&destroy_control.returned, 0U);
    pthread_t destroy_thread;
    int destroy_started = destroyed_pending != NULL && pthread_create(
        &destroy_thread,
        NULL,
        request_control_main,
        &destroy_control
    ) == 0;
    int observer_entered = destroy_started && request_wait_for_atomic(
        &destroy_gate.observer_entered,
        1500U
    );
    int destroy_returned_before_release = observer_entered &&
        atomic_load_explicit(&destroy_control.returned, memory_order_acquire) != 0U;
    atomic_store_explicit(&destroy_gate.callback_release, 1U, memory_order_release);
    atomic_store_explicit(&destroy_gate.observer_release, 1U, memory_order_release);
    if (destroy_started) {
        (void)pthread_join(destroy_thread, NULL);
        destroyed_pending = NULL;
    } else {
        dcc_rest_request_destroy(destroyed_pending);
        destroyed_pending = NULL;
    }

    blocker_result = NULL;
    blocker_wait_status = blocker != NULL
        ? dcc_rest_request_wait(blocker, 2000U, &blocker_result)
        : DCC_ERR_STATE;
    dcc_status_t destroy_drain_status = dcc_rest_async_wait(client, 2000U);
    if (!destroy_started || !observer_entered || !destroy_returned_before_release ||
        destroy_control.status != DCC_OK ||
        blocker == NULL || blocker_wait_status != DCC_OK ||
        blocker_result == NULL || dcc_rest_result_status(blocker_result) != DCC_OK ||
        destroy_drain_status != DCC_OK ||
        atomic_load_explicit(&destroy_gate.callback_called, memory_order_acquire) != 1U ||
        atomic_load_explicit(&destroy_gate.observer_called, memory_order_acquire) != 1U ||
        atomic_load_explicit(&destroy_gate.stage, memory_order_acquire) != 2U ||
        atomic_load_explicit(&destroy_gate.order_ok, memory_order_acquire) != 1U ||
        atomic_load_explicit(&destroy_gate.result_status, memory_order_acquire) != DCC_ERR_CANCELED ||
        atomic_load_explicit(&intercept->calls, memory_order_acquire) != 1U) {
        fprintf(
            stderr,
            "nonblocking destroy detail started=%d entered=%d returned=%d status=%s "
            "elapsed=%llu callback=%u observer=%u stage=%u order=%u result=%s calls=%u\n",
            destroy_started,
            observer_entered,
            destroy_returned_before_release,
            dcc_status_string(destroy_control.status),
            (unsigned long long)destroy_control.elapsed_ms,
            atomic_load_explicit(&destroy_gate.callback_called, memory_order_acquire),
            atomic_load_explicit(&destroy_gate.observer_called, memory_order_acquire),
            atomic_load_explicit(&destroy_gate.stage, memory_order_acquire),
            atomic_load_explicit(&destroy_gate.order_ok, memory_order_acquire),
            dcc_status_string((dcc_status_t)atomic_load_explicit(
                &destroy_gate.result_status,
                memory_order_acquire
            )),
            atomic_load_explicit(&intercept->calls, memory_order_acquire)
        );
        ok = 0;
    }
    dcc_rest_request_destroy(blocker);
    (void)dcc_client_on_error(client, NULL, NULL);
    return ok ? 0 : 1;
}

static int request_cancel_contract(
    dcc_client_t *client,
    request_intercept_state_t *intercept
) {
    int ok = 1;
    dcc_rest_request_t *blocker = NULL;
    dcc_rest_request_t *pending = NULL;
    request_callback_state_t pending_callback;
    request_callback_state_init(&pending_callback);
    pending_callback.request_slot = &pending;

    request_intercept_reset(intercept, REQUEST_INTERCEPT_SUCCESS, 250U);
    if (request_submit_test(client, "/cancel-blocker", NULL, &blocker) != DCC_OK ||
        !request_wait_for_entered(intercept) ||
        request_submit_test(client, "/cancel-pending", &pending_callback, &pending) != DCC_OK ||
        dcc_rest_request_cancel(pending) != DCC_OK ||
        dcc_rest_request_cancel(pending) != DCC_OK) {
        ok = 0;
    }
    const dcc_rest_result_t *pending_result = NULL;
    if (pending == NULL ||
        dcc_rest_request_wait(pending, 1000U, &pending_result) != DCC_OK ||
        pending_result == NULL ||
        dcc_rest_result_status(pending_result) != DCC_ERR_CANCELED ||
        atomic_load_explicit(&pending_callback.called, memory_order_acquire) != 1U ||
        atomic_load_explicit(&intercept->calls, memory_order_acquire) != 1U) {
        ok = 0;
    }
    const dcc_rest_result_t *blocker_result = NULL;
    if (blocker == NULL ||
        dcc_rest_request_wait(blocker, 2000U, &blocker_result) != DCC_OK ||
        blocker_result == NULL || dcc_rest_result_status(blocker_result) != DCC_OK) {
        ok = 0;
    }
    dcc_rest_request_destroy(pending);
    dcc_rest_request_destroy(blocker);
    request_callback_state_deinit(&pending_callback);

    dcc_rest_request_t *active = NULL;
    request_callback_state_t active_callback;
    request_callback_state_init(&active_callback);
    active_callback.request_slot = &active;
    request_intercept_reset(intercept, REQUEST_INTERCEPT_SUCCESS, 250U);
    if (request_submit_test(client, "/cancel-active", &active_callback, &active) != DCC_OK ||
        !request_wait_for_entered(intercept) ||
        dcc_rest_request_cancel(active) != DCC_OK ||
        dcc_rest_request_cancel(active) != DCC_OK) {
        ok = 0;
    }
    const dcc_rest_result_t *active_result = NULL;
    if (active == NULL ||
        dcc_rest_request_wait(active, 2000U, &active_result) != DCC_OK ||
        active_result == NULL ||
        dcc_rest_result_status(active_result) != DCC_ERR_CANCELED ||
        atomic_load_explicit(&active_callback.called, memory_order_acquire) != 1U) {
        ok = 0;
    }
    dcc_rest_request_destroy(active);
    request_callback_state_deinit(&active_callback);

    for (unsigned i = 0U; i < 20U; ++i) {
        dcc_rest_request_t *raced = NULL;
        request_callback_state_t raced_callback;
        request_callback_state_init(&raced_callback);
        raced_callback.request_slot = &raced;
        request_intercept_reset(intercept, REQUEST_INTERCEPT_SUCCESS, i % 2U);
        if (request_submit_test(client, "/cancel-race", &raced_callback, &raced) != DCC_OK) {
            ok = 0;
            break;
        }
        request_canceller_t canceller = {.request = raced, .status = DCC_ERR_STATE};
        pthread_t cancel_thread;
        int cancel_started = pthread_create(
            &cancel_thread,
            NULL,
            request_canceller_main,
            &canceller
        ) == 0;
        const dcc_rest_result_t *raced_result = NULL;
        dcc_status_t wait_status = dcc_rest_request_wait(raced, 2000U, &raced_result);
        if (cancel_started) {
            (void)pthread_join(cancel_thread, NULL);
        }
        dcc_status_t outcome = raced_result != NULL
            ? dcc_rest_result_status(raced_result)
            : DCC_ERR_RUNTIME;
        if (!cancel_started || canceller.status != DCC_OK || wait_status != DCC_OK ||
            (outcome != DCC_OK && outcome != DCC_ERR_CANCELED) ||
            atomic_load_explicit(&raced_callback.called, memory_order_acquire) != 1U) {
            ok = 0;
        }
        dcc_rest_request_destroy(raced);
        request_callback_state_deinit(&raced_callback);
        if (!ok) break;
    }
    return ok ? 0 : 1;
}

static int request_destroy_and_auto_release_contract(
    dcc_client_t *client,
    request_intercept_state_t *intercept
) {
    int ok = 1;
    request_callback_state_t destroyed_active;
    request_callback_state_init(&destroyed_active);
    dcc_rest_request_t *active = NULL;
    request_intercept_reset(intercept, REQUEST_INTERCEPT_SUCCESS, 150U);
    if (request_submit_test(client, "/destroy-active", &destroyed_active, &active) != DCC_OK ||
        !request_wait_for_entered(intercept)) {
        ok = 0;
    }
    dcc_rest_request_destroy(active);
    active = NULL;
    if (dcc_rest_async_wait(client, 2000U) != DCC_OK ||
        atomic_load_explicit(&destroyed_active.called, memory_order_acquire) != 1U ||
        destroyed_active.clone == NULL ||
        dcc_rest_result_status(destroyed_active.clone) != DCC_ERR_CANCELED) {
        ok = 0;
    }
    request_callback_state_deinit(&destroyed_active);

    dcc_rest_request_t *blocker = NULL;
    dcc_rest_request_t *destroyed_pending_request = NULL;
    request_callback_state_t destroyed_pending;
    request_callback_state_init(&destroyed_pending);
    request_intercept_reset(intercept, REQUEST_INTERCEPT_SUCCESS, 150U);
    if (request_submit_test(client, "/destroy-pending-blocker", NULL, &blocker) != DCC_OK ||
        !request_wait_for_entered(intercept) ||
        request_submit_test(
            client,
            "/destroy-pending",
            &destroyed_pending,
            &destroyed_pending_request
        ) != DCC_OK) {
        ok = 0;
    }
    dcc_rest_request_destroy(destroyed_pending_request);
    destroyed_pending_request = NULL;
    const dcc_rest_result_t *blocker_result = NULL;
    if (dcc_rest_request_wait(blocker, 2000U, &blocker_result) != DCC_OK ||
        dcc_rest_async_wait(client, 2000U) != DCC_OK ||
        atomic_load_explicit(&destroyed_pending.called, memory_order_acquire) != 1U ||
        destroyed_pending.clone == NULL ||
        dcc_rest_result_status(destroyed_pending.clone) != DCC_ERR_CANCELED) {
        ok = 0;
    }
    dcc_rest_request_destroy(blocker);
    request_callback_state_deinit(&destroyed_pending);

    dcc_rest_request_t *callback_destroyed = NULL;
    request_callback_state_t callback_destroy;
    request_callback_state_init(&callback_destroy);
    callback_destroy.request_slot = &callback_destroyed;
    callback_destroy.destroy_in_callback = 1U;
    request_intercept_reset(intercept, REQUEST_INTERCEPT_SUCCESS, 0U);
    if (request_submit_test(
            client,
            "/destroy-from-callback",
            &callback_destroy,
            &callback_destroyed
        ) != DCC_OK ||
        dcc_rest_async_wait(client, 2000U) != DCC_OK ||
        callback_destroyed != NULL ||
        atomic_load_explicit(&callback_destroy.called, memory_order_acquire) != 1U) {
        ok = 0;
    }
    request_callback_state_deinit(&callback_destroy);

    request_callback_state_t callback_only;
    request_callback_state_init(&callback_only);
    request_intercept_reset(intercept, REQUEST_INTERCEPT_SUCCESS, 0U);
    if (request_submit_test(client, "/callback-only", &callback_only, NULL) != DCC_OK ||
        dcc_rest_async_wait(client, 2000U) != DCC_OK ||
        atomic_load_explicit(&callback_only.called, memory_order_acquire) != 1U) {
        ok = 0;
    }
    request_callback_state_deinit(&callback_only);
    return ok ? 0 : 1;
}

static int request_error_admission_and_retry_contract(
    dcc_client_t *client,
    request_intercept_state_t *intercept
) {
    int ok = 1;
    request_callback_state_t observed;
    request_callback_state_init(&observed);
    if (dcc_client_on_error(client, request_error_observer, &observed) != DCC_OK) {
        return 1;
    }

    request_intercept_reset(intercept, REQUEST_INTERCEPT_TRANSPORT_FAILURE, 0U);
    if (request_submit_test(client, "/fire-observe", NULL, NULL) != DCC_OK ||
        dcc_rest_async_wait(client, 2000U) != DCC_OK ||
        atomic_load_explicit(&observed.called, memory_order_acquire) != 1U) {
        ok = 0;
    }

    request_callback_state_t malformed;
    request_callback_state_init(&malformed);
    dcc_rest_request_t *malformed_request = NULL;
    malformed.request_slot = &malformed_request;
    if (dcc_client_on_error(client, request_error_observer, &malformed) != DCC_OK) {
        ok = 0;
    }
    request_intercept_reset(intercept, REQUEST_INTERCEPT_NO_RESPONSE, 0U);
    if (request_submit_test(
            client,
            "/no-response",
            &malformed,
            &malformed_request
        ) != DCC_OK) {
        ok = 0;
    }
    const dcc_rest_result_t *malformed_result = NULL;
    if (malformed_request == NULL ||
        dcc_rest_request_wait(malformed_request, 2000U, &malformed_result) != DCC_OK ||
        malformed_result == NULL ||
        dcc_rest_result_status(malformed_result) != DCC_ERR_RUNTIME ||
        atomic_load_explicit(&malformed.called, memory_order_acquire) != 2U ||
        atomic_load_explicit(&malformed.stage, memory_order_acquire) != 2U) {
        ok = 0;
    }
    dcc_rest_request_destroy(malformed_request);
    request_callback_state_deinit(&malformed);

    request_callback_state_t invalid_response;
    request_callback_state_init(&invalid_response);
    dcc_rest_request_t *invalid_response_request = NULL;
    invalid_response.request_slot = &invalid_response_request;
    if (dcc_client_on_error(client, request_error_observer, &invalid_response) != DCC_OK) {
        ok = 0;
    }
    request_intercept_reset(intercept, REQUEST_INTERCEPT_INVALID_RESPONSE, 0U);
    if (request_submit_test(
            client,
            "/invalid-response",
            &invalid_response,
            &invalid_response_request
        ) != DCC_OK) {
        ok = 0;
    }
    const dcc_rest_result_t *invalid_response_result = NULL;
    if (invalid_response_request == NULL ||
        dcc_rest_request_wait(
            invalid_response_request,
            2000U,
            &invalid_response_result
        ) != DCC_OK ||
        invalid_response_result == NULL ||
        dcc_rest_result_status(invalid_response_result) != DCC_ERR_RUNTIME ||
        atomic_load_explicit(&invalid_response.called, memory_order_acquire) != 2U ||
        atomic_load_explicit(&invalid_response.stage, memory_order_acquire) != 2U) {
        ok = 0;
    }
    dcc_rest_request_destroy(invalid_response_request);
    request_callback_state_deinit(&invalid_response);

    request_callback_state_t rejected;
    request_callback_state_init(&rejected);
    if (dcc_client_on_error(client, request_error_observer, &rejected) != DCC_OK) {
        ok = 0;
    }
    dcc_rest_request_desc_t rejected_description = DCC_REST_REQUEST_DESC_INIT;
    rejected_description.path = "/spawn-rejected";
    rejected_description.options.callback = request_result_callback;
    rejected_description.options.user_data = &rejected;
    dcc_rest_request_t *rejected_request = (dcc_rest_request_t *)(uintptr_t)1U;
    dcc_rest_test_fail_next_worker_spawn(client);
    dcc_status_t rejected_status = dcc_rest_submit(
        client,
        &rejected_description,
        &rejected_request
    );
    request_sleep_ms(10U);
    if (rejected_status != DCC_ERR_RUNTIME || rejected_request != NULL ||
        atomic_load_explicit(&rejected.called, memory_order_acquire) != 0U) {
        ok = 0;
    }

    request_callback_state_t retried;
    request_callback_state_init(&retried);
    dcc_rest_request_t *retried_request = NULL;
    retried.request_slot = &retried_request;
    if (dcc_client_on_error(client, request_error_observer, &retried) != DCC_OK) {
        ok = 0;
    }
    request_intercept_reset(intercept, REQUEST_INTERCEPT_RATE_LIMIT_ONCE, 0U);
    if (request_submit_test(client, "/retry-success", &retried, &retried_request) != DCC_OK) {
        ok = 0;
    }
    const dcc_rest_result_t *retried_result = NULL;
    if (retried_request == NULL ||
        dcc_rest_request_wait(retried_request, 3000U, &retried_result) != DCC_OK ||
        retried_result == NULL || dcc_rest_result_status(retried_result) != DCC_OK ||
        atomic_load_explicit(&intercept->calls, memory_order_acquire) != 2U ||
        atomic_load_explicit(&retried.called, memory_order_acquire) != 1U) {
        ok = 0;
    }
    dcc_rest_request_destroy(retried_request);
    request_callback_state_deinit(&retried);

    request_callback_state_t terminal_rate_limit;
    request_callback_state_init(&terminal_rate_limit);
    dcc_rest_request_t *terminal_request = NULL;
    terminal_rate_limit.request_slot = &terminal_request;
    if (dcc_client_on_error(
            client,
            request_error_observer,
            &terminal_rate_limit
        ) != DCC_OK) {
        ok = 0;
    }
    request_intercept_reset(intercept, REQUEST_INTERCEPT_RATE_LIMIT_ALWAYS, 0U);
    if (request_submit_test(
            client,
            "/retry-terminal",
            &terminal_rate_limit,
            &terminal_request
        ) != DCC_OK) {
        ok = 0;
    }
    const dcc_rest_result_t *terminal_result = NULL;
    if (terminal_request == NULL ||
        dcc_rest_request_wait(terminal_request, 3000U, &terminal_result) != DCC_OK ||
        terminal_result == NULL ||
        dcc_rest_result_status(terminal_result) != DCC_ERR_DISCORD ||
        terminal_result->http_status != 429U || terminal_result->discord_code != 20028 ||
        terminal_result->retry_after_ms == 0U ||
        atomic_load_explicit(&intercept->calls, memory_order_acquire) < 2U ||
        atomic_load_explicit(&terminal_rate_limit.called, memory_order_acquire) != 2U ||
        atomic_load_explicit(&terminal_rate_limit.stage, memory_order_acquire) != 2U) {
        ok = 0;
    }
    dcc_rest_request_destroy(terminal_request);
    request_callback_state_deinit(&terminal_rate_limit);
    request_callback_state_deinit(&observed);
    (void)dcc_client_on_error(client, NULL, NULL);
    return ok ? 0 : 1;
}

static int request_wait_for_queue_state(
    dcc_client_t *client,
    size_t pending,
    size_t active,
    size_t active_routes,
    uint32_t timeout_ms,
    dcc_rest_async_status_t *out
) {
    uint64_t started_ms = request_now_ms();
    do {
        dcc_rest_async_status_t snapshot = {.size = sizeof(snapshot)};
        if (dcc_rest_async_status(client, &snapshot) != DCC_OK) {
            return 0;
        }
        if (snapshot.pending == pending && snapshot.active == active &&
            snapshot.active_routes == active_routes) {
            if (out != NULL) {
                *out = snapshot;
            }
            return 1;
        }
        request_sleep_ms(1U);
    } while (request_now_ms() - started_ms < timeout_ms);
    if (out != NULL) {
        out->size = sizeof(*out);
        (void)dcc_rest_async_status(client, out);
    }
    return 0;
}

static int request_same_route_cancel_tracking_contract(void) {
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 2U,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK ||
        dcc_client_start(client) != DCC_OK) {
        dcc_client_destroy(client);
        return 1;
    }

    request_intercept_state_t intercept;
    memset(&intercept, 0, sizeof(intercept));
    atomic_init(&intercept.mode, REQUEST_INTERCEPT_SUCCESS);
    atomic_init(&intercept.delay_ms, 0U);
    atomic_init(&intercept.calls, 0U);
    atomic_init(&intercept.entered, 0U);
    atomic_init(&intercept.observed_delay_ms, 0U);
    atomic_init(&intercept.cooperative_hold, 1U);
    atomic_init(&intercept.cooperative_release, 0U);
    dcc_rest_set_interceptor(client, request_intercept, &intercept);

    request_runner_t runner = {.client = client, .status = DCC_ERR_STATE};
    pthread_t runner_thread;
    if (pthread_create(&runner_thread, NULL, request_runner_main, &runner) != 0) {
        atomic_store_explicit(&intercept.cooperative_release, 1U, memory_order_release);
        dcc_client_destroy(client);
        return 1;
    }

    int ok = 1;
    dcc_rest_request_t *original = NULL;
    dcc_rest_request_t *canceled = NULL;
    dcc_rest_request_t *blocked = NULL;
    const dcc_rest_result_t *original_result = NULL;
    const dcc_rest_result_t *canceled_result = NULL;
    const dcc_rest_result_t *blocked_result = NULL;
    dcc_rest_async_status_t before = {.size = sizeof(before)};
    dcc_rest_async_status_t during = {.size = sizeof(during)};
    dcc_rest_async_status_t after = {.size = sizeof(after)};

    if (request_submit_test(client, "/same-route", NULL, &original) != DCC_OK ||
        !request_wait_for_entered(&intercept) ||
        request_submit_test(client, "/same-route", NULL, &canceled) != DCC_OK ||
        dcc_rest_async_status(client, &before) != DCC_OK ||
        before.active != 1U || before.pending != 1U || before.active_routes != 1U ||
        before.pending_blocked_by_active_route != 1U ||
        dcc_rest_request_cancel(canceled) != DCC_OK ||
        dcc_rest_request_wait(canceled, 2000U, &canceled_result) != DCC_OK ||
        canceled_result == NULL ||
        dcc_rest_result_status(canceled_result) != DCC_ERR_CANCELED ||
        !request_wait_for_queue_state(client, 0U, 1U, 1U, 1000U, &during) ||
        request_submit_test(client, "/same-route", NULL, &blocked) != DCC_OK ||
        dcc_rest_async_status(client, &after) != DCC_OK ||
        after.active != 1U || after.pending != 1U || after.active_routes != 1U ||
        after.pending_blocked_by_active_route != 1U ||
        atomic_load_explicit(&intercept.calls, memory_order_acquire) != 1U ||
        dcc_rest_request_cancel(blocked) != DCC_OK ||
        dcc_rest_request_wait(blocked, 2000U, &blocked_result) != DCC_OK ||
        blocked_result == NULL ||
        dcc_rest_result_status(blocked_result) != DCC_ERR_CANCELED ||
        !request_wait_for_queue_state(client, 0U, 1U, 1U, 1000U, NULL)) {
        ok = 0;
    }

    atomic_store_explicit(&intercept.cooperative_release, 1U, memory_order_release);
    if (original == NULL ||
        dcc_rest_request_wait(original, 2000U, &original_result) != DCC_OK ||
        original_result == NULL || dcc_rest_result_status(original_result) != DCC_OK ||
        dcc_rest_async_wait(client, 2000U) != DCC_OK ||
        !request_wait_for_queue_state(client, 0U, 0U, 0U, 1000U, &after) ||
        atomic_load_explicit(&intercept.calls, memory_order_acquire) != 1U) {
        ok = 0;
    }
    if (!ok) {
        fprintf(
            stderr,
            "same-route cancel detail before=%zu/%zu/%zu/%zu "
            "during=%zu/%zu/%zu after=%zu/%zu/%zu/%zu calls=%u\n",
            before.pending,
            before.active,
            before.active_routes,
            before.pending_blocked_by_active_route,
            during.pending,
            during.active,
            during.active_routes,
            after.pending,
            after.active,
            after.active_routes,
            after.pending_blocked_by_active_route,
            atomic_load_explicit(&intercept.calls, memory_order_acquire)
        );
    }

    dcc_rest_request_destroy(blocked);
    dcc_rest_request_destroy(canceled);
    dcc_rest_request_destroy(original);
    (void)dcc_client_stop(client);
    (void)pthread_join(runner_thread, NULL);
    dcc_client_destroy(client);
    return ok && runner.status == DCC_OK ? 0 : 1;
}

static int request_client_teardown_contract(void) {
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 1U,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK ||
        dcc_client_start(client) != DCC_OK) {
        dcc_client_destroy(client);
        return 1;
    }
    request_intercept_state_t intercept;
    memset(&intercept, 0, sizeof(intercept));
    atomic_init(&intercept.mode, REQUEST_INTERCEPT_SUCCESS);
    atomic_init(&intercept.delay_ms, 0U);
    atomic_init(&intercept.calls, 0U);
    atomic_init(&intercept.entered, 0U);
    atomic_init(&intercept.observed_delay_ms, 0U);
    atomic_init(&intercept.cooperative_hold, 1U);
    atomic_init(&intercept.cooperative_release, 0U);
    dcc_rest_set_interceptor(client, request_intercept, &intercept);

    request_runner_t runner = {.client = client, .status = DCC_ERR_STATE};
    pthread_t runner_thread;
    if (pthread_create(&runner_thread, NULL, request_runner_main, &runner) != 0) {
        dcc_client_destroy(client);
        return 1;
    }

    request_callback_state_t callback;
    request_callback_state_init(&callback);
    dcc_rest_request_t *request = NULL;
    callback.request_slot = &request;
    request_callback_state_t pending_callback;
    request_callback_state_init(&pending_callback);
    dcc_rest_request_t *pending_request = NULL;
    pending_callback.request_slot = &pending_request;
    int ok = request_submit_test(client, "/client-teardown", &callback, &request) == DCC_OK &&
        request_wait_for_entered(&intercept) &&
        request_submit_test(
            client,
            "/client-teardown-pending",
            &pending_callback,
            &pending_request
        ) == DCC_OK;
    dcc_status_t stop_status = ok ? dcc_client_stop(client) : DCC_ERR_STATE;
    unsigned callback_before_teardown = atomic_load_explicit(
        &pending_callback.called,
        memory_order_acquire
    );

    request_start_gate_t start_gate;
    atomic_init(&start_gate.ready, 0U);
    atomic_init(&start_gate.release, 0U);
    request_control_t cancel_control = {
        .request = pending_request,
        .action = REQUEST_CONTROL_CANCEL,
        .start_gate = &start_gate,
        .status = DCC_ERR_STATE,
    };
    atomic_init(&cancel_control.returned, 0U);
    request_client_destroyer_t destroyer = {
        .client = client,
        .start_gate = &start_gate,
    };
    atomic_init(&destroyer.returned, 0U);
    pthread_t cancel_thread;
    pthread_t destroy_thread;
    int cancel_started = pthread_create(
        &cancel_thread,
        NULL,
        request_control_main,
        &cancel_control
    ) == 0;
    int destroy_started = pthread_create(
        &destroy_thread,
        NULL,
        request_client_destroyer_main,
        &destroyer
    ) == 0;
    int race_ready = cancel_started && destroy_started &&
        request_wait_for_atomic_at_least(&start_gate.ready, 2U, 1000U);
    atomic_store_explicit(&start_gate.release, 1U, memory_order_release);
    int pending_delivered = race_ready && request_wait_for_atomic(
        &pending_callback.stage,
        1000U
    );
    int cancel_returned = race_ready && request_wait_for_atomic(
        &cancel_control.returned,
        1000U
    );
    int destroy_waited_for_active = race_ready &&
        atomic_load_explicit(&destroyer.returned, memory_order_acquire) == 0U;
    atomic_store_explicit(&intercept.cooperative_release, 1U, memory_order_release);
    if (cancel_started) {
        (void)pthread_join(cancel_thread, NULL);
    }
    if (destroy_started) {
        (void)pthread_join(destroy_thread, NULL);
    } else {
        dcc_client_destroy(client);
    }
    (void)pthread_join(runner_thread, NULL);

    const dcc_rest_result_t *result = NULL;
    const dcc_rest_result_t *pending_result = NULL;
    if (!ok || stop_status != DCC_OK || cancel_control.status != DCC_OK ||
        callback_before_teardown != 0U || !race_ready || !pending_delivered ||
        !cancel_returned ||
        !destroy_waited_for_active ||
        atomic_load_explicit(&destroyer.returned, memory_order_acquire) != 1U ||
        request == NULL || runner.status != DCC_OK ||
        dcc_rest_request_wait(request, 1000U, &result) != DCC_OK ||
        result == NULL || dcc_rest_result_status(result) != DCC_ERR_CANCELED ||
        pending_request == NULL ||
        dcc_rest_request_wait(pending_request, 1000U, &pending_result) != DCC_OK ||
        pending_result == NULL ||
        dcc_rest_result_status(pending_result) != DCC_ERR_CANCELED ||
        dcc_rest_request_cancel(request) != DCC_OK ||
        dcc_rest_request_cancel(pending_request) != DCC_OK ||
        atomic_load_explicit(&callback.called, memory_order_acquire) != 1U ||
        atomic_load_explicit(&pending_callback.called, memory_order_acquire) != 1U) {
        ok = 0;
    }
    dcc_rest_request_destroy(request);
    dcc_rest_request_destroy(pending_request);
    if (callback.clone == NULL ||
        dcc_rest_result_status(callback.clone) != DCC_ERR_CANCELED ||
        pending_callback.clone == NULL ||
        dcc_rest_result_status(pending_callback.clone) != DCC_ERR_CANCELED) {
        ok = 0;
    }
    request_callback_state_deinit(&callback);
    request_callback_state_deinit(&pending_callback);
    return ok ? 0 : 1;
}

int main(void) {
    if (request_initializer_and_null_contract() != 0) {
        fprintf(stderr, "request initializer/null contract failed\n");
        return 1;
    }

    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 1U,
    };
    dcc_client_t *client = NULL;
    dcc_status_t status = dcc_client_create(&options, &client);
    if (status != DCC_OK || request_validation_contract(client) != 0) {
        fprintf(stderr, "request validation contract failed\n");
        dcc_client_destroy(client);
        return 1;
    }
    status = dcc_client_start(client);
    if (status != DCC_OK) {
        fprintf(stderr, "request client start failed: %s\n", dcc_status_string(status));
        dcc_client_destroy(client);
        return 1;
    }

    request_intercept_state_t intercept;
    memset(&intercept, 0, sizeof(intercept));
    atomic_init(&intercept.mode, REQUEST_INTERCEPT_SUCCESS);
    atomic_init(&intercept.delay_ms, 0U);
    atomic_init(&intercept.calls, 0U);
    atomic_init(&intercept.entered, 0U);
    atomic_init(&intercept.observed_delay_ms, 0U);
    atomic_init(&intercept.cooperative_hold, 0U);
    atomic_init(&intercept.cooperative_release, 1U);
    dcc_rest_set_interceptor(client, request_intercept, &intercept);

    request_runner_t runner = {.client = client, .status = DCC_ERR_STATE};
    pthread_t runner_thread;
    if (pthread_create(&runner_thread, NULL, request_runner_main, &runner) != 0) {
        fprintf(stderr, "request runtime thread creation failed\n");
        dcc_client_destroy(client);
        return 1;
    }

    int failed = request_delayed_copy_wait_contract(client, &intercept);
    if (!failed) {
        failed = request_nonblocking_cancel_destroy_contract(client, &intercept);
    }
    if (!failed) {
        failed = request_cancel_contract(client, &intercept);
    }
    if (!failed) {
        failed = request_destroy_and_auto_release_contract(client, &intercept);
    }
    if (!failed) {
        failed = request_error_admission_and_retry_contract(client, &intercept);
    }
    (void)dcc_client_stop(client);
    (void)pthread_join(runner_thread, NULL);
    dcc_client_destroy(client);
    if (failed || runner.status != DCC_OK) {
        fprintf(stderr, "request queue/handle contract failed\n");
        return 1;
    }
    if (request_same_route_cancel_tracking_contract() != 0) {
        fprintf(stderr, "request same-route cancel tracking contract failed\n");
        return 1;
    }
    if (request_client_teardown_contract() != 0) {
        fprintf(stderr, "request client teardown contract failed\n");
        return 1;
    }
    return 0;
}

#endif
