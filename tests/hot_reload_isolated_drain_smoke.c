#include <dcc/dcc.h>
#include <dcc/hot_reload.h>

#include "internal/events/dcc_events_internal.h"

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct drain_server {
    int fd;
    uint16_t port;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int first_seen;
    int release_first;
    char request[2][4096];
    size_t request_count;
} drain_server_t;

typedef struct dispatch_thread_args {
    dcc_client_t *client;
    int result;
} dispatch_thread_args_t;

typedef struct reload_thread_args {
    dcc_hot_reload_t *hot_reload;
    dcc_status_t status;
} reload_thread_args_t;

static void drain_server_signal_first_seen(drain_server_t *server) {
    pthread_mutex_lock(&server->mutex);
    server->first_seen = 1;
    pthread_cond_broadcast(&server->cond);
    while (!server->release_first) {
        pthread_cond_wait(&server->cond, &server->mutex);
    }
    pthread_mutex_unlock(&server->mutex);
}

static void drain_server_wait_first_seen(drain_server_t *server) {
    pthread_mutex_lock(&server->mutex);
    while (!server->first_seen) {
        pthread_cond_wait(&server->cond, &server->mutex);
    }
    pthread_mutex_unlock(&server->mutex);
}

static void drain_server_release_first(drain_server_t *server) {
    pthread_mutex_lock(&server->mutex);
    server->release_first = 1;
    pthread_cond_broadcast(&server->cond);
    pthread_mutex_unlock(&server->mutex);
}

static void *drain_server_main(void *arg) {
    drain_server_t *server = (drain_server_t *)arg;
    for (size_t i = 0; i < 2U; ++i) {
        struct pollfd pfd = { .fd = server->fd, .events = POLLIN };
        if (poll(&pfd, 1, 5000) <= 0) {
            return NULL;
        }
        int client = accept(server->fd, NULL, NULL);
        if (client < 0) {
            return NULL;
        }
        ssize_t n = read(client, server->request[i], sizeof(server->request[i]) - 1U);
        if (n > 0) {
            server->request[i][n] = '\0';
            server->request_count++;
        }
        if (i == 0U) {
            drain_server_signal_first_seen(server);
        }
        const char response[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 2\r\n"
            "Connection: close\r\n\r\n"
            "ok";
        (void)write(client, response, sizeof(response) - 1U);
        (void)shutdown(client, SHUT_WR);
        close(client);
    }
    return NULL;
}

static int drain_server_start(drain_server_t *server) {
    memset(server, 0, sizeof(*server));
    pthread_mutex_init(&server->mutex, NULL);
    pthread_cond_init(&server->cond, NULL);
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd < 0) {
        return -1;
    }
    int one = 1;
    (void)setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    if (bind(server->fd, (struct sockaddr *)&addr, sizeof(addr)) != 0 ||
        listen(server->fd, 2) != 0) {
        close(server->fd);
        return -1;
    }
    socklen_t len = sizeof(addr);
    if (getsockname(server->fd, (struct sockaddr *)&addr, &len) != 0) {
        close(server->fd);
        return -1;
    }
    server->port = ntohs(addr.sin_port);
    return 0;
}

static void drain_server_destroy(drain_server_t *server) {
    if (server->fd >= 0) {
        close(server->fd);
        server->fd = -1;
    }
    pthread_cond_destroy(&server->cond);
    pthread_mutex_destroy(&server->mutex);
}

static int dispatch_raw_ready(dcc_client_t *client, uint32_t sequence, const char *session_id) {
    char ready_json[512];
    int n = snprintf(
        ready_json,
        sizeof(ready_json),
        "{\"op\":0,\"s\":%u,\"t\":\"READY\",\"d\":{\"session_id\":\"%s\","
        "\"application\":{\"id\":\"1\"},\"user\":{\"id\":\"1\",\"username\":\"bot\","
        "\"discriminator\":\"0000\",\"bot\":true},\"guilds\":[]}}",
        sequence,
        session_id
    );
    if (n <= 0 || (size_t)n >= sizeof(ready_json)) {
        return -1;
    }
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_READY;
    event.raw.type = DCC_EVENT_READY;
    event.raw.sequence = sequence;
    event.raw.name = "READY";
    event.raw.json = ready_json;
    event.raw.json_len = (size_t)n;
    return dcc_event_bus_dispatch(&client->events, client, &event) == DCC_OK ? 0 : -1;
}

