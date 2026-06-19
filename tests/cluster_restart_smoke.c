#include <dcc/dcc.h>

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
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

typedef struct restart_gateway_server {
    int fd;
    unsigned short port;
    atomic_uint accepts;
    atomic_uint identifies;
    atomic_uint bad;
} restart_gateway_server_t;

typedef struct restart_state {
    dcc_cluster_t *cluster;
    atomic_uint ready_seen;
    atomic_uint bad;
} restart_state_t;

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
    if (len >= 126U) {
        return -1;
    }
    unsigned char header[2] = {0x81U, (unsigned char)len};
    return write_all(fd, header, sizeof(header)) == 0 && write_all(fd, text, len) == 0 ? 0 : -1;
}

static int read_text_frame(int fd, char *out, size_t cap) {
    unsigned char header[2];
    if (cap == 0 || read_exact(fd, header, sizeof(header)) != 0) {
        return -1;
    }
    uint64_t len = header[1] & 0x7fU;
    if (len == 126U || len == 127U || len + 1U > cap) {
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
    return (header[0] & 0x0fU) == 1U ? 0 : -1;
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
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0 || listen(fd, 4) != 0) {
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

static int serve_connection(restart_gateway_server_t *server, int fd, unsigned attempt) {
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    (void)setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (gateway_handshake(fd) != 0 ||
        write_text_frame(fd, "{\"op\":10,\"d\":{\"heartbeat_interval\":1000}}") != 0) {
        return -1;
    }

    char frame[2048];
    if (read_text_frame(fd, frame, sizeof(frame)) != 0 ||
        strstr(frame, "\"op\":2") == NULL ||
        strstr(frame, "\"shard\":[0,1]") == NULL) {
        return -1;
    }
    atomic_fetch_add(&server->identifies, 1U);

    if (attempt == 0) {
        return write_text_frame(fd, "{\"t\":\"READY\",\"s\":1,\"op\":0,\"d\":{\"guilds\":[]}}");
    }
    return write_text_frame(
        fd,
        "{\"t\":\"READY\",\"s\":2,\"op\":0,\"d\":{\"session_id\":\"restart-session\",\"guilds\":[]}}"
    );
}

static void *gateway_server_main(void *arg) {
    restart_gateway_server_t *server = (restart_gateway_server_t *)arg;
    for (unsigned i = 0; i < 2U; ++i) {
        int client = accept(server->fd, NULL, NULL);
        if (client < 0) {
            atomic_store(&server->bad, 1U);
            return NULL;
        }
        atomic_fetch_add(&server->accepts, 1U);
        if (serve_connection(server, client, i) != 0) {
            atomic_store(&server->bad, 1U);
        }
        close(client);
    }
    return NULL;
}

static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    restart_state_t *state = (restart_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready == NULL || ready->shard_id != 0 || ready->shard_count != 1) {
        atomic_store(&state->bad, 1U);
    } else {
        atomic_store(&state->ready_seen, 1U);
    }
    (void)dcc_cluster_stop(state->cluster);
}

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);

    restart_gateway_server_t server;
    memset(&server, 0, sizeof(server));
    atomic_init(&server.accepts, 0U);
    atomic_init(&server.identifies, 0U);
    atomic_init(&server.bad, 0U);
    if (start_listener(&server.fd, &server.port) != 0) {
        fprintf(stderr, "failed to start restart gateway server\n");
        return 1;
    }

    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, gateway_server_main, &server) != 0) {
        close(server.fd);
        return 1;
    }

    char gateway_url[160];
    snprintf(gateway_url, sizeof(gateway_url), "ws://127.0.0.1:%u/?v=10&encoding=json", (unsigned)server.port);

    dcc_cluster_t *cluster = NULL;
    dcc_cluster_options_t opts = {
        .size = sizeof(opts),
        .client_options = {
            .size = sizeof(dcc_client_options_t),
            .token = "mock-token",
            .intents = DCC_INTENT_GUILDS,
            .gateway_url = gateway_url,
        },
        .shard_count = 1,
        .total_shard_count = 1,
        .shard_restart_max = 1,
        .shard_restart_base_delay_ms = 10,
        .shard_restart_max_delay_ms = 10,
    };
    dcc_status_t status = dcc_cluster_create(&opts, &cluster);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_cluster_create failed: %s\n", dcc_status_string(status));
        close(server.fd);
        return 1;
    }

    dcc_cluster_restart_policy_t policy = {
        .size = sizeof(policy),
    };
    dcc_status_t policy_status = dcc_cluster_get_restart_policy(cluster, &policy);

    restart_state_t state;
    memset(&state, 0, sizeof(state));
    state.cluster = cluster;
    atomic_init(&state.ready_seen, 0U);
    atomic_init(&state.bad, 0U);
    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_READY, on_ready, &state, NULL, 0);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_start(cluster);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_wait(cluster);
    }

    (void)pthread_join(server_thread, NULL);
    close(server.fd);

    dcc_cluster_shard_info_t info = {
        .size = sizeof(info),
    };
    dcc_status_t info_status = dcc_cluster_shard_info(cluster, 0, &info);
    dcc_cluster_health_summary_t summary = {
        .size = sizeof(summary),
    };
    dcc_status_t summary_status = dcc_cluster_health_summary(cluster, &summary);
    int ok = status == DCC_OK &&
        policy_status == DCC_OK &&
        policy.max_restarts == 1 &&
        policy.base_delay_ms == 10 &&
        policy.max_delay_ms == 10 &&
        info_status == DCC_OK &&
        summary_status == DCC_OK &&
        info.status == DCC_CLUSTER_SHARD_STOPPED &&
        info.starts == 1 &&
        info.restart_attempts == 1 &&
        info.restarts == 1 &&
        info.restart_suppressed == 0 &&
        info.restart_budget_used == 1 &&
        info.ready_events == 1 &&
        info.log_errors >= 1 &&
        summary.total_shards == 1 &&
        summary.stopped_shards == 1 &&
        summary.starts == 1 &&
        summary.restart_attempts == 1 &&
        summary.restarts == 1 &&
        summary.restart_suppressed == 0 &&
        summary.restart_budget_used == 1 &&
        summary.ready_events == 1 &&
        summary.log_errors >= 1 &&
        atomic_load(&state.ready_seen) == 1U &&
        atomic_load(&state.bad) == 0U &&
        atomic_load(&server.bad) == 0U &&
        atomic_load(&server.accepts) == 2U &&
        atomic_load(&server.identifies) == 2U;

    if (!ok) {
        fprintf(
            stderr,
            "cluster restart smoke failed: status=%s policy=%s info=%s state=%u/%u "
            "summary=%s accepts=%u identifies=%u server_bad=%u health_status=%s starts=%llu attempts=%llu "
            "restarts=%llu suppressed=%llu budget=%llu ready=%llu errors=%llu "
            "summary_attempts=%llu summary_budget=%llu last=%s\n",
            dcc_status_string(status),
            dcc_status_string(policy_status),
            dcc_status_string(info_status),
            atomic_load(&state.ready_seen),
            atomic_load(&state.bad),
            dcc_status_string(summary_status),
            atomic_load(&server.accepts),
            atomic_load(&server.identifies),
            atomic_load(&server.bad),
            dcc_cluster_shard_status_string(info.status),
            (unsigned long long)info.starts,
            (unsigned long long)info.restart_attempts,
            (unsigned long long)info.restarts,
            (unsigned long long)info.restart_suppressed,
            (unsigned long long)info.restart_budget_used,
            (unsigned long long)info.ready_events,
            (unsigned long long)info.log_errors,
            (unsigned long long)summary.restart_attempts,
            (unsigned long long)summary.restart_budget_used,
            info.last_error
        );
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_destroy(cluster);
    return 0;
}

#endif
