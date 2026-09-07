#include "internal/json/dcc_json.h"
#include "internal/events/dcc_events_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/app/dcc_app_interaction_queue_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static volatile uint64_t sink;
static dcc_client_t *client;
static dcc_json_gateway_payload_t *payload;
static char body[65536];
static const char message[] = "{\"op\":0,\"s\":43,\"t\":\"MESSAGE_CREATE\",\"d\":{\"id\":\"11\",\"channel_id\":\"22\",\"guild_id\":\"33\",\"content\":\"offline benchmark message\",\"author\":{\"id\":\"44\",\"username\":\"bench-user\"}}}";

static double elapsed(struct timespec a, struct timespec b) {
    return (double)(b.tv_sec - a.tv_sec) + (double)(b.tv_nsec - a.tv_nsec) / 1e9;
}
static int now(struct timespec *value) {
#if defined(_WIN32)
    return timespec_get(value, TIME_UTC) == TIME_UTC ? 0 : 1;
#else
    return clock_gettime(CLOCK_MONOTONIC, value);
#endif
}
static void listener(dcc_client_t *c, const dcc_event_t *event, void *data) {
    (void)c; (void)event; (void)data; ++sink;
}
static int parse(size_t stage) {
    if (stage != 1U && dcc_json_stage1_validate(message, sizeof(message) - 1U) != DCC_OK) return 1;
    if (stage != 0U) {
        if (dcc_json_parse_gateway_payload(message, sizeof(message) - 1U, payload) != DCC_OK) return 1;
        sink += payload->message.id;
    } else sink += sizeof(message) - 1U;
    return 0;
}
static int dispatch(size_t ignored) {
    (void)ignored;
    dcc_event_t event = {.type = DCC_EVENT_MESSAGE_CREATE};
    return dcc_event_bus_dispatch(&client->events, client, &event) != DCC_OK;
}
static int request(size_t bytes) {
    dcc_rest_async_request_t *r = dcc_rest_async_request_new(
        client, "POST", "benchmark", "/webhooks/1/offline-token", body, bytes,
        "application/octet-stream", "offline-audit", DCC_REST_AUTH_BOT,
        "offline-token", DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY, 1U,
        DCC_REST_PRIORITY_NORMAL, NULL, NULL, NULL);
    if (!r) return 1;
    sink += r->body_len + (unsigned char)r->wire_path[0];
    dcc_rest_async_request_free(r);
    return 0;
}
static int admission(size_t charge) {
    dcc_status_t status = DCC_ERR_STATE;
    dcc_app_interaction_queue_t *q = dcc_app_interaction_queue_create(client, charge, &status);
    if (status != DCC_OK || q == NULL) return 1;
    ++sink;
    dcc_app_interaction_queue_release_owner(q);
    return 0;
}
static int run(const char *fixture, const char *stage, size_t parameter,
               size_t iterations, int (*fn)(size_t)) {
    for (size_t i = 0; i < 128U; ++i) if (fn(parameter)) return 1;
    struct timespec start, end;
    if (now(&start)) return 1;
    clock_t cpu_start = clock();
    for (size_t i = 0; i < iterations; ++i) if (fn(parameter)) return 1;
    clock_t cpu_end = clock();
    if (now(&end)) return 1;
    printf("{\"fixture\":\"%s\",\"stage\":\"%s\",\"iterations\":%zu,"
           "\"cpu_seconds\":%.9f,\"elapsed_seconds\":%.9f,\"allocations\":null}\n",
           fixture, stage, iterations, (double)(cpu_end - cpu_start) / CLOCKS_PER_SEC,
           elapsed(start, end));
    return 0;
}
int main(void) {
    size_t iterations = 20000U;
    const char *setting = getenv("DCC_BENCH_ITERS");
    if (setting) {
        char *end = NULL;
        unsigned long long value = strtoull(setting, &end, 10);
        if (end == setting || *end || setting[0] == '-' || !value || value > 100000000ULL) return 2;
        iterations = (size_t)value;
    }
    payload = malloc(sizeof(*payload));
    dcc_client_options_t options = {.size = sizeof(options), .token = ""};
    if (!payload || dcc_client_create(&options, &client) != DCC_OK) return 1;
    memset(body, 'x', sizeof(body)); body[1] = '\0';
    int failed = run("message_create_v1", "stage1", 0U, iterations, parse) ||
        run("message_create_v1", "gateway", 1U, iterations, parse) ||
        run("message_create_v1", "stage1_gateway", 2U, iterations, parse);
    const size_t counts[] = {0U, 1U, 8U, 9U, 32U};
    for (size_t i = 0; i < 5U && !failed; ++i) {
        dcc_listener_id_t ids[32];
        for (size_t j = 0; j < counts[i]; ++j)
            if (dcc_event_bus_add(&client->events, DCC_EVENT_MESSAGE_CREATE, listener,
                                  (void *)(uintptr_t)(j + 1U), &ids[j]) != DCC_OK) return 1;
        char name[32];
        (void)snprintf(name, sizeof(name), "listeners_%zu", counts[i]);
        failed = run(name, "dispatch", 0U, iterations * 10U, dispatch);
        for (size_t j = 0; j < counts[i]; ++j)
            if (dcc_event_bus_remove(&client->events, DCC_EVENT_MESSAGE_CREATE, ids[j]) != DCC_OK) return 1;
    }
    failed = failed || run("body_0", "construct_free", 0U, iterations * 10U, request) ||
        run("body_32", "construct_free", 32U, iterations * 10U, request) ||
        run("body_65536", "construct_free", sizeof(body), iterations, request) ||
        run("interaction_identity_13", "queue_admit_release",
            dcc_app_interaction_queue_identity_charge(13U), iterations * 10U, admission);
    fprintf(stderr, "sink=%llu compiler=%s payload_bytes=%zu\n",
            (unsigned long long)sink,
#ifdef __VERSION__
            __VERSION__,
#else
            "unknown",
#endif
            sizeof(*payload));
    dcc_client_destroy(client);
    free(payload);
    return failed;
}
