#include "support/http_smoke_server.h"

#include <dcc/dcc.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct flow_seen {
    int called;
    uint16_t status;
    dcc_status_t error;
} flow_seen_t;

static void flow_rest_cb(dcc_client_t *client, const dcc_rest_response_t *response, void *user_data) {
    (void)client;
    flow_seen_t *seen = (flow_seen_t *)user_data;
    if (seen != NULL && response != NULL) {
        seen->called = 1;
        seen->status = response->status;
        seen->error = response->error;
    }
}

static int expect_request(
    http_server_t *server,
    pthread_t thread,
    dcc_status_t status,
    const flow_seen_t *seen,
    const char *method,
    const char *path,
    const char *body
) {
    (void)pthread_join(thread, NULL);
    close(server->fd);
    if (status != DCC_OK ||
        seen == NULL ||
        !seen->called ||
        seen->status != 200 ||
        strcmp(server->method, method) != 0 ||
        strcmp(server->path, path) != 0 ||
        strcmp(server->body, body) != 0) {
        fprintf(
            stderr,
            "unexpected flow request: st=%s called=%d status=%u method=%s path=%s body=%s\n",
            dcc_status_string(status),
            seen != NULL ? seen->called : 0,
            seen != NULL ? seen->status : 0,
            server->method,
            server->path,
            server->body
        );
        return 0;
    }
    return 1;
}

static int start_flow_server(http_server_t *server, pthread_t *thread) {
    if (start_server(server, thread) != 0) {
        fprintf(stderr, "failed to start flow server\n");
        return 0;
    }
    set_api_base_for_server(server);
    return 1;
}

static dcc_status_t set_message(dcc_message_builder_t *message, const char *content) {
    dcc_message_builder_init(message);
    return dcc_message_builder_set_content(message, content);
}

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t status = dcc_client_create(&opts, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "failed to create flow client: %s\n", dcc_status_string(status));
        return 1;
    }

    dcc_interaction_t interaction = {
        .id = 555U,
        .application_id = 666U,
        .token = "tok",
    };
    dcc_interaction_flow_t flow;
    dcc_flow_init(&flow, client, &interaction);

    dcc_message_builder_t message;
    http_server_t server;
    pthread_t thread;
    flow_seen_t seen;

    if (set_message(&message, "hello") != DCC_OK ||
        !start_flow_server(&server, &thread)) {
        dcc_client_destroy(client);
        return 1;
    }
    memset(&seen, 0, sizeof(seen));
    status = dcc_flow_reply(&flow, &message, flow_rest_cb, &seen);
    if (!expect_request(
            &server,
            thread,
            status,
            &seen,
            "POST",
            "/interactions/555/tok/callback",
            "{\"type\":4,\"data\":{\"content\":\"hello\"}}"
        ) ||
        dcc_flow_state(&flow) != DCC_INTERACTION_FLOW_REPLIED) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (set_message(&message, "again") != DCC_OK ||
        !start_flow_server(&server, &thread)) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    memset(&seen, 0, sizeof(seen));
    status = dcc_flow_reply(&flow, &message, flow_rest_cb, &seen);
    if (!expect_request(
            &server,
            thread,
            status,
            &seen,
            "POST",
            "/webhooks/666/tok",
            "{\"content\":\"again\"}"
        ) ||
        dcc_flow_state(&flow) != DCC_INTERACTION_FLOW_FOLLOWED_UP) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    dcc_flow_init(&flow, client, &interaction);
    if (dcc_flow_set_started_at(&flow, 1000U) != DCC_OK ||
        dcc_flow_auto_defer_ephemeral(&flow, 1500U) != DCC_OK ||
        dcc_flow_maybe_auto_defer(&flow, 2000U, flow_rest_cb, &seen) != DCC_OK ||
        dcc_flow_state(&flow) != DCC_INTERACTION_FLOW_READY ||
        !start_flow_server(&server, &thread)) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    memset(&seen, 0, sizeof(seen));
    status = dcc_flow_maybe_auto_defer(&flow, 2600U, flow_rest_cb, &seen);
    if (!expect_request(
            &server,
            thread,
            status,
            &seen,
            "POST",
            "/interactions/555/tok/callback",
            "{\"type\":5,\"data\":{\"flags\":64}}"
        ) ||
        dcc_flow_state(&flow) != DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    if (set_message(&message, "done") != DCC_OK ||
        !start_flow_server(&server, &thread)) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }
    memset(&seen, 0, sizeof(seen));
    status = dcc_flow_reply(&flow, &message, flow_rest_cb, &seen);
    if (!expect_request(
            &server,
            thread,
            status,
            &seen,
            "PATCH",
            "/webhooks/666/tok/messages/@original",
            "{\"content\":\"done\"}"
        ) ||
        dcc_flow_state(&flow) != DCC_INTERACTION_FLOW_ORIGINAL_EDITED ||
        strcmp(dcc_flow_state_string(dcc_flow_state(&flow)), "original_edited") != 0) {
        (void)unsetenv("DCC_DISCORD_API_BASE");
        dcc_client_destroy(client);
        return 1;
    }

    (void)unsetenv("DCC_DISCORD_API_BASE");
    dcc_client_destroy(client);
    return 0;
}

#endif
