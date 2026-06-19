#include "cluster_restart_suppress_smoke_support.h"

#if !defined(_WIN32)

#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdatomic.h>
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

int suppress_start_listener(int *out_fd, unsigned short *out_port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }
    int yes = 1;
    (void)setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct timeval timeout;
    timeout.tv_sec = 8;
    timeout.tv_usec = 0;
    (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

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

static int serve_connection(suppress_gateway_server_t *server, int fd) {
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
        if (read_text_frame(fd, frame, sizeof(frame)) != 0) {
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

    return write_text_frame(fd, "{\"t\":\"READY\",\"s\":1,\"op\":0,\"d\":{\"guilds\":[]}}");
}

void *suppress_gateway_server_main(void *arg) {
    suppress_gateway_server_t *server = (suppress_gateway_server_t *)arg;
    for (unsigned i = 0; i < 2U; ++i) {
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
    }
    return NULL;
}

#endif
