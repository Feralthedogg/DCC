#include <dcc/dcc.h>
#include <dcc/interaction_flow.h>

#include "internal/rest/dcc_rest_intercept_internal.h"

#if defined(_WIN32)
int main(void) { return 0; }
#else

#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct queue_red_state {
    atomic_uint request_count;
    atomic_uint callback_count;
    atomic_bool release_first;
} queue_red_state_t;

static void queue_red_callback(
    dcc_client_t *client, const dcc_rest_response_t *response,
    void *user_data) {
    (void)client;
    queue_red_state_t *state = (queue_red_state_t *)user_data;
    if (state != NULL && response != NULL && response->status == 204U)
        atomic_fetch_add_explicit(
            &state->callback_count, 1U, memory_order_acq_rel);
}

static dcc_status_t queue_red_interceptor(
    dcc_client_t *client, const char *method, const char *path,
    const void *body, size_t body_len, const char *content_type,
    dcc_rest_cb callback, void *user_data, void *intercept_user_data) {
    (void)method;
    (void)path;
    (void)body;
    (void)body_len;
    (void)content_type;
    queue_red_state_t *state = (queue_red_state_t *)intercept_user_data;
    atomic_fetch_add_explicit(&state->request_count, 1U, memory_order_acq_rel);
    while (!atomic_load_explicit(&state->release_first, memory_order_acquire))
        usleep(100U);
    if (callback != NULL) {
        dcc_rest_response_t response = {
            .size = sizeof(response), .status = 204U, .error = DCC_OK};
        callback(client, &response, user_data);
    }
    return DCC_OK;
}

typedef struct queue_red_runner {
    dcc_client_t *client;
} queue_red_runner_t;

static void *queue_red_runtime(void *user_data) {
    queue_red_runner_t *runner = (queue_red_runner_t *)user_data;
    (void)dcc_client_wait(runner->client);
    return NULL;
}

int main(void) {
    dcc_client_options_t options = {
        .size = sizeof(options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 2U,
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK ||
        dcc_client_start(client) != DCC_OK)
        return 1;
    queue_red_runner_t runner = {.client = client};
    pthread_t runtime;
    if (pthread_create(&runtime, NULL, queue_red_runtime, &runner) != 0)
        return 1;

    queue_red_state_t state;
    memset(&state, 0, sizeof(state));
    atomic_init(&state.request_count, 0U);
    atomic_init(&state.callback_count, 0U);
    atomic_init(&state.release_first, false);
    dcc_rest_set_interceptor(client, queue_red_interceptor, &state);

    dcc_interaction_t interaction = {
        .id = 700U, .application_id = 701U, .token = "ordered-token"};
    dcc_interaction_flow_t flow;
    dcc_flow_init(&flow, client, &interaction);
    dcc_message_builder_t first = DCC_MESSAGE_BUILDER_INIT;
    first.present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
    first.content = "first";
    dcc_message_builder_t second = DCC_MESSAGE_BUILDER_INIT;
    second.present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
    second.content = "second";

    dcc_status_t first_status =
        dcc_flow_reply(&flow, &first, queue_red_callback, &state);
    dcc_status_t second_status =
        dcc_flow_reply(&flow, &second, queue_red_callback, &state);
    usleep(50000U);
    unsigned before_release = atomic_load_explicit(
        &state.request_count, memory_order_acquire);
    atomic_store_explicit(&state.release_first, true, memory_order_release);
    dcc_status_t drain = dcc_rest_async_wait(client, 5000U);
    unsigned final_requests = atomic_load_explicit(
        &state.request_count, memory_order_acquire);
    unsigned callbacks = atomic_load_explicit(
        &state.callback_count, memory_order_acquire);

    dcc_rest_set_interceptor(client, NULL, NULL);
    (void)dcc_client_stop(client);
    (void)pthread_join(runtime, NULL);
    dcc_client_destroy(client);
    if (first_status != DCC_OK || second_status != DCC_OK ||
        before_release != 1U || final_requests != 2U || callbacks != 2U ||
        drain != DCC_OK) {
        fprintf(stderr,
                "interaction FIFO RED: first=%d second=%d before=%u final=%u "
                "callbacks=%u drain=%d\n",
                first_status, second_status, before_release, final_requests,
                callbacks, drain);
        return 1;
    }
    return 0;
}

#endif
