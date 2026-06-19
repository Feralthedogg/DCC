#include "http_smoke_support.h"

#include "internal/http/dcc_http.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_internal_http_smoke(void) {
    http_server_t server;
    pthread_t thread;
    if (start_server(&server, &thread) != 0) {
        fprintf(stderr, "failed to start local http server: %s\n", strerror(errno));
        return 1;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u/ping", (unsigned)server.port);

    dcc_http_request_t request = {
        .method = "GET",
        .url = url,
        .timeout_ms = 5000,
    };
    dcc_http_response_t response;
    dcc_status_t st = dcc_http_perform(&request, &response);

    (void)pthread_join(thread, NULL);
    close(server.fd);

    if (st != DCC_OK) {
        fprintf(stderr, "dcc_http_perform failed: %s\n", dcc_status_string(st));
        return 1;
    }

    int ok = response.status == 200 &&
             response.body_len == 5 &&
             memcmp(response.body, "hello", 5) == 0;
    if (!ok) {
        fprintf(stderr, "unexpected response: status=%u body_len=%llu\n",
                response.status,
                (unsigned long long)response.body_len);
        dcc_http_response_deinit(&response);
        return 1;
    }

    dcc_http_response_deinit(&response);
    return 0;
}

int run_internal_http_chunked_smoke(void) {
    http_server_t server;
    pthread_t thread;
    if (start_server_mode(&server, &thread, 1) != 0) {
        fprintf(stderr, "failed to start local chunked http server: %s\n", strerror(errno));
        return 1;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u/chunked", (unsigned)server.port);

    dcc_http_request_t request = {
        .method = "GET",
        .url = url,
        .timeout_ms = 5000,
    };
    dcc_http_response_t response;
    dcc_status_t st = dcc_http_perform(&request, &response);

    (void)pthread_join(thread, NULL);
    close(server.fd);

    if (st != DCC_OK) {
        fprintf(stderr, "dcc_http_perform chunked failed: %s\n", dcc_status_string(st));
        return 1;
    }

    int ok = response.status == 200 &&
             response.body_len == 5 &&
             memcmp(response.body, "hello", 5) == 0;
    dcc_http_response_deinit(&response);
    if (!ok) {
        fprintf(stderr, "unexpected chunked response\n");
        return 1;
    }
    return 0;
}

void rest_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    (void)client;
    rest_seen_t *seen = (rest_seen_t *)user_data;
    seen->called = 1;
    seen->status = response->status;
    seen->error = response->error;
    seen->body_len = response->body_len;
    if (response->body != NULL && response->body_len < sizeof(seen->body)) {
        memcpy(seen->body, response->body, response->body_len);
        seen->body[response->body_len] = '\0';
    }
}

void rate_limited_cb(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    rest_seen_t *seen = (rest_seen_t *)user_data;
    const dcc_rate_limited_event_t *limited = dcc_event_rate_limited(event);
    if (seen == NULL || limited == NULL) {
        return;
    }
    seen->rate_limited_called = 1;
    seen->retry_after = limited->retry_after;
    seen->global = limited->global;
    if (limited->method != NULL) {
        snprintf(seen->method, sizeof(seen->method), "%s", limited->method);
    }
    if (limited->path != NULL) {
        snprintf(seen->path, sizeof(seen->path), "%s", limited->path);
    }
}

void async_rest_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    rest_async_seen_t *seen = (rest_async_seen_t *)user_data;
    if (seen == NULL || response == NULL) {
        (void)dcc_client_stop(client);
        return;
    }

    int index = seen->called;
    if (index >= 0 && index < 2) {
        seen->statuses[index] = response->status;
        seen->errors[index] = response->error;
        if (response->body != NULL && response->body_len < sizeof(seen->bodies[index])) {
            memcpy(seen->bodies[index], response->body, response->body_len);
            seen->bodies[index][response->body_len] = '\0';
        }
    }
    seen->called++;
    if (seen->called >= 2) {
        (void)dcc_client_stop(client);
    }
}

void async_order_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    rest_async_order_item_t *item = (rest_async_order_item_t *)user_data;
    if (item == NULL || item->state == NULL || response == NULL) {
        (void)dcc_client_stop(client);
        return;
    }

    rest_async_order_state_t *state = item->state;
    int index = state->called;
    if (index >= 0 && index < 4) {
        state->labels[index] = item->label;
        state->statuses[index] = response->status;
        state->errors[index] = response->error;
    }
    state->called++;
    if (state->expected > 0 && state->called >= state->expected) {
        (void)dcc_client_stop(client);
    }
}

