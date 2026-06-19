#include <dcc/dcc.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct http_response {
    int status;
    char body[8192];
} http_response_t;

typedef struct wait_state {
    dcc_interaction_server_t *server;
    dcc_status_t status;
} wait_state_t;

static void *wait_thread_main(void *arg) {
    wait_state_t *state = (wait_state_t *)arg;
    state->status = dcc_interaction_server_wait(state->server);
    return NULL;
}

static dcc_status_t stop_wait_server(
    dcc_interaction_server_t *server,
    pthread_t wait_thread,
    int wait_started
) {
    dcc_status_t status = dcc_interaction_server_stop(server);
    if (wait_started && pthread_join(wait_thread, NULL) != 0) {
        return DCC_ERR_RUNTIME;
    }
    return status;
}

static int write_all(int fd, const char *data, size_t len) {
    size_t offset = 0;
    while (offset < len) {
        ssize_t n = send(fd, data + offset, len - offset, 0);
        if (n <= 0) {
            return 0;
        }
        offset += (size_t)n;
    }
    return 1;
}

static int read_response(int fd, http_response_t *out) {
    char raw[12000];
    size_t used = 0;
    size_t header_len = 0;
    size_t content_len = 0;
    int has_content_len = 0;
    memset(out, 0, sizeof(*out));
    while (used + 1U < sizeof(raw)) {
        ssize_t n = recv(fd, raw + used, sizeof(raw) - used - 1U, 0);
        if (n < 0) {
            return 0;
        }
        if (n == 0) {
            break;
        }
        used += (size_t)n;
        raw[used] = '\0';
        char *body = strstr(raw, "\r\n\r\n");
        if (body == NULL) {
            continue;
        }
        header_len = (size_t)((body + 4) - raw);
        char *content_len_header = strstr(raw, "\r\nContent-Length:");
        if (content_len_header == NULL) {
            content_len_header = strstr(raw, "\r\ncontent-length:");
        }
        if (content_len_header != NULL && content_len_header < body) {
            unsigned long parsed_len = 0;
            if (sscanf(content_len_header, "\r\n%*[^:]: %lu", &parsed_len) == 1) {
                content_len = (size_t)parsed_len;
                has_content_len = 1;
            }
        }
        if (has_content_len && used >= header_len + content_len) {
            break;
        }
    }
    raw[used] = '\0';

    int status = 0;
    if (sscanf(raw, "HTTP/1.1 %d", &status) != 1) {
        return 0;
    }
    char *body = strstr(raw, "\r\n\r\n");
    if (body == NULL) {
        return 0;
    }
    body += 4;
    size_t body_len = used - (size_t)(body - raw);
    if (has_content_len && body_len > content_len) {
        body_len = content_len;
    }
    if (body_len >= sizeof(out->body)) {
        return 0;
    }
    memcpy(out->body, body, body_len);
    out->body[body_len] = '\0';
    out->status = status;
    return 1;
}

static int send_get(uint16_t port, const char *path, http_response_t *out) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return 0;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0) {
        close(fd);
        return 0;
    }

    char request[512];
    int n = snprintf(
        request,
        sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: 127.0.0.1:%u\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n",
        path,
        (unsigned)port
    );
    if (n <= 0 || (size_t)n >= sizeof(request) || !write_all(fd, request, (size_t)n)) {
        close(fd);
        return 0;
    }

    int ok = read_response(fd, out);
    close(fd);
    return ok;
}

int main(void) {
    static const char public_key_hex[] =
        "0000000000000000000000000000000000000000000000000000000000000000";

    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_client_t *client = NULL;
    dcc_status_t status = dcc_client_create(&client_options, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "client create failed: %s\n", dcc_status_string(status));
        return 1;
    }

    dcc_hot_reload_t *hot_reload = NULL;
    status = dcc_hot_reload_create(client, "fixture-module.so", NULL, &hot_reload);
    if (status != DCC_OK || hot_reload == NULL) {
        fprintf(stderr, "hot reload create failed: %s\n", dcc_status_string(status));
        dcc_client_destroy(client);
        return 1;
    }

    dcc_interaction_server_options_t server_options;
    memset(&server_options, 0, sizeof(server_options));
    server_options.size = sizeof(server_options);
    server_options.public_key_hex = public_key_hex;
    server_options.address = "127.0.0.1";
    server_options.port = 0;

    dcc_interaction_server_t *server = NULL;
    status = dcc_interaction_server_create(&server_options, &server);
    if (status != DCC_OK || server == NULL) {
        fprintf(stderr, "interaction server create failed: %s\n", dcc_status_string(status));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        return 1;
    }

    if (dcc_hot_reload_add_health_route(NULL, "/hot-reloadz", hot_reload) != DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_add_health_route(server, "/hot-reloadz", NULL) != DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_add_health_route(server, NULL, hot_reload) != DCC_OK) {
        fprintf(stderr, "hot reload health route setup contract failed\n");
        dcc_interaction_server_destroy(server);
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        return 1;
    }

    status = dcc_interaction_server_start(server);
    if (status != DCC_OK) {
        fprintf(stderr, "interaction server start failed: %s\n", dcc_status_string(status));
        dcc_interaction_server_destroy(server);
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        return 1;
    }

    wait_state_t wait_state = {
        .server = server,
        .status = DCC_ERR_RUNTIME,
    };
    pthread_t wait_thread;
    int wait_started = 0;
    if (pthread_create(&wait_thread, NULL, wait_thread_main, &wait_state) != 0) {
        fprintf(stderr, "interaction server wait thread start failed\n");
        (void)stop_wait_server(server, wait_thread, wait_started);
        dcc_interaction_server_destroy(server);
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        return 1;
    }
    wait_started = 1;

    http_response_t response;
    memset(&response, 0, sizeof(response));
    uint16_t port = dcc_interaction_server_port(server);
    if (!send_get(port, "/hot-reloadz", &response) ||
        response.status != 503 ||
        strstr(response.body, "\"backend\":\"in_process\"") == NULL ||
        strstr(response.body, "\"health\":\"stopped\"") == NULL ||
        strstr(response.body, "\"loaded\":false") == NULL ||
        strstr(
            response.body,
            "\"worker_events\":{\"active_failures\":0,\"last_good_promotions\":0,"
            "\"dispatch_failures\":0,\"temp_error_responses\":0}"
        ) == NULL ||
        strstr(response.body, "\"active_worker\":{\"loaded\":false,\"generation\":0,\"pid\":0}") == NULL ||
        strstr(response.body, "\"last_good_worker\":{\"loaded\":false,\"generation\":0,\"pid\":0}") == NULL) {
        fprintf(stderr, "hot reload health route response mismatch: %d %s\n", response.status, response.body);
        (void)stop_wait_server(server, wait_thread, wait_started);
        dcc_interaction_server_destroy(server);
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        return 1;
    }

    if (dcc_hot_reload_add_health_route(server, "/late-hot-reloadz", hot_reload) != DCC_ERR_STATE) {
        fprintf(stderr, "hot reload health route runtime mutation contract failed\n");
        (void)stop_wait_server(server, wait_thread, wait_started);
        dcc_interaction_server_destroy(server);
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        return 1;
    }

    status = stop_wait_server(server, wait_thread, wait_started);
    if (wait_state.status != DCC_OK) {
        fprintf(stderr, "interaction server wait failed: %s\n", dcc_status_string(wait_state.status));
        status = wait_state.status;
    }
    dcc_interaction_server_destroy(server);
    dcc_hot_reload_destroy(hot_reload);
    dcc_client_destroy(client);
    return status == DCC_OK ? 0 : 1;
}
#endif
