#include "internal/ws/dcc_ws.h"

#include <dcc/dcc.h>
#include <llam/runtime.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

typedef struct ws_server {
    int fd;
    unsigned short port;
} ws_server_t;

typedef struct ws_client_task {
    char url[128];
    dcc_status_t status;
    int saw_world;
    int saw_close_code;
} ws_client_task_t;

static char *find_header(char *headers, const char *name) {
    size_t name_len = strlen(name);
    char *line = headers;
    while (line != NULL && *line != '\0') {
        char *next = strstr(line, "\r\n");
        if (next != NULL) {
            *next = '\0';
        }
        char *colon = strchr(line, ':');
        if (colon != NULL && (size_t)(colon - line) == name_len && strncasecmp(line, name, name_len) == 0) {
            char *value = colon + 1;
            while (*value == ' ' || *value == '\t') {
                value++;
            }
            return value;
        }
        if (next == NULL) {
            break;
        }
        line = next + 2;
    }
    return NULL;
}

static int ws_accept_for_key(const char *key, char out[64]) {
    char material[128];
    unsigned char digest[SHA_DIGEST_LENGTH];
    int material_len = snprintf(material, sizeof(material), "%s%s", key, WS_GUID);
    if (material_len <= 0 || (size_t)material_len >= sizeof(material)) {
        return -1;
    }
    SHA1((const unsigned char *)material, (size_t)material_len, digest);
    int encoded = EVP_EncodeBlock((unsigned char *)out, digest, SHA_DIGEST_LENGTH);
    if (encoded <= 0 || encoded >= 64) {
        return -1;
    }
    out[encoded] = '\0';
    return 0;
}

static int read_http_upgrade(int client, char *buf, size_t cap) {
    size_t used = 0;
    while (used + 1 < cap) {
        ssize_t n = read(client, buf + used, cap - used - 1);
        if (n <= 0) {
            return -1;
        }
        used += (size_t)n;
        buf[used] = '\0';
        if (strstr(buf, "\r\n\r\n") != NULL) {
            return 0;
        }
    }
    return -1;
}

static int read_client_text_frame(int client, char *out, size_t out_cap) {
    unsigned char header[2];
    if (read(client, header, sizeof(header)) != (ssize_t)sizeof(header)) {
        return -1;
    }
    if ((header[0] & 0x0fU) != 0x1U || (header[1] & 0x80U) == 0) {
        return -1;
    }
    size_t len = header[1] & 0x7fU;
    if (len >= 126U || len + 1 > out_cap) {
        return -1;
    }
    unsigned char mask[4];
    if (read(client, mask, sizeof(mask)) != (ssize_t)sizeof(mask)) {
        return -1;
    }
    unsigned char payload[128];
    if (read(client, payload, len) != (ssize_t)len) {
        return -1;
    }
    for (size_t i = 0; i < len; ++i) {
        out[i] = (char)(payload[i] ^ mask[i % 4]);
    }
    out[len] = '\0';
    return 0;
}

static int write_server_text_frame(int client, const char *text) {
    size_t len = strlen(text);
    if (len >= 126U) {
        return -1;
    }
    unsigned char header[2] = {
        0x81U,
        (unsigned char)len,
    };
    if (write(client, header, sizeof(header)) != (ssize_t)sizeof(header)) {
        return -1;
    }
    if (write(client, text, len) != (ssize_t)len) {
        return -1;
    }
    return 0;
}

static int write_server_fragmented_text_frame(int client, const char *first, const char *second) {
    size_t first_len = strlen(first);
    size_t second_len = strlen(second);
    if (first_len >= 126U || second_len >= 126U) {
        return -1;
    }

    unsigned char first_header[2] = {
        0x01U,
        (unsigned char)first_len,
    };
    unsigned char second_header[2] = {
        0x80U,
        (unsigned char)second_len,
    };
    if (write(client, first_header, sizeof(first_header)) != (ssize_t)sizeof(first_header)) {
        return -1;
    }
    if (write(client, first, first_len) != (ssize_t)first_len) {
        return -1;
    }
    if (write(client, second_header, sizeof(second_header)) != (ssize_t)sizeof(second_header)) {
        return -1;
    }
    if (write(client, second, second_len) != (ssize_t)second_len) {
        return -1;
    }
    return 0;
}

