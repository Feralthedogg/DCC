# Ownership And Async

C does not encode ownership in the type system. DCC therefore uses naming and
callback rules consistently. Treat an undocumented pointer as borrowed until
the declaring header says otherwise.

## Ownership At A Glance

| Value source | Ownership | Valid until | Retain it by |
| --- | --- | --- | --- |
| Event accessor such as `dcc_event_message()` | Borrowed | Event callback returns | `dcc_message_clone()` |
| `dcc_event_data()` | Borrowed parser view | Callback returns or another frame is parsed | `dcc_gateway_event_data_clone()` |
| Cache getter | Borrowed client view | Object replacement, removal, eviction, cache clear, or client destruction | Matching `dcc_*_clone()` |
| REST callback `result->body` | Borrowed | REST callback returns | Copy the bytes or call `dcc_rest_result_clone()` |
| Retained request result | Request-owned | `dcc_rest_request_destroy()` | Call `dcc_rest_result_clone()` |
| `dcc_rest_result_clone()` result | Caller-owned | `dcc_rest_result_free()` | Already owned |
| `dcc_*_clone()` result | Caller-owned | Matching `dcc_*_free()` | Already owned |
| `dcc_*_create()` result | Caller-owned runtime object | Matching `dcc_*_destroy()` | Already owned |
| Sugar variadic builder literals | Block-scoped | End of the current C block | Use explicit arrays or `*_ARRAY` forms |

## Retaining An Event Object

```c
static dcc_message_t *latest_message;

static void on_event(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)user_data;

    const dcc_message_t *borrowed = dcc_event_message(event);
    if (borrowed == NULL) {
        return;
    }

    dcc_message_t *copy = dcc_message_clone(borrowed);
    if (copy == NULL) {
        return;
    }

    dcc_message_free(latest_message);
    latest_message = copy;
}
```

Never store `borrowed` itself. It points into event/parser-owned memory.

## REST Admission, Completion, And Success

`dcc_rest_submit()` has three distinct outcomes to reason about:

1. **Admission:** a non-`DCC_OK` return rejects the operation. A supplied
   output handle is cleared, no work is queued, and no terminal callback runs.
   A `DCC_OK` return means DCC accepted the operation and will deliver exactly
   one terminal completion.
2. **Completion:** `dcc_rest_request_wait()` returning `DCC_OK` means callback
   and observer delivery finished. A timeout leaves the request handle and its
   eventual result owned by the caller; retain the handle and wait again.
3. **HTTP success:** inspect the terminal result with
   `dcc_rest_result_status()` or `dcc_rest_result_ok()`. Completion can still
   contain a transport failure or a non-2xx Discord response.

All path, content-type, body, file metadata, file bytes, and typed endpoint
inputs are copied or serialized before a successful submission returns. The
callback function and `user_data` remain borrowed until the terminal callback
returns. Callback threads have no fixed affinity: normal work usually completes
on a runtime worker, while bulk cancellation or teardown can deliver on the
thread performing that operation.

Pass a non-NULL `out_request` to retain a caller-owned handle. The result view
returned by `dcc_rest_request_wait()` remains valid until that handle is
destroyed. `dcc_rest_request_destroy()` requests cancellation and releases the
caller reference without waiting; it never suppresses an installed callback.
Therefore callback user data must remain alive until completion is observed or
client shutdown has drained the operation, even if the handle was destroyed.
Use `dcc_rest_result_clone()` when result bytes must outlive the callback or
request handle.

## Complete REST Ownership Example

This example is compiled against an installed DCC package during documentation
checks. It is not run by the test suite and contacts Discord only when a user
runs it with `DCC_TOKEN` set.

