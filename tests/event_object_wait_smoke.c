#include <dcc/dcc.h>

#include "support/event_object_wait_smoke_support.h"

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int event_object_wait_close_server(object_wait_server_t *server, pthread_t server_thread) {
    close(server->fd);
    (void)pthread_join(server_thread, NULL);
    return 1;
}

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);

    object_wait_server_t server;
    memset(&server, 0, sizeof(server));
    atomic_init(&server.accepts, 0U);
    atomic_init(&server.identifies, 0U);
    atomic_init(&server.bad, 0U);
    if (start_listener(&server.fd, &server.port) != 0) {
        fprintf(stderr, "failed to start object wait gateway server\n");
        return 1;
    }

    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, server_thread_main, &server) != 0) {
        close(server.fd);
        return 1;
    }

    char gateway_url[160];
    snprintf(gateway_url, sizeof(gateway_url), "ws://127.0.0.1:%u/?v=10&encoding=json", (unsigned)server.port);

    dcc_client_t *client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "mock-token",
        .intents = DCC_INTENT_GUILDS,
        .gateway_url = gateway_url,
    };
    dcc_status_t status = dcc_client_create(&opts, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_client_create failed: %s\n", dcc_status_string(status));
        return event_object_wait_close_server(&server, server_thread);
    }

    if (event_object_wait_smoke_check_invalid_handling(client) != 0) {
        dcc_client_destroy(client);
        return event_object_wait_close_server(&server, server_thread);
    }

    status = dcc_client_start(client);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_client_start failed: %s\n", dcc_status_string(status));
        dcc_client_destroy(client);
        return event_object_wait_close_server(&server, server_thread);
    }

    int failed = event_object_wait_smoke_run_waits(client, &server, server_thread, status);
    dcc_client_destroy(client);
    return failed;
}

#endif
