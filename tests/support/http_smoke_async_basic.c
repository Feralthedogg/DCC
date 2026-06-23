#include "http_smoke_support.h"

#if !defined(_WIN32)
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_public_rest_async_smoke(void) {
    http_server_t first;
    http_server_t second;
    pthread_t first_thread;
    pthread_t second_thread;
    if (start_server(&first, &first_thread) != 0) {
        fprintf(stderr, "failed to start first async rest server: %s\n", strerror(errno));
        return 1;
    }
    if (start_server(&second, &second_thread) != 0) {
        fprintf(stderr, "failed to start second async rest server: %s\n", strerror(errno));
        close(first.fd);
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 1,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        close(first.fd);
        close(second.fd);
        return 1;
    }

    st = dcc_client_start(client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_start failed: %s\n", dcc_status_string(st));
        close(first.fd);
        close(second.fd);
        dcc_client_destroy(client);
        return 1;
    }

    char first_url[128];
    char second_url[128];
    snprintf(first_url, sizeof(first_url), "http://127.0.0.1:%u/async-first", (unsigned)first.port);
    snprintf(second_url, sizeof(second_url), "http://127.0.0.1:%u/async-second", (unsigned)second.port);

    rest_async_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    st = dcc_rest_request_async(client, "GET", first_url, NULL, async_rest_cb, &seen);
    if (st == DCC_OK) {
        st = dcc_rest_request_async(client, "GET", second_url, NULL, async_rest_cb, &seen);
    }
    if (st == DCC_OK) {
        st = dcc_client_wait(client);
    } else {
        (void)dcc_client_stop(client);
    }

    (void)pthread_join(first_thread, NULL);
    (void)pthread_join(second_thread, NULL);
    close(first.fd);
    close(second.fd);
    dcc_client_destroy(client);

    if (st != DCC_OK) {
        fprintf(stderr, "async rest run failed: %s\n", dcc_status_string(st));
        return 1;
    }
    if (seen.called != 2 ||
        seen.statuses[0] != 200 ||
        seen.statuses[1] != 200 ||
        seen.errors[0] != DCC_OK ||
        seen.errors[1] != DCC_OK ||
        strcmp(seen.bodies[0], "hello") != 0 ||
        strcmp(seen.bodies[1], "hello") != 0 ||
        strcmp(first.method, "GET") != 0 ||
        strcmp(first.path, "/async-first") != 0 ||
        strcmp(second.method, "GET") != 0 ||
        strcmp(second.path, "/async-second") != 0) {
        fprintf(stderr,
                "unexpected async rest result: called=%d status0=%u status1=%u err0=%s err1=%s first=%s %s second=%s %s\n",
                seen.called,
                seen.statuses[0],
                seen.statuses[1],
                dcc_status_string(seen.errors[0]),
                dcc_status_string(seen.errors[1]),
                first.method,
                first.path,
                second.method,
                second.path);
        return 1;
    }

    return 0;
}