<!-- DCC_DOC_SNIPPET_BEGIN(rest-ownership) -->
```c
#include <dcc/client.h>
#include <dcc/rest/request.h>
#include <dcc/rest/result.h>

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef struct rest_example_state {
    atomic_uint completions;
    atomic_int callback_status;
} rest_example_state_t;

typedef struct runtime_thread_state {
    dcc_client_t *client;
    dcc_status_t status;
} runtime_thread_state_t;

static void on_result(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
) {
    (void)client;
    rest_example_state_t *state = (rest_example_state_t *)user_data;
    atomic_store_explicit(
        &state->callback_status,
        dcc_rest_result_status(result),
        memory_order_relaxed
    );
    atomic_fetch_add_explicit(&state->completions, 1U, memory_order_release);
}

static void run_client(runtime_thread_state_t *state) {
    state->status = dcc_client_wait(state->client);
}

#if defined(_WIN32)
typedef HANDLE runtime_thread_t;

static DWORD WINAPI run_client_thread(LPVOID user_data) {
    runtime_thread_state_t *state = (runtime_thread_state_t *)user_data;
    run_client(state);
    return 0U;
}

static int start_runtime_thread(
    runtime_thread_t *thread,
    runtime_thread_state_t *state
) {
    *thread = CreateThread(NULL, 0U, run_client_thread, state, 0U, NULL);
    return *thread != NULL;
}

static void join_runtime_thread(runtime_thread_t thread) {
    (void)WaitForSingleObject(thread, INFINITE);
    (void)CloseHandle(thread);
}
#else
typedef pthread_t runtime_thread_t;

static void *run_client_thread(void *user_data) {
    runtime_thread_state_t *state = (runtime_thread_state_t *)user_data;
    run_client(state);
    return NULL;
}

static int start_runtime_thread(
    runtime_thread_t *thread,
    runtime_thread_state_t *state
) {
    return pthread_create(thread, NULL, run_client_thread, state) == 0;
}

static void join_runtime_thread(runtime_thread_t thread) {
    (void)pthread_join(thread, NULL);
}
#endif

int main(void) {
    const char *token = getenv("DCC_TOKEN");
    if (token == NULL || token[0] == '\0') {
        fprintf(stderr, "set DCC_TOKEN to run this example\n");
        return 2;
    }

    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = token,
        .rest_concurrency = 1U,
    };
    dcc_client_t *client = NULL;
    dcc_status_t status = dcc_client_create(&client_options, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "create: %s\n", dcc_status_string(status));
        return 1;
    }
    status = dcc_client_start(client);
    if (status != DCC_OK) {
        fprintf(stderr, "start: %s\n", dcc_status_string(status));
        dcc_client_destroy(client);
        return 1;
    }

    runtime_thread_state_t runtime = {client, DCC_ERR_RUNTIME};
    runtime_thread_t runtime_thread;
    if (!start_runtime_thread(&runtime_thread, &runtime)) {
        (void)dcc_client_stop(client);
        dcc_client_destroy(client);
        return 1;
    }

    rest_example_state_t callback_state;
    atomic_init(&callback_state.completions, 0U);
    atomic_init(&callback_state.callback_status, DCC_ERR_STATE);

    char path[] = "/users/@me";
    dcc_rest_call_options_t call_options = DCC_REST_CALL_OPTIONS_INIT;
    call_options.callback = on_result;
    call_options.user_data = &callback_state;

    dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
    description.method = DCC_REST_GET;
    description.path = path;
    description.options = &call_options;

    dcc_rest_request_t *request = NULL;
    status = dcc_rest_submit(client, &description, &request);
    if (status != DCC_OK) {
        /* Admission was rejected: request is NULL and no callback will run. */
        fprintf(stderr, "admission: %s\n", dcc_status_string(status));
        (void)dcc_client_stop(client);
        join_runtime_thread(runtime_thread);
        dcc_client_destroy(client);
        return 1;
    }

    /* DCC copied path and option inputs before successful admission returned. */
    memset(path, 0, sizeof(path));

    const dcc_rest_result_t *result = NULL;
    status = dcc_rest_request_wait(request, 5000U, &result);
    if (status == DCC_ERR_TIMEOUT) {
        /* A timeout does not release ownership; retain and wait again. */
        status = dcc_rest_request_wait(request, 0U, &result);
    }

    int exit_code = 0;
    if (status != DCC_OK || result == NULL) {
        fprintf(stderr, "wait: %s\n", dcc_status_string(status));
        exit_code = 1;
    } else if (!dcc_rest_result_ok(result)) {
        fprintf(
            stderr,
            "completion: transport=%s http=%u\n",
            dcc_status_string(result->transport_status),
            (unsigned)result->http_status
        );
        exit_code = 1;
    } else {
        if (result->body_len != 0U) {
            (void)fwrite(result->body, 1U, result->body_len, stdout);
        }
        (void)fputc('\n', stdout);
    }

    /* This releases the retained handle; completed result views expire here. */
    dcc_rest_request_destroy(request);
    (void)dcc_client_stop(client);
    join_runtime_thread(runtime_thread);

    if (atomic_load_explicit(&callback_state.completions, memory_order_acquire) != 1U) {
        fprintf(stderr, "accepted request did not produce exactly one callback\n");
        exit_code = 1;
    }
    if (runtime.status != DCC_OK) {
        fprintf(stderr, "runtime: %s\n", dcc_status_string(runtime.status));
        exit_code = 1;
    }
    dcc_client_destroy(client);
    return exit_code;
}
```
<!-- DCC_DOC_SNIPPET_END(rest-ownership) -->

The owner starts and drives the client runtime on a dedicated thread. It
releases the retained handle after completion; during shutdown it requests
stop, joins the runtime driver, and only then destroys the client and
callback-owned state. Production applications may use a different owner
thread, but must preserve the same ordering: close admission, drain terminal
delivery, release shared callback state, then release the client.

## Callback Discipline

- Keep Gateway and supervisor callbacks short.
- Do not block a callback on long CPU work, a subprocess, or unbounded I/O.
- Copy or clone inputs before handing work to another task or thread.
- Ensure callback `user_data` outlives every registered callback.
- Stop the app/client and drain outstanding work before freeing shared state.

## Builder Lifetimes

Sugar macros often use compound literals. Values passed immediately to a DCC
function are convenient and safe. Values stored beyond the current block need
explicit backing arrays or the corresponding `*_ARRAY` macro. Dynamic or
cross-module builders should use the explicit builder API.
