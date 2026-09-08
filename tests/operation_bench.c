/* Offline operation completion, not network/Discord latency. Setup and flow
 * identity registration are outside the timed stage; queueing, scheduling,
 * synchronous interceptor response, callbacks and drain are inside it. */
#include <dcc/dcc.h>
#include <dcc/interaction_flow.h>
#include <dcc/rest/core/async.h>
#include <dcc/rest/request.h>
#include "internal/rest/dcc_rest_intercept_internal.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum { ITERATIONS = 500 };
static atomic_uint callbacks;
static atomic_uint errors;

static dcc_status_t respond(dcc_client_t *client, const char *method,
    const char *path, const void *body, size_t body_len, const char *content_type,
    dcc_rest_cb callback, void *user_data, void *intercept_data) {
    (void)method; (void)path; (void)body; (void)body_len;
    (void)content_type; (void)intercept_data;
    dcc_rest_response_t response = {.size = sizeof(response), .status = 204U, .error = DCC_OK};
    callback(client, &response, user_data);
    return DCC_OK;
}
static void completed(dcc_client_t *client, const dcc_rest_result_t *result, void *data) {
    (void)client; (void)data;
    if (!result || dcc_rest_result_status(result) != DCC_OK) atomic_fetch_add(&errors, 1U);
    atomic_fetch_add(&callbacks, 1U);
}
static void *runtime(void *data) {
    return (void *)(uintptr_t)dcc_client_wait(data);
}
static double seconds(struct timespec a, struct timespec b) {
    return (double)(b.tv_sec - a.tv_sec) + (double)(b.tv_nsec - a.tv_nsec) / 1e9;
}
static int compare(const void *a, const void *b) {
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}
static int measure(dcc_client_t *client, dcc_interaction_flow_t **flows, int interaction) {
    double latencies[ITERATIONS], total = 0;
    dcc_rest_call_options_t call_options = DCC_REST_CALL_OPTIONS_INIT;
    call_options.callback = completed;
    dcc_rest_request_desc_t desc = DCC_REST_REQUEST_DESC_INIT;
    desc.method = DCC_REST_POST;
    desc.path = "/benchmark";
    static const char json[] = "{\"content\":\"offline reply\"}";
    desc.body = json;
    desc.body_len = sizeof(json) - 1U;
    desc.content_type = "application/json";
    desc.options = &call_options;
    dcc_message_builder_t message = DCC_MESSAGE_BUILDER_INIT;
    message.present = DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
    message.content = "offline reply";
    atomic_store(&callbacks, 0U);
    atomic_store(&errors, 0U);
    clock_t cpu_start = clock();
    for (size_t i = 0; i < ITERATIONS; ++i) {
        struct timespec start, end;
        if (clock_gettime(CLOCK_MONOTONIC, &start)) return 1;
        dcc_status_t status = interaction
            ? dcc_flow_reply(flows[i], &message, completed, NULL)
            : dcc_rest_submit(client, &desc, NULL);
        if (status != DCC_OK || dcc_rest_async_wait(client, 5000U) != DCC_OK) return 1;
        if (clock_gettime(CLOCK_MONOTONIC, &end)) return 1;
        latencies[i] = seconds(start, end);
        total += latencies[i];
    }
    clock_t cpu_end = clock();
    if (atomic_load(&callbacks) != ITERATIONS || atomic_load(&errors)) return 1;
    qsort(latencies, ITERATIONS, sizeof(latencies[0]), compare);
    printf("{\"fixture\":\"%s\",\"stage\":\"admit_complete_drain\",\"iterations\":%u,"
           "\"cpu_seconds\":%.9f,\"elapsed_seconds\":%.9f,\"allocations\":null,"
           "\"p95_elapsed_ns\":%.3f,\"p99_elapsed_ns\":%.3f}\n",
           interaction ? "interaction_reply_interceptor_v1" : "rest_submit_interceptor_v1",
           ITERATIONS, (double)(cpu_end - cpu_start) / CLOCKS_PER_SEC, total,
           latencies[474] * 1e9, latencies[494] * 1e9);
    return 0;
}
int main(void) {
    atomic_init(&callbacks, 0U); atomic_init(&errors, 0U);
    dcc_client_options_t options = {.size = sizeof(options), .token = "",
        .rest_concurrency = 1U, .interaction_max_live_queues = 512U,
        .interaction_max_tombstones = 1024U};
    dcc_client_t *client = NULL;
    if (dcc_client_create(&options, &client) != DCC_OK || dcc_client_start(client) != DCC_OK) return 1;
    dcc_rest_set_interceptor(client, respond, NULL);
    pthread_t thread;
    if (pthread_create(&thread, NULL, runtime, client)) return 1;
    dcc_interaction_flow_t *flows[ITERATIONS] = {0};
    int failed = 0;
    for (size_t i = 0; i < ITERATIONS; ++i) {
        dcc_interaction_t interaction = {.id = i + 1U, .application_id = 701U,
                                        .token = "offline-token"};
        if (dcc_flow_create(client, &interaction, &flows[i]) != DCC_OK) { failed = 1; break; }
    }
    if (!failed) failed = measure(client, flows, 0) || measure(client, flows, 1);
    for (size_t i = 0; i < ITERATIONS; ++i) dcc_flow_destroy(flows[i]);
    (void)dcc_client_stop(client);
    void *runtime_status = NULL;
    (void)pthread_join(thread, &runtime_status);
    dcc_client_destroy(client);
    return failed || runtime_status != (void *)(uintptr_t)DCC_OK;
}