int run_public_rest_future_smoke(void) {
    http_server_t server;
    pthread_t server_thread;
    if (start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "failed to start future rest server: %s\n", strerror(errno));
        return 1;
    }
    server.delay_ms = 80;

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 1,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "future client create failed: %s\n", dcc_status_string(st));
        close(server.fd);
        return 1;
    }

    dcc_rest_future_t *future = NULL;
    if (dcc_rest_request_future(NULL, "GET", "http://127.0.0.1/", NULL, &future) != DCC_ERR_INVALID_ARG ||
        dcc_rest_request_future(client, "GET", "http://127.0.0.1/", NULL, &future) != DCC_ERR_STATE ||
        dcc_rest_future_wait(NULL, 1, &(dcc_rest_response_t){ .size = sizeof(dcc_rest_response_t) }) !=
            DCC_ERR_INVALID_ARG ||
        dcc_rest_future_destroy(NULL) != DCC_ERR_INVALID_ARG ||
        dcc_rest_future_completed(NULL) != 0) {
        fprintf(stderr, "future invalid handling failed\n");
        close(server.fd);
        dcc_client_destroy(client);
        return 1;
    }

    st = dcc_client_start(client);
    if (st != DCC_OK) {
        fprintf(stderr, "future client start failed: %s\n", dcc_status_string(st));
        close(server.fd);
        dcc_client_destroy(client);
        return 1;
    }

    client_wait_thread_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_STATE,
    };
    pthread_t wait_thread;
    int wait_thread_started = 0;
    if (pthread_create(&wait_thread, NULL, client_wait_thread_main, &wait_state) != 0) {
        fprintf(stderr, "failed to start future runtime thread: %s\n", strerror(errno));
        st = DCC_ERR_RUNTIME;
    } else {
        wait_thread_started = 1;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u/future", (unsigned)server.port);
    if (st == DCC_OK) {
        st = dcc_rest_request_method_future_priority(
            client,
            DCC_REST_GET,
            url,
            NULL,
            DCC_REST_PRIORITY_HIGH,
            &future
        );
    }

    dcc_rest_response_t response = {
        .size = sizeof(response),
    };
    dcc_status_t future_timeout_status = DCC_OK;
    if (st == DCC_OK) {
        dcc_rest_response_t timeout_response = {
            .size = sizeof(timeout_response),
        };
        future_timeout_status = dcc_rest_future_wait(future, 10, &timeout_response);
        if (future_timeout_status != DCC_ERR_TIMEOUT) {
            st = DCC_ERR_STATE;
        }
    }
    if (st == DCC_OK) {
        st = dcc_rest_future_wait(future, 2000, &response);
    }

    char response_body[16];
    response_body[0] = '\0';
    if (response.body != NULL && response.body_len < sizeof(response_body)) {
        memcpy(response_body, response.body, response.body_len);
        response_body[response.body_len] = '\0';
    }
    int response_ok = response.status == 200 &&
        response.error == DCC_OK &&
        response.body != NULL &&
        response.body_len == 5 &&
        strcmp(response_body, "hello") == 0;

    dcc_status_t destroy_status = DCC_ERR_STATE;
    if (future != NULL && dcc_rest_future_completed(future)) {
        destroy_status = dcc_rest_future_destroy(future);
        future = NULL;
    }

    (void)dcc_client_stop(client);
    if (wait_thread_started) {
        (void)pthread_join(wait_thread, NULL);
    }
    (void)pthread_join(server_thread, NULL);
    close(server.fd);
    dcc_client_destroy(client);

    if (st != DCC_OK ||
        !response_ok ||
        future_timeout_status != DCC_ERR_TIMEOUT ||
        destroy_status != DCC_OK ||
        wait_state.status != DCC_OK ||
        strcmp(server.method, "GET") != 0 ||
        strcmp(server.path, "/future") != 0) {
        fprintf(stderr,
                "future rest failed: st=%s timeout=%s status=%u error=%s body=%s destroy=%s runtime=%s method=%s path=%s\n",
                dcc_status_string(st),
                dcc_status_string(future_timeout_status),
                response.status,
                dcc_status_string(response.error),
                response_body,
                dcc_status_string(destroy_status),
                dcc_status_string(wait_state.status),
                server.method,
                server.path);
        return 1;
    }

    return 0;
}

typedef struct app_send_thread_seen {
    int called;
    dcc_snowflake_t message_id;
    dcc_snowflake_t thread_id;
    dcc_status_t status;
    uint16_t response_status;
} app_send_thread_seen_t;

static void app_send_thread_cb(
    dcc_app_t *app,
    const dcc_rest_response_t *response,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    dcc_status_t status,
    void *user_data
) {
    app_send_thread_seen_t *seen = (app_send_thread_seen_t *)user_data;
    if (seen != NULL) {
        seen->called++;
        seen->message_id = message_id;
        seen->thread_id = thread_id;
        seen->status = status;
        seen->response_status = response != NULL ? response->status : 0U;
    }
    (void)dcc_app_stop(app);
}

int run_public_rest_app_send_thread_smoke(void) {
    http_server_t server;
    pthread_t server_thread;
    if (start_message_thread_server(&server, &server_thread) != 0) {
        fprintf(stderr, "failed to start app send-thread server: %s\n", strerror(errno));
        return 1;
    }
    set_api_base_for_server(&server);

    dcc_app_options_t opts;
    dcc_app_options_init(&opts);
    opts.client.token = "";
    opts.client.intents = DCC_INTENT_GUILDS;
    opts.client.rest_concurrency = 1;

    dcc_app_t *app = NULL;
    dcc_status_t st = dcc_app_create(&opts, &app);
    if (st == DCC_OK) {
        st = dcc_app_start(app);
    }

    app_send_thread_seen_t seen;
    memset(&seen, 0, sizeof(seen));
    if (st == DCC_OK) {
        st = dcc_app_send_text_with_thread(
            app,
            222ULL,
            "posted",
            "support thread",
            app_send_thread_cb,
            &seen
        );
    }
    if (st == DCC_OK) {
        st = dcc_app_wait(app);
    } else if (app != NULL) {
        (void)dcc_app_stop(app);
        close(server.fd);
        server.fd = -1;
    }

    (void)pthread_join(server_thread, NULL);
    if (server.fd >= 0) {
        close(server.fd);
    }
    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_app_destroy(app);

    if (st != DCC_OK ||
        seen.called != 1 ||
        seen.status != DCC_OK ||
        seen.response_status != 200U ||
        seen.message_id != 500ULL ||
        seen.thread_id != 900ULL ||
        server.request_count != 2U ||
        strcmp(server.methods[0], "POST") != 0 ||
        strcmp(server.paths[0], "/channels/222/messages") != 0 ||
        strstr(server.bodies[0], "\"content\":\"posted\"") == NULL ||
        strcmp(server.methods[1], "POST") != 0 ||
        strcmp(server.paths[1], "/channels/222/messages/500/threads") != 0 ||
        strstr(server.bodies[1], "\"name\":\"support thread\"") == NULL) {
        fprintf(stderr,
                "app send-thread failed: st=%s called=%d status=%s response=%u ids=%llu/%llu "
                "requests=%u first=%s %s body=%s second=%s %s body=%s\n",
                dcc_status_string(st),
                seen.called,
                dcc_status_string(seen.status),
                seen.response_status,
                (unsigned long long)seen.message_id,
                (unsigned long long)seen.thread_id,
                server.request_count,
                server.methods[0],
                server.paths[0],
                server.bodies[0],
                server.methods[1],
                server.paths[1],
                server.bodies[1]);
        return 1;
    }

    return 0;
}