static int write_server_close_frame(int client, uint16_t code) {
    unsigned char frame[4] = {
        0x88U,
        0x02U,
        (unsigned char)((code >> 8U) & 0xffU),
        (unsigned char)(code & 0xffU),
    };
    return write(client, frame, sizeof(frame)) == (ssize_t)sizeof(frame) ? 0 : -1;
}

static void *ws_server_main(void *arg) {
    ws_server_t *server = (ws_server_t *)arg;
    int client = accept(server->fd, NULL, NULL);
    if (client < 0) {
        return NULL;
    }

    char request[4096];
    if (read_http_upgrade(client, request, sizeof(request)) != 0) {
        close(client);
        return NULL;
    }

    char *key = find_header(request, "Sec-WebSocket-Key");
    char accept_value[64];
    if (key == NULL || ws_accept_for_key(key, accept_value) != 0) {
        close(client);
        return NULL;
    }

    char response[512];
    int response_len = snprintf(
        response,
        sizeof(response),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "\r\n",
        accept_value
    );
    if (response_len <= 0 || write(client, response, (size_t)response_len) != response_len) {
        close(client);
        return NULL;
    }

    char client_text[128];
    if (read_client_text_frame(client, client_text, sizeof(client_text)) == 0 && strcmp(client_text, "hello") == 0) {
        (void)write_server_fragmented_text_frame(client, "wo", "rld");
        (void)write_server_close_frame(client, 4004);
    }

    close(client);
    return NULL;
}

static int start_server(ws_server_t *server, pthread_t *thread) {
    memset(server, 0, sizeof(*server));
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

    if (pthread_create(thread, NULL, ws_server_main, server) != 0) {
        close(server->fd);
        return -1;
    }
    return 0;
}

static void ws_client_main(void *arg) {
    ws_client_task_t *task = (ws_client_task_t *)arg;
    dcc_ws_t *ws = NULL;
    task->status = dcc_ws_connect(&ws, task->url, 5000);
    if (task->status != DCC_OK) {
        llam_runtime_request_stop();
        return;
    }

    task->status = dcc_ws_send_text(ws, "hello", 5);
    if (task->status == DCC_OK) {
        dcc_ws_message_t message;
        task->status = dcc_ws_recv(ws, &message);
        if (task->status == DCC_OK &&
            message.opcode == DCC_WS_TEXT &&
            message.len == 5 &&
            memcmp(message.data, "world", 5) == 0) {
            task->saw_world = 1;
        }
        dcc_ws_message_deinit(&message);
        if (task->status == DCC_OK) {
            task->status = dcc_ws_recv(ws, &message);
            if (task->status == DCC_ERR_CANCELED && dcc_ws_last_close_code(ws) == 4004) {
                task->saw_close_code = 1;
                task->status = DCC_OK;
            }
            dcc_ws_message_deinit(&message);
        }
    }

    dcc_ws_close(ws, 1000, "done");
    dcc_ws_destroy(ws);
    llam_runtime_request_stop();
}

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);

    ws_server_t server;
    pthread_t server_thread;
    if (start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "failed to start websocket smoke server\n");
        return 1;
    }

    ws_client_task_t task;
    memset(&task, 0, sizeof(task));
    snprintf(task.url, sizeof(task.url), "ws://127.0.0.1:%u/gateway", (unsigned)server.port);

    llam_runtime_opts_t opts;
    if (llam_runtime_opts_init(&opts, LLAM_RUNTIME_OPTS_CURRENT_SIZE) != 0 ||
        llam_runtime_init_ex(&opts, LLAM_RUNTIME_OPTS_CURRENT_SIZE) != 0) {
        fprintf(stderr, "failed to init LLAM runtime\n");
        close(server.fd);
        return 1;
    }

    llam_task_t *client_task = llam_spawn(ws_client_main, &task, NULL);
    if (client_task == NULL || llam_detach(client_task) != 0) {
        fprintf(stderr, "failed to spawn LLAM websocket task\n");
        llam_runtime_shutdown();
        close(server.fd);
        return 1;
    }

    int run_rc = llam_run();
    llam_runtime_shutdown();

    (void)pthread_join(server_thread, NULL);
    close(server.fd);

    if (run_rc != 0 || task.status != DCC_OK || !task.saw_world || !task.saw_close_code) {
        fprintf(stderr,
                "websocket smoke failed: run=%d status=%s saw_world=%d close=%d\n",
                run_rc,
                dcc_status_string(task.status),
                task.saw_world,
                task.saw_close_code);
        return 1;
    }
    return 0;
}

#endif
