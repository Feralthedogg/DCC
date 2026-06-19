#include "http_smoke_server.h"

#if !defined(_WIN32)

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static void *http_server_main(void *arg) {
    http_server_t *server = (http_server_t *)arg;
    int request_total = server->chunked == 2 || server->chunked == 4 || server->chunked == 5 ? 2 : 1;

    for (int request_index = 0; request_index < request_total; ++request_index) {
        int client = accept(server->fd, NULL, NULL);
        if (client < 0) {
            return NULL;
        }
#if defined(SO_NOSIGPIPE)
        int no_sigpipe = 1;
        (void)setsockopt(client, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe, sizeof(no_sigpipe));
#endif

        char request_buf[8192];
        ssize_t request_len = read(client, request_buf, sizeof(request_buf) - 1);
        if (request_len < 0) {
            request_len = 0;
        }
        request_buf[request_len] = '\0';
        (void)sscanf(request_buf, "%15s %511s", server->method, server->path);
        char *headers_end = strstr(request_buf, "\r\n\r\n");
        if (headers_end != NULL) {
            size_t headers_len = (size_t)(headers_end - request_buf);
            if (headers_len >= sizeof(server->headers)) {
                headers_len = sizeof(server->headers) - 1U;
            }
            memcpy(server->headers, request_buf, headers_len);
            server->headers[headers_len] = '\0';
            if (request_index < 2) {
                memcpy(server->request_headers[request_index], request_buf, headers_len);
                server->request_headers[request_index][headers_len] = '\0';
            }
        }
        if (request_index < 2) {
            snprintf(server->methods[request_index], sizeof(server->methods[request_index]), "%s", server->method);
            snprintf(server->paths[request_index], sizeof(server->paths[request_index]), "%s", server->path);
        }
        server->request_count++;
        atomic_fetch_add_explicit(&server->requests_seen, 1U, memory_order_release);
        char *body = strstr(request_buf, "\r\n\r\n");
        char *content_type = strstr(request_buf, "\r\nContent-Type: ");
        if (content_type != NULL && (body == NULL || content_type < body)) {
            content_type += sizeof("\r\nContent-Type: ") - 1U;
            char *content_type_end = strstr(content_type, "\r\n");
            if (content_type_end != NULL) {
                size_t content_type_len = (size_t)(content_type_end - content_type);
                if (content_type_len >= sizeof(server->content_type)) {
                    content_type_len = sizeof(server->content_type) - 1U;
                }
                memcpy(server->content_type, content_type, content_type_len);
                server->content_type[content_type_len] = '\0';
            }
        }
        if (body != NULL) {
            body += 4;
            size_t body_len = (size_t)(request_buf + request_len - body);
            if (body_len >= sizeof(server->body)) {
                body_len = sizeof(server->body) - 1U;
            }
            memcpy(server->body, body, body_len);
            server->body[body_len] = '\0';
            server->body_len = body_len;
            if (request_index < 2) {
                memcpy(server->bodies[request_index], body, body_len);
                server->bodies[request_index][body_len] = '\0';
                server->body_lens[request_index] = body_len;
            }
        }

        if (server->delay_ms != 0) {
            usleep((useconds_t)server->delay_ms * 1000U);
        }

        if (server->chunked == 1) {
            const char response[] =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Transfer-Encoding: chunked\r\n"
                "Connection: close\r\n"
                "\r\n"
                "2\r\nhe\r\n3\r\nllo\r\n0\r\n\r\n";
            (void)write(client, response, sizeof(response) - 1);
        } else if (server->chunked == 2 && request_index == 0) {
            const char response[] =
                "HTTP/1.1 429 Too Many Requests\r\n"
                "Content-Type: application/json\r\n"
                "Retry-After: 0.02\r\n"
                "X-RateLimit-Global: true\r\n"
                "X-RateLimit-Bucket: retry-smoke\r\n"
                "Content-Length: 40\r\n"
                "Connection: close\r\n"
                "\r\n"
                "{\"message\":\"rate limited\",\"global\":true}";
            (void)write(client, response, sizeof(response) - 1);
        } else if (server->chunked == 3) {
            const char response[] =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "X-RateLimit-Remaining: 0\r\n"
                "X-RateLimit-Reset-After: 0.08\r\n"
                "X-RateLimit-Bucket: channel-message-read\r\n"
                "Content-Length: 5\r\n"
                "Connection: close\r\n"
                "\r\n"
                "hello";
            (void)write(client, response, sizeof(response) - 1);
        } else if (server->chunked == 4 && request_index == 0) {
            const char response[] =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: 12\r\n"
                "Connection: close\r\n"
                "\r\n"
                "{\"id\":\"222\"}";
            (void)write(client, response, sizeof(response) - 1);
        } else if (server->chunked == 5 && request_index == 0) {
            const char response[] =
                "HTTP/1.1 429 Too Many Requests\r\n"
                "Content-Type: application/json\r\n"
                "Retry-After: 0.75\r\n"
                "X-RateLimit-Bucket: async-retry-queue\r\n"
                "Content-Length: 41\r\n"
                "Connection: close\r\n"
                "\r\n"
                "{\"message\":\"rate limited\",\"global\":false}";
            (void)write(client, response, sizeof(response) - 1);
        } else if (server->chunked == 6) {
            const char response[] =
                "HTTP/1.1 401 Unauthorized\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: 26\r\n"
                "Connection: close\r\n"
                "\r\n"
                "{\"message\":\"unauthorized\"}";
            (void)write(client, response, sizeof(response) - 1);
        } else {
            const char response[] =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 5\r\n"
                "Connection: close\r\n"
                "\r\n"
                "hello";
            (void)write(client, response, sizeof(response) - 1);
        }
        close(client);
    }
    return NULL;
}

int start_server_mode(http_server_t *server, pthread_t *thread, int chunked) {
    memset(server, 0, sizeof(*server));
    atomic_init(&server->requests_seen, 0U);
    server->chunked = chunked;
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

    if (bind(server->fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0) {
        close(server->fd);
        return -1;
    }
    if (listen(server->fd, 1) != 0) {
        close(server->fd);
        return -1;
    }

    socklen_t len = sizeof(addr);
    if (getsockname(server->fd, (struct sockaddr *)&addr, &len) != 0) {
        close(server->fd);
        return -1;
    }
    server->port = ntohs(addr.sin_port);

    if (pthread_create(thread, NULL, http_server_main, server) != 0) {
        close(server->fd);
        return -1;
    }
    return 0;
}

int start_server(http_server_t *server, pthread_t *thread) {
    return start_server_mode(server, thread, 0);
}

int start_rate_limited_server(http_server_t *server, pthread_t *thread) {
    return start_server_mode(server, thread, 2);
}

int start_bucket_empty_server(http_server_t *server, pthread_t *thread) {
    return start_server_mode(server, thread, 3);
}

int start_direct_message_server(http_server_t *server, pthread_t *thread) {
    return start_server_mode(server, thread, 4);
}

int start_async_retry_queue_server(http_server_t *server, pthread_t *thread) {
    return start_server_mode(server, thread, 5);
}

uint64_t test_now_ms(void) {
    struct timeval tv;
    (void)gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000U + (uint64_t)tv.tv_usec / 1000U;
}

void set_api_base_for_server(const http_server_t *server) {
    char base[128];
    snprintf(base, sizeof(base), "http://127.0.0.1:%u", (unsigned)server->port);
    (void)setenv("DCC_DISCORD_API_BASE", base, 1);
}

#endif
