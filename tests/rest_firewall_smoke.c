#include <dcc/dcc.h>

#if !defined(_WIN32)
#include "support/http_smoke_server.h"
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>

static int check_standalone_firewall(void) {
    dcc_rest_firewall_options_t opts;
    dcc_rest_firewall_options_init(&opts);
    opts.invalid_request_soft_limit = 1U;
    opts.invalid_request_hard_limit = 2U;
    opts.soft_limit_delay_ms = 7U;

    dcc_rest_firewall_t *firewall = NULL;
    if (dcc_rest_firewall_create(&opts, &firewall) != DCC_OK) {
        fprintf(stderr, "firewall init failed\n");
        return 0;
    }

    dcc_rest_firewall_request_t request = {
        .size = sizeof(request),
        .method = "POST",
        .path = "/channels/1/messages",
        .now_ms = 1000U,
        .critical = 0U,
    };
    dcc_rest_firewall_result_t result;
    if (dcc_rest_firewall_check(firewall, &request, &result) != DCC_OK ||
        result.decision != DCC_REST_FIREWALL_ALLOW ||
        dcc_rest_firewall_record_response(firewall, 401U, 1001U) != DCC_OK) {
        fprintf(stderr, "firewall first request failed\n");
        dcc_rest_firewall_destroy(firewall);
        return 0;
    }

    request.now_ms = 1002U;
    if (dcc_rest_firewall_check(firewall, &request, &result) != DCC_OK ||
        result.decision != DCC_REST_FIREWALL_DELAY ||
        result.reason != DCC_REST_FIREWALL_REASON_SOFT_INVALID_REQUEST_LIMIT ||
        result.delay_ms != 7U ||
        dcc_rest_firewall_record_response(firewall, 429U, 1003U) != DCC_OK) {
        fprintf(stderr, "firewall soft delay failed\n");
        dcc_rest_firewall_destroy(firewall);
        return 0;
    }

    request.now_ms = 1004U;
    if (dcc_rest_firewall_check(firewall, &request, &result) != DCC_OK ||
        result.decision != DCC_REST_FIREWALL_REJECT ||
        result.reason != DCC_REST_FIREWALL_REASON_HARD_INVALID_REQUEST_LIMIT) {
        fprintf(stderr, "firewall hard reject failed\n");
        dcc_rest_firewall_destroy(firewall);
        return 0;
    }

    request.path = "/interactions/1/tok/callback";
    request.now_ms = 1005U;
    if (dcc_rest_firewall_check(firewall, &request, &result) != DCC_OK ||
        result.decision != DCC_REST_FIREWALL_DELAY ||
        !result.critical) {
        fprintf(stderr, "firewall critical path handling failed\n");
        dcc_rest_firewall_destroy(firewall);
        return 0;
    }

    dcc_rest_firewall_destroy(firewall);
    return 1;
}

#if !defined(_WIN32)
static int check_attached_firewall(void) {
    dcc_client_options_t client_opts = {
        .size = sizeof(client_opts),
        .token = "firewall-token",
    };
    dcc_client_t *client = NULL;
    if (dcc_client_create(&client_opts, &client) != DCC_OK) {
        fprintf(stderr, "client create failed\n");
        return 0;
    }

    dcc_rest_firewall_options_t opts;
    dcc_rest_firewall_options_init(&opts);
    opts.invalid_request_soft_limit = 1U;
    opts.invalid_request_hard_limit = 1U;
    opts.soft_limit_delay_ms = 0U;
    if (dcc_rest_firewall_attach(client, &opts) != DCC_OK) {
        fprintf(stderr, "firewall attach failed\n");
        dcc_client_destroy(client);
        return 0;
    }

    http_server_t server;
    pthread_t thread;
    if (start_server_mode(&server, &thread, 6) != 0) {
        fprintf(stderr, "failed to start firewall server\n");
        dcc_client_destroy(client);
        return 0;
    }
    set_api_base_for_server(&server);

    uint16_t first_status = 0;
    dcc_rest_request_desc_t description = DCC_REST_REQUEST_DESC_INIT;
    description.path = "/channels/123";
    dcc_rest_request_t *request = NULL;
    const dcc_rest_result_t *request_result = NULL;
    dcc_status_t start_status = dcc_client_start(client);
    dcc_status_t submit_status = start_status == DCC_OK
        ? dcc_rest_submit(client, &description, &request) : start_status;
    dcc_status_t wait_status = submit_status == DCC_OK
        ? dcc_rest_request_wait(request, 5000U, &request_result) : submit_status;
    if (start_status != DCC_OK || submit_status != DCC_OK ||
        wait_status != DCC_OK ||
        request_result == NULL ||
        (first_status = request_result->http_status) != 401U ||
        first_status != 401U) {
        fprintf(stderr, "first attached request failed: %u start=%d submit=%d wait=%d result=%p\n",
                (unsigned)first_status, start_status, submit_status, wait_status,
                (void *)request_result);
        dcc_rest_request_destroy(request);
        dcc_client_destroy(client);
        close(server.fd);
        (void)pthread_join(thread, NULL);
        return 0;
    }
    dcc_rest_request_destroy(request);
    (void)pthread_join(thread, NULL);
    close(server.fd);

    dcc_rest_firewall_snapshot_t snapshot = {
        .size = sizeof(snapshot),
    };
    if (dcc_rest_firewall_snapshot(client, &snapshot) != DCC_OK ||
        !snapshot.attached ||
        snapshot.invalid_request_count != 1U ||
        !snapshot.hard_limit_exceeded) {
        fprintf(stderr, "attached snapshot failed\n");
        dcc_client_destroy(client);
        return 0;
    }

    description.path = "/channels/456";
    description.options = NULL;
    dcc_status_t rejected = dcc_rest_submit(client, &description, NULL);
    if (rejected != DCC_ERR_STATE ||
        atomic_load_explicit(&server.requests_seen, memory_order_acquire) != 1U) {
        fprintf(stderr, "attached preflight reject failed: %s requests=%u\n",
                dcc_status_string(rejected),
                atomic_load_explicit(&server.requests_seen, memory_order_acquire));
        dcc_client_destroy(client);
        return 0;
    }

    char json[1024];
    size_t json_len = 0;
    if (dcc_rest_firewall_snapshot_json(&snapshot, json, sizeof(json), &json_len) != DCC_OK ||
        json_len == 0 ||
        strstr(json, "\"hard_limit_exceeded\":true") == NULL ||
        strstr(json, "\"invalid_request_count\":1") == NULL) {
        fprintf(stderr, "snapshot json failed: %s\n", json);
        dcc_client_destroy(client);
        return 0;
    }

    dcc_rest_firewall_detach(client);
    (void)dcc_client_stop(client);
    (void)dcc_client_wait(client);
    dcc_client_destroy(client);
    return 1;
}
#endif

int main(void) {
    if (!check_standalone_firewall()) {
        return 1;
    }
    if (!dcc_rest_firewall_status_is_invalid(401U) ||
        !dcc_rest_firewall_status_is_invalid(403U) ||
        !dcc_rest_firewall_status_is_invalid(429U) ||
        dcc_rest_firewall_status_is_invalid(400U) ||
        strcmp(dcc_rest_firewall_decision_string(DCC_REST_FIREWALL_DELAY), "delay") != 0 ||
        strcmp(
            dcc_rest_firewall_reason_string(DCC_REST_FIREWALL_REASON_HARD_INVALID_REQUEST_LIMIT),
            "hard_invalid_request_limit"
        ) != 0) {
        fprintf(stderr, "firewall helpers failed\n");
        return 1;
    }
    return 0;
}