int run_public_rest_async_priority_smoke(void) {
    http_server_t low;
    http_server_t normal;
    http_server_t high;
    pthread_t low_thread;
    pthread_t normal_thread;
    pthread_t high_thread;
    if (start_server(&low, &low_thread) != 0) {
        fprintf(stderr, "failed to start low priority rest server: %s\n", strerror(errno));
        return 1;
    }
    low.delay_ms = 80;
    if (start_server(&normal, &normal_thread) != 0) {
        fprintf(stderr, "failed to start normal priority rest server: %s\n", strerror(errno));
        close(low.fd);
        return 1;
    }
    if (start_server(&high, &high_thread) != 0) {
        fprintf(stderr, "failed to start high priority rest server: %s\n", strerror(errno));
        close(low.fd);
        close(normal.fd);
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
        .rest_concurrency = 1,
    };
    dcc_status_t st = dcc_client_create(&opts, &client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(st));
        close(low.fd);
        close(normal.fd);
        close(high.fd);
        return 1;
    }

    st = dcc_client_start(client);
    if (st != DCC_OK) {
        fprintf(stderr, "dcc_client_start failed: %s\n", dcc_status_string(st));
        close(low.fd);
        close(normal.fd);
        close(high.fd);
        dcc_client_destroy(client);
        return 1;
    }

    char low_url[128];
    char normal_url[128];
    char high_url[128];
    snprintf(low_url, sizeof(low_url), "http://127.0.0.1:%u/async-low", (unsigned)low.port);
    snprintf(normal_url, sizeof(normal_url), "http://127.0.0.1:%u/async-normal", (unsigned)normal.port);
    snprintf(high_url, sizeof(high_url), "http://127.0.0.1:%u/async-high", (unsigned)high.port);

    rest_async_order_state_t state;
    memset(&state, 0, sizeof(state));
    state.expected = 3;
    rest_async_order_item_t low_item = { .state = &state, .label = 1 };
    rest_async_order_item_t normal_item = { .state = &state, .label = 2 };
    rest_async_order_item_t high_item = { .state = &state, .label = 3 };

    st = dcc_rest_request_async_priority(
        client,
        "GET",
        low_url,
        NULL,
        DCC_REST_PRIORITY_LOW,
        async_order_cb,
        &low_item
    );
    if (st == DCC_OK) {
        st = dcc_rest_request_async_priority(
            client,
            "GET",
            normal_url,
            NULL,
            DCC_REST_PRIORITY_NORMAL,
            async_order_cb,
            &normal_item
        );
    }
    if (st == DCC_OK) {
        st = dcc_rest_request_async_priority(
            client,
            "GET",
            high_url,
            NULL,
            DCC_REST_PRIORITY_HIGH,
            async_order_cb,
            &high_item
        );
    }
    if (st == DCC_OK) {
        st = dcc_client_wait(client);
    } else {
        (void)dcc_client_stop(client);
    }

    (void)pthread_join(low_thread, NULL);
    (void)pthread_join(normal_thread, NULL);
    (void)pthread_join(high_thread, NULL);
    close(low.fd);
    close(normal.fd);
    close(high.fd);
    dcc_client_destroy(client);

    if (st != DCC_OK ||
        state.called != 3 ||
        state.labels[0] != 1 ||
        state.labels[1] != 3 ||
        state.labels[2] != 2 ||
        state.statuses[0] != 200 ||
        state.statuses[1] != 200 ||
        state.statuses[2] != 200 ||
        state.errors[0] != DCC_OK ||
        state.errors[1] != DCC_OK ||
        state.errors[2] != DCC_OK ||
        strcmp(low.path, "/async-low") != 0 ||
        strcmp(high.path, "/async-high") != 0 ||
        strcmp(normal.path, "/async-normal") != 0) {
        fprintf(stderr,
                "unexpected async priority result: st=%s called=%d labels=%d,%d,%d statuses=%u,%u,%u paths=%s,%s,%s\n",
                dcc_status_string(st),
                state.called,
                state.labels[0],
                state.labels[1],
                state.labels[2],
                state.statuses[0],
                state.statuses[1],
                state.statuses[2],
                low.path,
                high.path,
                normal.path);
        return 1;
    }
    return 0;
}


#endif
