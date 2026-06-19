#include "gateway_smoke_api_server.h"

#if !defined(_WIN32)

#include "gateway_smoke_ws.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static void *api_server_main(void *arg) {
    api_server_t *server = (api_server_t *)arg;
    int client = accept(server->fd, NULL, NULL);
    if (client < 0) {
        return NULL;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    (void)setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    (void)setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    char request[4096];
    if (read_http_upgrade(client, request, sizeof(request)) == 0 &&
        strstr(request, "GET /gateway/bot HTTP/1.1") != NULL &&
        strstr(request, "Authorization: Bot mock-token") != NULL) {
        server->saw_request = 1;
    }

    char body[512];
    int body_len = snprintf(
        body,
        sizeof(body),
        "{\"url\":\"ws://127.0.0.1:%u\","
        "\"shards\":3,"
        "\"session_start_limit\":{\"total\":1000,\"remaining\":999,"
        "\"reset_after\":60000,\"max_concurrency\":2}}",
        (unsigned)server->gateway_port
    );
    if (body_len > 0 && (size_t)body_len < sizeof(body)) {
        char response[768];
        int response_len = snprintf(
            response,
            sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            body_len,
            body
        );
        if (response_len > 0 && (size_t)response_len < sizeof(response)) {
            (void)write(client, response, (size_t)response_len);
        }
    }

    close(client);
    return NULL;
}

int start_api_server(api_server_t *server, pthread_t *thread, unsigned short gateway_port) {
    memset(server, 0, sizeof(*server));
    server->gateway_port = gateway_port;
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd < 0) {
        return -1;
    }

    int yes = 1;
    (void)setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(server->fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0 ||
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

    if (pthread_create(thread, NULL, api_server_main, server) != 0) {
        close(server->fd);
        return -1;
    }
    return 0;
}

#endif
