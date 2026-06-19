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
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define CHAOS_CONNECTIONS 4U
#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

typedef struct chaos_gateway_server {
    int fd;
    unsigned short port;
    atomic_uint accepts;
    atomic_uint identifies;
    atomic_uint resumes;
    atomic_uint bad;
} chaos_gateway_server_t;

typedef struct chaos_state {
    dcc_client_t *client;
    chaos_gateway_server_t *server;
    atomic_uint ready_count;
    atomic_uint resumed_count;
    atomic_uint close_4000;
    atomic_uint close_4009;
    atomic_uint monitor_done;
    atomic_uint bad;
} chaos_state_t;

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
    timeout.tv_sec = 10;
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

static int read_until_gateway_op(int fd, char *frame, size_t frame_cap, unsigned op) {
    char needle[32];
    int needle_len = snprintf(needle, sizeof(needle), "\"op\":%u", op);
    if (needle_len <= 0 || (size_t)needle_len >= sizeof(needle)) {
        return -1;
    }

    for (unsigned i = 0; i < 12U; ++i) {
        int frame_status = read_text_frame(fd, frame, frame_cap);
        if (frame_status != 0) {
            return -1;
        }
        if (strstr(frame, "\"op\":1") != NULL) {
            (void)write_text_frame(fd, "{\"op\":11,\"d\":null}");
            continue;
        }
        if (strstr(frame, needle) != NULL) {
            return 0;
        }
    }
    return -1;
}

static int write_ready(chaos_gateway_server_t *server, int fd, unsigned seq, const char *session_id) {
    char ready[512];
    int ready_len = snprintf(
        ready,
        sizeof(ready),
        "{\"t\":\"READY\",\"s\":%u,\"op\":0,\"d\":{\"session_id\":\"%s\","
        "\"guilds\":[],\"resume_gateway_url\":\"ws://127.0.0.1:%u/?v=10&encoding=json\"}}",
        seq,
        session_id,
        (unsigned)server->port
    );
    return ready_len > 0 && (size_t)ready_len < sizeof(ready)
        ? write_text_frame(fd, ready)
        : -1;
}

