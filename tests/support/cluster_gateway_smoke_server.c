#include "cluster_gateway_smoke_support.h"

#if !defined(_WIN32)

#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

typedef struct cluster_gateway_conn {
    cluster_gateway_server_t *server;
    int fd;
} cluster_gateway_conn_t;

static int write_all(int fd, const void *data, size_t len) {
    const char *cursor = (const char *)data;
    while (len > 0) {
        ssize_t n = write(fd, cursor, len);
        if (n <= 0) {
            return -1;
        }
        cursor += (size_t)n;
        len -= (size_t)n;
    }
    return 0;
}

static int read_exact(int fd, void *data, size_t len) {
    char *cursor = (char *)data;
    while (len > 0) {
        ssize_t n = read(fd, cursor, len);
        if (n <= 0) {
            return -1;
        }
        cursor += (size_t)n;
        len -= (size_t)n;
    }
    return 0;
}

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

static int read_http_request(int fd, char *buf, size_t cap) {
    size_t used = 0;
    while (used + 1U < cap) {
        ssize_t n = read(fd, buf + used, cap - used - 1U);
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

static int write_text_frame(int fd, const char *text) {
    size_t len = strlen(text);
    unsigned char header[4];
    size_t header_len = 0;
    header[header_len++] = 0x81U;
    if (len < 126U) {
        header[header_len++] = (unsigned char)len;
    } else if (len <= 0xffffU) {
        header[header_len++] = 126U;
        header[header_len++] = (unsigned char)(len >> 8U);
        header[header_len++] = (unsigned char)len;
    } else {
        return -1;
    }
    return write_all(fd, header, header_len) == 0 && write_all(fd, text, len) == 0 ? 0 : -1;
}

static int read_text_frame(int fd, char *out, size_t cap) {
    unsigned char header[2];
    if (cap == 0 || read_exact(fd, header, sizeof(header)) != 0) {
        return -1;
    }
    unsigned opcode = header[0] & 0x0fU;
    unsigned masked = header[1] & 0x80U;
    uint64_t len = header[1] & 0x7fU;
    if (len == 126U) {
        unsigned char ext[2];
        if (read_exact(fd, ext, sizeof(ext)) != 0) {
            return -1;
        }
        len = ((uint64_t)ext[0] << 8U) | (uint64_t)ext[1];
    } else if (len == 127U) {
        return -1;
    }
    if (len + 1U > cap) {
        return -1;
    }

    unsigned char mask[4] = {0, 0, 0, 0};
    if (masked && read_exact(fd, mask, sizeof(mask)) != 0) {
        return -1;
    }
    if (read_exact(fd, out, (size_t)len) != 0) {
        return -1;
    }
    if (masked) {
        for (uint64_t i = 0; i < len; ++i) {
            out[i] = (char)((unsigned char)out[i] ^ mask[i & 3U]);
        }
    }
    out[len] = '\0';
    return opcode == 1U ? 0 : (opcode == 8U ? 1 : 2);
}

static int gateway_handshake(int fd) {
    char request[4096];
    if (read_http_request(fd, request, sizeof(request)) != 0) {
        return -1;
    }
    char *key = find_header(request, "Sec-WebSocket-Key");
    if (key == NULL) {
        return -1;
    }
    char accept_value[64];
    if (ws_accept_for_key(key, accept_value) != 0) {
        return -1;
    }
    char response[512];
    int response_len = snprintf(
        response,
        sizeof(response),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n\r\n",
        accept_value
    );
    return response_len > 0 &&
            (size_t)response_len < sizeof(response) &&
            write_all(fd, response, (size_t)response_len) == 0
        ? 0
        : -1;
}

static void *gateway_conn_main(void *arg) {
    cluster_gateway_conn_t *conn = (cluster_gateway_conn_t *)arg;
    cluster_gateway_server_t *server = conn->server;
    int fd = conn->fd;
    free(conn);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    (void)setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (gateway_handshake(fd) != 0 ||
        write_text_frame(fd, "{\"op\":10,\"d\":{\"heartbeat_interval\":1000}}") != 0) {
        atomic_store(&server->bad, 1U);
        close(fd);
        return NULL;
    }

    char identify[2048];
    int saw_identify = 0;
    for (unsigned i = 0; i < 4U; ++i) {
        int frame_status = read_text_frame(fd, identify, sizeof(identify));
        if (frame_status != 0) {
            break;
        }
        if (strstr(identify, "\"op\":1") != NULL) {
            (void)write_text_frame(fd, "{\"op\":11,\"d\":null}");
            continue;
        }
        if (strstr(identify, "\"op\":2") != NULL) {
            saw_identify = 1;
            break;
        }
    }
    if (!saw_identify) {
        atomic_store(&server->bad, 1U);
        close(fd);
        return NULL;
    }

    char *shard = strstr(identify, "\"shard\":[");
    unsigned shard_id = CLUSTER_GATEWAY_SHARDS;
    unsigned shard_count = 0;
    if (shard == NULL ||
        sscanf(shard, "\"shard\":[%u,%u]", &shard_id, &shard_count) != 2 ||
        shard_id >= CLUSTER_GATEWAY_SHARDS ||
        shard_count != CLUSTER_GATEWAY_SHARDS) {
        atomic_store(&server->bad, 1U);
        close(fd);
        return NULL;
    }

    unsigned phase = 0;
    unsigned expected = 0;
    if (atomic_compare_exchange_strong(&server->initial_seen[shard_id], &expected, 1U)) {
        phase = 0;
    } else {
        expected = 0;
        if (atomic_compare_exchange_strong(&server->rolling_seen[shard_id], &expected, 1U)) {
            phase = 1U;
            atomic_fetch_add(&server->rolling_identify_count, 1U);
        } else {
            atomic_store(&server->bad, 1U);
            close(fd);
            return NULL;
        }
    }
    if (phase > 1U) {
        atomic_store(&server->bad, 1U);
        close(fd);
        return NULL;
    }
    atomic_fetch_add(&server->identify_count, 1U);

    char ready[512];
    int ready_len = snprintf(
        ready,
        sizeof(ready),
        "{\"t\":\"READY\",\"s\":1,\"op\":0,\"d\":{\"session_id\":\"cluster-session-%u-%u\","
        "\"guilds\":[],\"resume_gateway_url\":\"ws://127.0.0.1:%u/?v=10&encoding=json\"}}",
        shard_id,
        phase,
        (unsigned)server->port
    );
    if (ready_len <= 0 || (size_t)ready_len >= sizeof(ready) || write_text_frame(fd, ready) != 0) {
        atomic_store(&server->bad, 1U);
        close(fd);
        return NULL;
    }

    char frame[512];
    while (read_text_frame(fd, frame, sizeof(frame)) == 0) {
        if (strstr(frame, "\"op\":1") != NULL) {
            (void)write_text_frame(fd, "{\"op\":11,\"d\":null}");
        }
    }
    close(fd);
    return NULL;
}

static void *gateway_server_main(void *arg) {
    cluster_gateway_server_t *server = (cluster_gateway_server_t *)arg;
    for (unsigned i = 0; i < CLUSTER_GATEWAY_CONNECTIONS; ++i) {
        int client = accept(server->fd, NULL, NULL);
        if (client < 0) {
            atomic_store(&server->bad, 1U);
            return NULL;
        }

        cluster_gateway_conn_t *conn = (cluster_gateway_conn_t *)malloc(sizeof(*conn));
        if (conn == NULL) {
            atomic_store(&server->bad, 1U);
            close(client);
            return NULL;
        }
        conn->server = server;
        conn->fd = client;
        if (pthread_create(&server->handlers[i], NULL, gateway_conn_main, conn) != 0) {
            atomic_store(&server->bad, 1U);
            close(client);
            free(conn);
            return NULL;
        }
    }

    for (unsigned i = 0; i < CLUSTER_GATEWAY_CONNECTIONS; ++i) {
        (void)pthread_join(server->handlers[i], NULL);
    }
    return NULL;
}

static void *api_server_main(void *arg) {
    cluster_api_server_t *server = (cluster_api_server_t *)arg;
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
    if (read_http_request(client, request, sizeof(request)) == 0 &&
        strstr(request, "GET /gateway/bot HTTP/1.1") != NULL &&
        strstr(request, "Authorization: Bot mock-token") != NULL) {
        atomic_fetch_add(&server->requests, 1U);
    }

    char body[512];
    int body_len = snprintf(
        body,
        sizeof(body),
        "{\"url\":\"ws://127.0.0.1:%u\","
        "\"shards\":2,"
        "\"session_start_limit\":{\"total\":1000,\"remaining\":999,"
        "\"reset_after\":60000,\"max_concurrency\":2}}",
        (unsigned)server->gateway_port
    );
    if (body_len > 0 && (size_t)body_len < sizeof(body)) {
        char response[768];
        int response_len = snprintf(
            response,
            sizeof(response),
            "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n"
            "Content-Length: %d\r\nConnection: close\r\n\r\n%s",
            body_len,
            body
        );
        if (response_len > 0 && (size_t)response_len < sizeof(response)) {
            (void)write_all(client, response, (size_t)response_len);
        }
    }
    close(client);
    return NULL;
}

static int start_listener(int *out_fd, unsigned short *out_port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }
    int yes = 1;
    (void)setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0 || listen(fd, 8) != 0) {
        close(fd);
        return -1;
    }

    socklen_t len = sizeof(addr);
    if (getsockname(fd, (struct sockaddr *)&addr, &len) != 0) {
        close(fd);
        return -1;
    }
    *out_fd = fd;
    *out_port = ntohs(addr.sin_port);
    return 0;
}