void async_record_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    (void)client;
    rest_async_seen_t *seen = (rest_async_seen_t *)user_data;
    if (seen == NULL || response == NULL) {
        return;
    }

    int index = seen->called;
    if (index >= 0 && index < 2) {
        seen->statuses[index] = response->status;
        seen->errors[index] = response->error;
        if (response->body != NULL && response->body_len < sizeof(seen->bodies[index])) {
            memcpy(seen->bodies[index], response->body, response->body_len);
            seen->bodies[index][response->body_len] = '\0';
        }
    }
    seen->called++;
}

void *client_wait_thread_main(void *arg) {
    client_wait_thread_state_t *state = (client_wait_thread_state_t *)arg;
    if (state == NULL || state->client == NULL) {
        return NULL;
    }
    state->status = dcc_client_wait(state->client);
    return NULL;
}

void *event_wait_thread_main(void *arg) {
    event_wait_thread_state_t *state = (event_wait_thread_state_t *)arg;
    if (state == NULL || state->client == NULL) {
        return NULL;
    }
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_event(state->client, state->type, state->timeout_ms, &state->snapshot);
    return NULL;
}

int run_public_event_wait_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "event wait client create failed: %s\n", dcc_status_string(st));
        return 1;
    }

    dcc_event_snapshot_t invalid_snapshot = {
        .size = sizeof(invalid_snapshot),
    };
    if (dcc_client_wait_for_event(NULL, DCC_EVENT_READY, 1, &invalid_snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event(client, DCC_EVENT_READY, 1, &invalid_snapshot) != DCC_ERR_STATE) {
        fprintf(stderr, "event wait invalid handling failed\n");
        dcc_client_destroy(client);
        return 1;
    }

    st = dcc_client_start(client);
    if (st != DCC_OK) {
        fprintf(stderr, "event wait client start failed: %s\n", dcc_status_string(st));
        dcc_client_destroy(client);
        return 1;
    }

    event_wait_thread_state_t ready_wait;
    memset(&ready_wait, 0, sizeof(ready_wait));
    ready_wait.client = client;
    ready_wait.type = DCC_EVENT_READY;
    ready_wait.timeout_ms = 2000;

    pthread_t ready_thread;
    if (pthread_create(&ready_thread, NULL, event_wait_thread_main, &ready_wait) != 0) {
        fprintf(stderr, "event wait ready thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        dcc_client_destroy(client);
        return 1;
    }

    usleep(10000);

    client_wait_thread_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_STATE,
    };
    pthread_t wait_thread;
    if (pthread_create(&wait_thread, NULL, client_wait_thread_main, &wait_state) != 0) {
        fprintf(stderr, "event wait runtime thread failed: %s\n", strerror(errno));
        (void)dcc_client_stop(client);
        (void)pthread_join(ready_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    (void)pthread_join(ready_thread, NULL);
    if (ready_wait.status != DCC_OK ||
        ready_wait.snapshot.type != DCC_EVENT_READY ||
        strcmp(ready_wait.snapshot.name, "READY") != 0 ||
        strcmp(ready_wait.snapshot.raw_name, "READY") != 0 ||
        ready_wait.snapshot.shard_id != 0) {
        fprintf(stderr,
                "event wait ready failed: status=%s type=%d name=%s raw=%s shard=%u\n",
                dcc_status_string(ready_wait.status),
                (int)ready_wait.snapshot.type,
                ready_wait.snapshot.name,
                ready_wait.snapshot.raw_name,
                ready_wait.snapshot.shard_id);
        (void)dcc_client_stop(client);
        (void)pthread_join(wait_thread, NULL);
        dcc_client_destroy(client);
        return 1;
    }

    dcc_event_snapshot_t timeout_snapshot = {
        .size = sizeof(timeout_snapshot),
    };
    st = dcc_client_wait_for_event(client, DCC_EVENT_MESSAGE_CREATE, 25, &timeout_snapshot);
    (void)dcc_client_stop(client);
    (void)pthread_join(wait_thread, NULL);
    dcc_client_destroy(client);

    if (st != DCC_ERR_TIMEOUT || wait_state.status != DCC_OK) {
        fprintf(stderr,
                "event wait timeout failed: wait=%s runtime=%s\n",
                dcc_status_string(st),
                dcc_status_string(wait_state.status));
        return 1;
    }
    return 0;
}

#endif
