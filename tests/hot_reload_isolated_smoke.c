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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct isolated_server {
    int fd;
    uint16_t port;
    char request[4096];
    size_t request_len;
} isolated_server_t;

typedef struct isolated_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
} isolated_wait_state_t;

static void *isolated_wait_main(void *arg) {
    isolated_wait_state_t *state = (isolated_wait_state_t *)arg;
    state->status = dcc_client_wait(state->client);
    return NULL;
}

static void *isolated_server_main(void *arg) {
    isolated_server_t *server = (isolated_server_t *)arg;
    int client = accept(server->fd, NULL, NULL);
    if (client < 0) {
        return NULL;
    }
    ssize_t n = read(client, server->request, sizeof(server->request) - 1U);
    if (n > 0) {
        server->request_len = (size_t)n;
        server->request[n] = '\0';
    }
    const char response[] =
        "HTTP/1.1 204 No Content\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n\r\n";
    (void)write(client, response, sizeof(response) - 1U);
    close(client);
    return NULL;
}

static int isolated_server_start(isolated_server_t *server) {
    memset(server, 0, sizeof(*server));
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
        listen(server->fd, 1) != 0) {
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

static int dispatch_raw_ready(dcc_client_t *client) {
    static const char ready_json[] =
        "{\"op\":0,\"s\":1,\"t\":\"READY\",\"d\":{\"session_id\":\"isolated\","
        "\"application\":{\"id\":\"1\"},\"user\":{\"id\":\"1\",\"username\":\"bot\","
        "\"discriminator\":\"0000\",\"bot\":true},\"guilds\":[]}}";
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_READY;
    event.raw.type = DCC_EVENT_READY;
    event.raw.sequence = 1;
    event.raw.name = "READY";
    event.raw.json = ready_json;
    event.raw.json_len = sizeof(ready_json) - 1U;
    return dcc_event_bus_dispatch(&client->events, client, &event) == DCC_OK ? 0 : -1;
}

static const char *set_worker_path_lookup_env(char *path_buffer, size_t path_buffer_size) {
    const char *worker = DCC_HOT_RELOAD_WORKER;
    const char *slash = strrchr(worker, '/');
    if (slash == NULL) {
        return worker;
    }

    size_t dir_len = (size_t)(slash - worker);
    const char *old_path = getenv("PATH");
    int n = snprintf(
        path_buffer,
        path_buffer_size,
        "%.*s%s%s",
        (int)dir_len,
        worker,
        old_path != NULL && old_path[0] != '\0' ? ":" : "",
        old_path != NULL ? old_path : ""
    );
    if (n <= 0 || (size_t)n >= path_buffer_size || setenv("PATH", path_buffer, 1) != 0) {
        return NULL;
    }
    return slash + 1;
}

int main(void) {
    isolated_server_t server;
    if (isolated_server_start(&server) != 0) {
        fprintf(stderr, "isolated test server start failed: %s\n", strerror(errno));
        return 1;
    }
    pthread_t thread;
    if (pthread_create(&thread, NULL, isolated_server_main, &server) != 0) {
        close(server.fd);
        return 1;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%u/isolated", (unsigned)server.port);
    setenv("DCC_ISOLATED_TEST_URL", url, 1);
    char path_buffer[4096];
    const char *worker_name = set_worker_path_lookup_env(path_buffer, sizeof(path_buffer));
    if (worker_name == NULL || worker_name[0] == '\0') {
        fprintf(stderr, "isolated worker PATH setup failed\n");
        close(server.fd);
        pthread_join(thread, NULL);
        return 1;
    }

    dcc_client_options_t client_options = { .size = sizeof(client_options), .token = "" };
    dcc_client_t *client = NULL;
    dcc_status_t status = dcc_client_create(&client_options, &client);
    dcc_hot_reload_t *hot_reload = NULL;
    isolated_wait_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_RUNTIME,
    };
    pthread_t wait_thread;
    int wait_thread_started = 0;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = worker_name,
        .worker_health_timeout_ms = 3000,
        .worker_drain_timeout_ms = 300,
    };
    if (status == DCC_OK) {
        status = dcc_client_start(client);
    }
    if (status == DCC_OK &&
        pthread_create(&wait_thread, NULL, isolated_wait_main, &wait_state) == 0) {
        wait_thread_started = 1;
    } else if (status == DCC_OK) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_create(client, DCC_HOT_RELOAD_ISOLATED_MODULE, &options, &hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_start(hot_reload);
    }
    if (status == DCC_OK) {
        usleep(200000U);
    }
    dcc_hot_reload_health_snapshot_t health = {
        .size = sizeof(health),
    };
    char health_json[2048];
    if (status == DCC_OK &&
        (dcc_hot_reload_wait_until_loaded(hot_reload, 1000, &health) != DCC_OK ||
         health.backend != DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER ||
         health.loaded != 1U ||
         health.worker_supervisor_running != 1U ||
         health.active_worker_loaded != 1U ||
         health.active_worker_generation != 1U ||
         health.active_worker_pid == 0U ||
         health.last_good_worker_loaded != 0U ||
         health.last_good_worker_generation != 0U ||
         health.last_good_worker_pid != 0U ||
         health.worker_health_timeout_ms != 3000U ||
         health.worker_drain_timeout_ms != 300U ||
         dcc_hot_reload_health_snapshot_json(&health, health_json, sizeof(health_json), NULL) != DCC_OK ||
         strstr(health_json, "\"backend\":\"isolated_worker\"") == NULL ||
         strstr(health_json, "\"active_worker\":{\"loaded\":true,\"generation\":1,\"pid\":") == NULL ||
         strstr(health_json, "\"last_good_worker\":{\"loaded\":false,\"generation\":0,\"pid\":0}") == NULL)) {
        fprintf(stderr, "isolated health snapshot failed: %s\n", health_json);
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK && dispatch_raw_ready(client) != 0) {
        status = DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_destroy(hot_reload);
    (void)dcc_client_stop(client);
    if (wait_thread_started) {
        pthread_join(wait_thread, NULL);
        if (wait_state.status != DCC_OK && status == DCC_OK) {
            status = wait_state.status;
        }
    }
    dcc_client_destroy(client);
    close(server.fd);
    pthread_join(thread, NULL);

    if (status != DCC_OK ||
        strstr(server.request, "POST /isolated HTTP/1.1") == NULL ||
        strstr(server.request, "{\"worker\":true}") == NULL) {
        fprintf(stderr, "isolated worker smoke failed: %s\n%s\n", dcc_status_string(status), server.request);
        return 1;
    }
    return 0;
}
#endif