static void *dispatch_thread_main(void *arg) {
    dispatch_thread_args_t *args = (dispatch_thread_args_t *)arg;
    args->result = dispatch_raw_ready(args->client, 1U, "isolated-drain-1");
    return NULL;
}

static void *reload_thread_main(void *arg) {
    reload_thread_args_t *args = (reload_thread_args_t *)arg;
    args->status = dcc_hot_reload_reload(args->hot_reload);
    return NULL;
}

int main(void) {
    drain_server_t server;
    if (drain_server_start(&server) != 0) {
        fprintf(stderr, "isolated drain server start failed: %s\n", strerror(errno));
        return 1;
    }
    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, drain_server_main, &server) != 0) {
        drain_server_destroy(&server);
        return 1;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u/isolated-drain", (unsigned)server.port);
    setenv("DCC_ISOLATED_TEST_URL", url, 1);

    dcc_client_options_t client_options = { .size = sizeof(client_options), .token = "" };
    dcc_client_t *client = NULL;
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = DCC_HOT_RELOAD_WORKER,
        .worker_health_timeout_ms = 3000,
        .worker_drain_timeout_ms = 300,
    };
    dcc_status_t status = dcc_client_create(&client_options, &client);
    if (status == DCC_OK) {
        status = dcc_hot_reload_create(client, DCC_HOT_RELOAD_ISOLATED_MODULE, &options, &hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }

    dispatch_thread_args_t dispatch_args = { .client = client, .result = -1 };
    pthread_t dispatch_thread;
    if (status == DCC_OK && pthread_create(&dispatch_thread, NULL, dispatch_thread_main, &dispatch_args) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        drain_server_wait_first_seen(&server);
    }

    reload_thread_args_t reload_args = { .hot_reload = hot_reload, .status = DCC_ERR_RUNTIME };
    pthread_t reload_thread;
    if (status == DCC_OK && pthread_create(&reload_thread, NULL, reload_thread_main, &reload_args) != 0) {
        status = DCC_ERR_RUNTIME;
    }

    if (status == DCC_OK) {
        usleep(50000U);
        drain_server_release_first(&server);
        pthread_join(dispatch_thread, NULL);
        pthread_join(reload_thread, NULL);
        if (dispatch_args.result != 0 || reload_args.status != DCC_OK ||
            dcc_hot_reload_generation(hot_reload) != 2U) {
            status = DCC_ERR_RUNTIME;
        }
    } else {
        drain_server_release_first(&server);
    }

    if (status == DCC_OK && dispatch_raw_ready(client, 2U, "isolated-drain-2") != 0) {
        status = DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_destroy(hot_reload);
    dcc_client_destroy(client);
    drain_server_destroy(&server);
    pthread_join(server_thread, NULL);

    if (status != DCC_OK ||
        server.request_count != 2U ||
        strstr(server.request[0], "POST /isolated-drain HTTP/1.1") == NULL ||
        strstr(server.request[0], "{\"worker\":true}") == NULL ||
        strstr(server.request[1], "POST /isolated-drain HTTP/1.1") == NULL ||
        strstr(server.request[1], "{\"worker\":true}") == NULL) {
        fprintf(stderr,
                "isolated drain failed: %s count=%zu\nfirst:\n%s\nsecond:\n%s\n",
                dcc_status_string(status),
                server.request_count,
                server.request[0],
                server.request[1]);
        return 1;
    }
    return 0;
}
#endif
