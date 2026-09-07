/* DCC_DOC_SNIPPET_BEGIN(rest-ownership) */
#include <dcc/client.h>
#include <dcc/interaction_flow.h>
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

/* Call from an owner thread while another thread runs dcc_client_wait().
 * The caller keeps callback_state alive through completion (including errors).
 * Context handlers can use dcc_ctx_defer_ex/dcc_ctx_edit_original_ex with the
 * same options/output pattern, then hand the handle to their owner thread. */
dcc_status_t dcc_example_queued_reply(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    rest_example_state_t *callback_state
) {
    dcc_interaction_flow_t *flow = NULL;
    dcc_status_t status = dcc_flow_create(client, interaction, &flow);
    if (status != DCC_OK) return status;

    /* Default defer is non-ephemeral; NULL output auto-releases its handle. */
    status = dcc_flow_defer_ex(flow, NULL, NULL);
    dcc_rest_request_t *edit = NULL;
    if (status == DCC_OK) {
        char text[] = "Work finished";
        dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
        status = dcc_message_builder_set_content(&message, text);
        dcc_rest_call_options_t options = DCC_REST_CALL_OPTIONS_INIT;
        options.priority = DCC_REST_PRIORITY_HIGH;
        options.flags = DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY |
            DCC_REST_CALL_FLAG_SENSITIVE_RESULT_BODY;
        options.callback = on_result;
        options.user_data = callback_state;
        if (status == DCC_OK)
            status = dcc_flow_edit_original_ex(flow, &message, &options, &edit);
        /* message, text and options may now leave scope: admission copied them.
         * edit already exists even while the defer is still in flight. */
    }
    dcc_flow_destroy(flow); /* Accepted work and the result survive this. */
    if (status != DCC_OK) return status; /* Rejected edit: NULL handle, no callback. */

    const dcc_rest_result_t *result = NULL;
    status = dcc_rest_request_wait(edit, 0U, &result);
    if (status == DCC_OK) status = dcc_rest_result_status(result);
    dcc_rest_request_destroy(edit);
    return status;
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
/* DCC_DOC_SNIPPET_END(rest-ownership) */
