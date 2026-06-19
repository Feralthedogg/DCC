#include "cluster_chaos_smoke_server.h"

#if !defined(_WIN32)

#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

typedef struct cluster_chaos_conn {
    cluster_chaos_server_t *server;
    int fd;
} cluster_chaos_conn_t;

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

static int write_close_frame(int fd, uint16_t code, const char *reason) {
    size_t reason_len = reason != NULL ? strlen(reason) : 0;
    if (reason_len > 120U) {
        reason_len = 120U;
    }
    unsigned char frame[128];
    size_t payload_len = reason_len + 2U;
    frame[0] = 0x88U;
    frame[1] = (unsigned char)payload_len;
    frame[2] = (unsigned char)((code >> 8U) & 0xffU);
    frame[3] = (unsigned char)(code & 0xffU);
    if (reason_len > 0) {
        memcpy(frame + 4, reason, reason_len);
    }
    return write_all(fd, frame, payload_len + 2U);
}

static int read_text_frame(int fd, char *out, size_t cap) {
    unsigned char header[2];
    if (cap == 0 || read_exact(fd, header, sizeof(header)) != 0) {
        return -1;
    }
    unsigned opcode = header[0] & 0x0fU;
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
    if ((header[1] & 0x80U) && read_exact(fd, mask, sizeof(mask)) != 0) {
        return -1;
    }
    if (read_exact(fd, out, (size_t)len) != 0) {
        return -1;
    }
    if (header[1] & 0x80U) {
        for (uint64_t i = 0; i < len; ++i) {
            out[i] = (char)((unsigned char)out[i] ^ mask[i & 3U]);
        }
    }
    out[len] = '\0';
    return opcode == 1U ? 0 : (opcode == 8U ? 1 : 2);
}