int dcc_test_cluster_gateway_start_gateway_server(cluster_gateway_server_t *server, pthread_t *thread) {
    memset(server, 0, sizeof(*server));
    atomic_init(&server->identify_count, 0U);
    atomic_init(&server->rolling_identify_count, 0U);
    atomic_init(&server->bad, 0U);
    for (unsigned i = 0; i < CLUSTER_GATEWAY_SHARDS; ++i) {
        atomic_init(&server->initial_seen[i], 0U);
        atomic_init(&server->rolling_seen[i], 0U);
    }
    if (start_listener(&server->fd, &server->port) != 0) {
        return -1;
    }
    if (pthread_create(thread, NULL, gateway_server_main, server) != 0) {
        close(server->fd);
        return -1;
    }
    return 0;
}

int dcc_test_cluster_gateway_start_api_server(
    cluster_api_server_t *server,
    pthread_t *thread,
    unsigned short gateway_port
) {
    memset(server, 0, sizeof(*server));
    server->gateway_port = gateway_port;
    atomic_init(&server->requests, 0U);
    if (start_listener(&server->fd, &server->port) != 0) {
        return -1;
    }
    if (pthread_create(thread, NULL, api_server_main, server) != 0) {
        close(server->fd);
        return -1;
    }
    return 0;
}

#endif