static int write_resumed(int fd, unsigned seq) {
    char resumed[96];
    int resumed_len = snprintf(resumed, sizeof(resumed), "{\"t\":\"RESUMED\",\"s\":%u,\"op\":0,\"d\":{}}", seq);
    return resumed_len > 0 && (size_t)resumed_len < sizeof(resumed)
        ? write_text_frame(fd, resumed)
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

static int serve_connection(chaos_gateway_server_t *server, int fd, unsigned attempt) {
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
    if (attempt == 0U || attempt == 3U) {
        if (read_until_gateway_op(fd, frame, sizeof(frame), 2U) != 0 ||
            strstr(frame, "\"shard\":[0,1]") == NULL) {
            return -1;
        }
        atomic_fetch_add(&server->identifies, 1U);
    } else {
        if (read_until_gateway_op(fd, frame, sizeof(frame), 6U) != 0 ||
            strstr(frame, "\"session_id\":\"chaos-session") == NULL) {
            return -1;
        }
        atomic_fetch_add(&server->resumes, 1U);
    }

    switch (attempt) {
        case 0:
            if (write_ready(server, fd, 1U, "chaos-session") != 0) {
                return -1;
            }
            gateway_frame_settle();
            if (write_close_frame(fd, 4000, "chaos transient close") != 0) {
                return -1;
            }
            drain_client_close(fd);
            break;
        case 1:
            if (write_resumed(fd, 2U) != 0) {
                return -1;
            }
            gateway_frame_settle();
            if (write_text_frame(fd, "{\"op\":7,\"d\":null}") != 0) {
                return -1;
            }
            break;
        case 2:
            if (write_resumed(fd, 3U) != 0) {
                return -1;
            }
            gateway_frame_settle();
            if (write_close_frame(fd, 4009, "chaos stale seq") != 0) {
                return -1;
            }
            drain_client_close(fd);
            break;
        case 3:
            if (write_ready(server, fd, 4U, "chaos-session-final") != 0) {
                return -1;
            }
            while (read_text_frame(fd, frame, sizeof(frame)) == 0) {
                if (strstr(frame, "\"op\":1") != NULL) {
                    (void)write_text_frame(fd, "{\"op\":11,\"d\":null}");
                }
            }
            break;
        default:
            return -1;
    }

    return 0;
}

static void *gateway_server_main(void *arg) {
    chaos_gateway_server_t *server = (chaos_gateway_server_t *)arg;
    for (unsigned i = 0; i < CHAOS_CONNECTIONS; ++i) {
        int client = accept_with_timeout(server->fd);
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

static void *chaos_monitor_main(void *arg) {
    chaos_state_t *state = (chaos_state_t *)arg;
    for (unsigned i = 0; i < 4500U; ++i) {
        if (atomic_load(&state->monitor_done) != 0U) {
            return NULL;
        }
        if (atomic_load(&state->bad) != 0U ||
            (state->server != NULL && atomic_load(&state->server->bad) != 0U)) {
            (void)dcc_client_stop(state->client);
            return NULL;
        }
        if (atomic_load(&state->ready_count) == 2U &&
            atomic_load(&state->resumed_count) == 2U &&
            atomic_load(&state->close_4000) == 1U &&
            atomic_load(&state->close_4009) == 1U) {
            (void)dcc_client_stop(state->client);
            return NULL;
        }
        (void)usleep(10000U);
    }
    atomic_store(&state->bad, 1U);
    (void)dcc_client_stop(state->client);
    return NULL;
}

static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    chaos_state_t *state = (chaos_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready == NULL || ready->shard_id != 0 || ready->shard_count != 1 || ready->session_id == NULL) {
        atomic_store(&state->bad, 1U);
        return;
    }

    unsigned count = atomic_fetch_add(&state->ready_count, 1U) + 1U;
    if (count > 2U) {
        atomic_store(&state->bad, 1U);
    }
}

static void on_resumed(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    chaos_state_t *state = (chaos_state_t *)user_data;
    if (dcc_event_type(event) != DCC_EVENT_RESUMED) {
        atomic_store(&state->bad, 1U);
        return;
    }
    atomic_fetch_add(&state->resumed_count, 1U);
}

static void on_socket_close(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    chaos_state_t *state = (chaos_state_t *)user_data;
    const dcc_socket_close_event_t *close_event = dcc_event_socket_close(event);
    if (close_event == NULL) {
        atomic_store(&state->bad, 1U);
        return;
    }
    if (close_event->code == 4000) {
        atomic_fetch_add(&state->close_4000, 1U);
    } else if (close_event->code == 4009) {
        atomic_fetch_add(&state->close_4009, 1U);
    }
}

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);

    chaos_gateway_server_t server;
    memset(&server, 0, sizeof(server));
    atomic_init(&server.accepts, 0U);
    atomic_init(&server.identifies, 0U);
    atomic_init(&server.resumes, 0U);
    atomic_init(&server.bad, 0U);
    if (start_listener(&server.fd, &server.port) != 0) {
        fprintf(stderr, "failed to start chaos gateway server\n");
        return 1;
    }

    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, gateway_server_main, &server) != 0) {
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
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        return 1;
    }

    chaos_state_t state;
    memset(&state, 0, sizeof(state));
    state.client = client;
    state.server = &server;
    atomic_init(&state.ready_count, 0U);
    atomic_init(&state.resumed_count, 0U);
    atomic_init(&state.close_4000, 0U);
    atomic_init(&state.close_4009, 0U);
    atomic_init(&state.monitor_done, 0U);
    atomic_init(&state.bad, 0U);

    pthread_t monitor_thread;
    int monitor_started = pthread_create(&monitor_thread, NULL, chaos_monitor_main, &state) == 0;

    if (status == DCC_OK) {
        status = dcc_client_on(client, DCC_EVENT_READY, on_ready, &state, NULL);
    }
    if (status == DCC_OK) {
        status = dcc_client_on(client, DCC_EVENT_RESUMED, on_resumed, &state, NULL);
    }
    if (status == DCC_OK) {
        status = dcc_client_on(client, DCC_EVENT_SOCKET_CLOSE, on_socket_close, &state, NULL);
    }
    if (status == DCC_OK) {
        status = dcc_client_start(client);
    }
    if (status == DCC_OK) {
        status = dcc_client_wait(client);
    }

    atomic_store(&state.monitor_done, 1U);
    if (monitor_started) {
        (void)pthread_join(monitor_thread, NULL);
    }
    (void)pthread_join(server_thread, NULL);
    close(server.fd);

    int ok = status == DCC_OK &&
        atomic_load(&state.bad) == 0U &&
        atomic_load(&state.ready_count) == 2U &&
        atomic_load(&state.resumed_count) == 2U &&
        atomic_load(&state.close_4000) == 1U &&
        atomic_load(&state.close_4009) == 1U &&
        atomic_load(&server.bad) == 0U &&
        atomic_load(&server.accepts) == CHAOS_CONNECTIONS &&
        atomic_load(&server.identifies) == 2U &&
        atomic_load(&server.resumes) == 2U;

    if (!ok) {
        fprintf(
            stderr,
            "gateway chaos smoke failed: status=%s bad=%u ready=%u resumed=%u "
            "close4000=%u close4009=%u server_bad=%u accepts=%u identifies=%u resumes=%u last=%s\n",
            dcc_status_string(status),
            atomic_load(&state.bad),
            atomic_load(&state.ready_count),
            atomic_load(&state.resumed_count),
            atomic_load(&state.close_4000),
            atomic_load(&state.close_4009),
            atomic_load(&server.bad),
            atomic_load(&server.accepts),
            atomic_load(&server.identifies),
            atomic_load(&server.resumes),
            dcc_client_last_error(client)
        );
        dcc_client_destroy(client);
        return 1;
    }

    dcc_client_destroy(client);
    return 0;
}

#endif