static int accept_with_timeout(int fd) {
    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(fd, &reads);

    struct timeval timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;
    int selected = select(fd + 1, &reads, NULL, NULL, &timeout);
    if (selected <= 0) {
        return -1;
    }
    return accept(fd, NULL, NULL);
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
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0 || listen(fd, 16) != 0) {
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

static int read_until_identify_or_resume(int fd, char *frame, size_t frame_cap, unsigned *out_op) {
    for (unsigned i = 0; i < 16U; ++i) {
        int frame_status = read_text_frame(fd, frame, frame_cap);
        if (frame_status != 0) {
            return -1;
        }
        if (strstr(frame, "\"op\":1") != NULL) {
            (void)write_text_frame(fd, "{\"op\":11,\"d\":null}");
            continue;
        }
        if (strstr(frame, "\"op\":2") != NULL) {
            *out_op = 2U;
            return 0;
        }
        if (strstr(frame, "\"op\":6") != NULL) {
            *out_op = 6U;
            return 0;
        }
    }
    return -1;
}

static int parse_identify_shard(const char *frame, unsigned *out_shard) {
    char *shard = strstr(frame, "\"shard\":[");
    unsigned shard_id = CLUSTER_CHAOS_SHARDS;
    unsigned shard_count = 0;
    if (shard == NULL ||
        sscanf(shard, "\"shard\":[%u,%u]", &shard_id, &shard_count) != 2 ||
        shard_id >= CLUSTER_CHAOS_SHARDS ||
        shard_count != CLUSTER_CHAOS_SHARDS) {
        return -1;
    }
    *out_shard = shard_id;
    return 0;
}

static int parse_resume_shard(const char *frame, unsigned *out_shard) {
    const char *session = strstr(frame, "cluster-chaos-");
    unsigned shard_id = CLUSTER_CHAOS_SHARDS;
    if (session == NULL ||
        sscanf(session, "cluster-chaos-%u", &shard_id) != 1 ||
        shard_id >= CLUSTER_CHAOS_SHARDS) {
        return -1;
    }
    *out_shard = shard_id;
    return 0;
}

static int write_ready(cluster_chaos_server_t *server, int fd, unsigned shard, unsigned seq, const char *suffix) {
    char ready[512];
    int ready_len = snprintf(
        ready,
        sizeof(ready),
        "{\"t\":\"READY\",\"s\":%u,\"op\":0,\"d\":{\"session_id\":\"cluster-chaos-%u%s\","
        "\"guilds\":[],\"resume_gateway_url\":\"ws://127.0.0.1:%u/?v=10&encoding=json\"}}",
        seq,
        shard,
        suffix,
        (unsigned)server->port
    );
    return ready_len > 0 && (size_t)ready_len < sizeof(ready)
        ? write_text_frame(fd, ready)
        : -1;
}

static void gateway_frame_settle(void) {
    (void)usleep(20000U);
}

static void drain_client_close(int fd) {
    char frame[256];
    for (unsigned i = 0; i < 8U; ++i) {
        int frame_status = read_text_frame(fd, frame, sizeof(frame));
        if (frame_status == 1) {
            return;
        }
        if (frame_status != 0) {
            return;
        }
        if (strstr(frame, "\"op\":1") != NULL) {
            (void)write_text_frame(fd, "{\"op\":11,\"d\":null}");
        }
    }
}

static int write_resumed(int fd, unsigned shard) {
    char resumed[96];
    int resumed_len = snprintf(
        resumed,
        sizeof(resumed),
        "{\"t\":\"RESUMED\",\"s\":%u,\"op\":0,\"d\":{}}",
        shard * 100U + 2U
    );
    return resumed_len > 0 && (size_t)resumed_len < sizeof(resumed)
        ? write_text_frame(fd, resumed)
        : -1;
}

static int serve_connection(cluster_chaos_server_t *server, int fd) {
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    (void)setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (gateway_handshake(fd) != 0 ||
        write_text_frame(fd, "{\"op\":10,\"d\":{\"heartbeat_interval\":100}}") != 0) {
        return -1;
    }

    char frame[2048];
    unsigned op = 0;
    if (read_until_identify_or_resume(fd, frame, sizeof(frame), &op) != 0) {
        return -1;
    }

    unsigned shard = CLUSTER_CHAOS_SHARDS;
    if (op == 2U) {
        if (parse_identify_shard(frame, &shard) != 0) {
            return -1;
        }
        unsigned expected = 0U;
        if (atomic_compare_exchange_strong(&server->shard_phase[shard], &expected, 1U)) {
            atomic_fetch_add(&server->identifies, 1U);
            if (write_ready(server, fd, shard, shard * 100U + 1U, "") != 0) {
                return -1;
            }
            gateway_frame_settle();
            if (write_close_frame(fd, 4000, "cluster chaos transient close") != 0) {
                return -1;
            }
            drain_client_close(fd);
            atomic_fetch_add(&server->close_4000, 1U);
            return 0;
        }

        expected = 2U;
        if (!atomic_compare_exchange_strong(&server->shard_phase[shard], &expected, 3U)) {
            return -1;
        }
        atomic_fetch_add(&server->identifies, 1U);
        if (write_ready(server, fd, shard, shard * 100U + 3U, "-final") != 0) {
            return -1;
        }
        atomic_fetch_add(&server->final_ready, 1U);
        while (read_text_frame(fd, frame, sizeof(frame)) == 0) {
            if (strstr(frame, "\"op\":1") != NULL) {
                (void)write_text_frame(fd, "{\"op\":11,\"d\":null}");
            }
        }
        return 0;
    }

    if (parse_resume_shard(frame, &shard) != 0) {
        return -1;
    }
    unsigned expected = 1U;
    if (!atomic_compare_exchange_strong(&server->shard_phase[shard], &expected, 2U)) {
        return -1;
    }
    atomic_fetch_add(&server->resumes, 1U);
    if (write_resumed(fd, shard) != 0) {
        return -1;
    }
    gateway_frame_settle();
    if (write_close_frame(fd, 4009, "cluster chaos stale session") != 0) {
        return -1;
    }
    drain_client_close(fd);
    atomic_fetch_add(&server->close_4009, 1U);
    return 0;
}

static void *gateway_conn_main(void *arg) {
    cluster_chaos_conn_t *conn = (cluster_chaos_conn_t *)arg;
    cluster_chaos_server_t *server = conn->server;
    int fd = conn->fd;
    free(conn);

    if (serve_connection(server, fd) != 0) {
        atomic_store(&server->bad, 1U);
    }
    close(fd);
    return NULL;
}

static void *gateway_server_main(void *arg) {
    cluster_chaos_server_t *server = (cluster_chaos_server_t *)arg;
    for (unsigned i = 0; i < CLUSTER_CHAOS_CONNECTIONS; ++i) {
        int client = accept_with_timeout(server->fd);
        if (client < 0) {
            atomic_store(&server->bad, 1U);
            return NULL;
        }
        atomic_fetch_add(&server->accepts, 1U);

        cluster_chaos_conn_t *conn = (cluster_chaos_conn_t *)malloc(sizeof(*conn));
        if (conn == NULL) {
            atomic_store(&server->bad, 1U);
            close(client);
            return NULL;
        }
        conn->server = server;
        conn->fd = client;
        unsigned handler_index = atomic_fetch_add(&server->handler_count, 1U);
        if (handler_index >= CLUSTER_CHAOS_CONNECTIONS ||
            pthread_create(&server->handlers[handler_index], NULL, gateway_conn_main, conn) != 0) {
            atomic_store(&server->bad, 1U);
            close(client);
            free(conn);
            return NULL;
        }
    }

    unsigned handlers = atomic_load(&server->handler_count);
    for (unsigned i = 0; i < handlers; ++i) {
        (void)pthread_join(server->handlers[i], NULL);
    }
    return NULL;
}

int dcc_test_cluster_chaos_start_gateway_server(cluster_chaos_server_t *server, pthread_t *thread) {
    memset(server, 0, sizeof(*server));
    atomic_init(&server->handler_count, 0U);
    atomic_init(&server->accepts, 0U);
    atomic_init(&server->identifies, 0U);
    atomic_init(&server->resumes, 0U);
    atomic_init(&server->close_4000, 0U);
    atomic_init(&server->close_4009, 0U);
    atomic_init(&server->final_ready, 0U);
    atomic_init(&server->bad, 0U);
    for (unsigned i = 0; i < CLUSTER_CHAOS_SHARDS; ++i) {
        atomic_init(&server->shard_phase[i], 0U);
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

#endif
