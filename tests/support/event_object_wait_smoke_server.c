#include "event_object_wait_smoke_support.h"

#if !defined(_WIN32)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

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

int start_listener(int *out_fd, unsigned short *out_port) {
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

static int serve_connection(object_wait_server_t *server, int fd) {
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
    int saw_identify = 0;
    for (unsigned i = 0; i < 4U; ++i) {
        int frame_status = read_text_frame(fd, frame, sizeof(frame));
        if (frame_status != 0) {
            return -1;
        }
        if (strstr(frame, "\"op\":1") != NULL) {
            (void)write_text_frame(fd, "{\"op\":11,\"d\":null}");
            continue;
        }
        if (strstr(frame, "\"op\":2") != NULL &&
            strstr(frame, "\"shard\":[0,1]") != NULL) {
            saw_identify = 1;
            break;
        }
    }
    if (!saw_identify) {
        return -1;
    }
    atomic_fetch_add(&server->identifies, 1U);

    if (write_text_frame(
        fd,
        "{\"t\":\"MESSAGE_CREATE\",\"s\":7,\"op\":0,\"d\":{"
        "\"id\":\"111\",\"channel_id\":\"222\",\"guild_id\":\"333\","
        "\"content\":\"waited object\",\"flags\":64,"
        "\"timestamp\":\"2026-06-16T00:00:00.000000+00:00\","
        "\"type\":0,\"author\":{\"id\":\"444\",\"username\":\"waiter\","
        "\"global_name\":\"Waiter\",\"discriminator\":\"0\",\"bot\":false}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        fd,
        "{\"t\":\"GUILD_MEMBER_ADD\",\"s\":8,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"user\":{\"id\":\"446\",\"username\":\"member\","
        "\"global_name\":\"Member\",\"discriminator\":\"0\",\"bot\":false}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        fd,
        "{\"t\":\"GUILD_ROLE_CREATE\",\"s\":9,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"role\":{\"id\":\"555\",\"name\":\"mods\","
        "\"permissions\":\"1024\",\"color\":16711680,\"position\":4,"
        "\"hoist\":true,\"managed\":false,\"mentionable\":true}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        fd,
        "{\"t\":\"USER_UPDATE\",\"s\":10,\"op\":0,\"d\":{"
        "\"id\":\"447\",\"username\":\"self\",\"global_name\":null,"
        "\"avatar\":\"user-avatar\",\"discriminator\":\"0\",\"bot\":false,"
        "\"public_flags\":64}}"
    ) != 0) {
        return -1;
    }

    while (read_text_frame(fd, frame, sizeof(frame)) == 0) {
        if (strstr(frame, "\"op\":1") != NULL) {
            (void)write_text_frame(fd, "{\"op\":11,\"d\":null}");
        }
    }
    return 0;
}

void *server_thread_main(void *arg) {
    object_wait_server_t *server = (object_wait_server_t *)arg;
    int client = accept(server->fd, NULL, NULL);
    if (client < 0) {
        atomic_store(&server->bad, 1U);
        return NULL;
    }
    atomic_fetch_add(&server->accepts, 1U);
    if (serve_connection(server, client) != 0) {
        atomic_store(&server->bad, 1U);
    }
    close(client);
    return NULL;
}

void *client_wait_thread_main(void *arg) {
    client_wait_state_t *state = (client_wait_state_t *)arg;
    state->status = dcc_client_wait(state->client);
    return NULL;
}

void *message_wait_thread_main(void *arg) {
    message_wait_state_t *state = (message_wait_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_message(
        state->client,
        DCC_EVENT_MESSAGE_CREATE,
        2000,
        &state->snapshot,
        &state->message
    );
    return NULL;
}

void *data_wait_thread_main(void *arg) {
    data_wait_state_t *state = (data_wait_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_gateway_data(
        state->client,
        DCC_EVENT_MESSAGE_CREATE,
        2000,
        &state->snapshot,
        &state->data
    );
    return NULL;
}

void *message_any_wait_thread_main(void *arg) {
    message_wait_state_t *state = (message_wait_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_message_any(
        state->client,
        state->types,
        state->type_count,
        2000,
        &state->snapshot,
        &state->message
    );
    return NULL;
}

void *data_any_wait_thread_main(void *arg) {
    data_wait_state_t *state = (data_wait_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_gateway_data_any(
        state->client,
        state->types,
        state->type_count,
        2000,
        &state->snapshot,
        &state->data
    );
    return NULL;
}

void *member_wait_thread_main(void *arg) {
    member_wait_state_t *state = (member_wait_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_member(
        state->client,
        DCC_EVENT_GUILD_MEMBER_ADD,
        2000,
        &state->snapshot,
        &state->member
    );
    return NULL;
}

void *role_wait_thread_main(void *arg) {
    role_wait_state_t *state = (role_wait_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_role(
        state->client,
        DCC_EVENT_GUILD_ROLE_CREATE,
        2000,
        &state->snapshot,
        &state->role
    );
    return NULL;
}

void *user_wait_thread_main(void *arg) {
    user_wait_state_t *state = (user_wait_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_user(
        state->client,
        DCC_EVENT_USER_UPDATE,
        2000,
        &state->snapshot,
        &state->user
    );
    return NULL;
}

void *user_any_wait_thread_main(void *arg) {
    user_wait_state_t *state = (user_wait_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_user_any(
        state->client,
        state->types,
        state->type_count,
        2000,
        &state->snapshot,
        &state->user
    );
    return NULL;
}

void *any_wait_thread_main(void *arg) {
    any_wait_state_t *state = (any_wait_state_t *)arg;
    state->snapshot.size = sizeof(state->snapshot);
    state->status = dcc_client_wait_for_event_any(
        state->client,
        state->types,
        state->type_count,
        2000,
        &state->snapshot
    );
    return NULL;
}

void *sequence_wait_thread_main(void *arg) {
    sequence_wait_state_t *state = (sequence_wait_state_t *)arg;
    for (size_t i = 0; i < state->type_count; ++i) {
        state->snapshots[i].size = sizeof(state->snapshots[i]);
    }
    state->status = dcc_client_wait_for_event_sequence(
        state->client,
        state->types,
        state->type_count,
        2000,
        state->snapshots,
        sizeof(state->snapshots) / sizeof(state->snapshots[0]),
        &state->completed
    );
    return NULL;
}

void *set_wait_thread_main(void *arg) {
    set_wait_state_t *state = (set_wait_state_t *)arg;
    for (size_t i = 0; i < state->type_count; ++i) {
        state->snapshots[i].size = sizeof(state->snapshots[i]);
    }
    state->status = dcc_client_wait_for_event_set(
        state->client,
        state->types,
        state->type_count,
        2000,
        state->snapshots,
        sizeof(state->snapshots) / sizeof(state->snapshots[0]),
        &state->completed
    );
    return NULL;
}

void *count_wait_thread_main(void *arg) {
    count_wait_state_t *state = (count_wait_state_t *)arg;
    for (size_t i = 0; i < state->target_count; ++i) {
        state->snapshots[i].size = sizeof(state->snapshots[i]);
    }
    state->status = dcc_client_wait_for_event_count(
        state->client,
        state->types,
        state->type_count,
        state->target_count,
        2000,
        state->snapshots,
        sizeof(state->snapshots) / sizeof(state->snapshots[0]),
        &state->completed
    );
    return NULL;
}

void *quorum_wait_thread_main(void *arg) {
    quorum_wait_state_t *state = (quorum_wait_state_t *)arg;
    for (size_t i = 0; i < state->target_count; ++i) {
        state->snapshots[i].size = sizeof(state->snapshots[i]);
    }
    state->status = dcc_client_wait_for_event_quorum(
        state->client,
        state->types,
        state->type_count,
        state->target_count,
        2000,
        state->snapshots,
        sizeof(state->snapshots) / sizeof(state->snapshots[0]),
        &state->completed
    );
    return NULL;
}

#endif
