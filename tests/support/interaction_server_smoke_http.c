#include "interaction_server_smoke_support.h"

#if LLAM_PLATFORM_POSIX

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void interaction_hex_encode(const unsigned char *data, size_t len, char *out) {
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0; i < len; ++i) {
        out[i * 2U] = hex[data[i] >> 4U];
        out[i * 2U + 1U] = hex[data[i] & 0x0fU];
    }
    out[len * 2U] = '\0';
}

static int interaction_write_all(int fd, const char *data, size_t len) {
    while (len > 0U) {
        ssize_t written = write(fd, data, len);
        if (written < 0 && (errno == EINTR || errno == EAGAIN)) {
            continue;
        }
        if (written <= 0) {
            return -1;
        }
        data += (size_t)written;
        len -= (size_t)written;
    }
    return 0;
}

int interaction_sign(
    EVP_PKEY *key,
    const char *timestamp,
    const char *body,
    char signature_hex[129]
) {
    size_t timestamp_len = strlen(timestamp);
    size_t body_len = strlen(body);
    unsigned char *message = (unsigned char *)malloc(timestamp_len + body_len);
    unsigned char signature[64];
    size_t signature_len = sizeof(signature);
    EVP_MD_CTX *ctx = NULL;
    int ok = 0;
    if (message == NULL) {
        return 0;
    }
    memcpy(message, timestamp, timestamp_len);
    memcpy(message + timestamp_len, body, body_len);

    ctx = EVP_MD_CTX_new();
    if (ctx != NULL &&
        EVP_DigestSignInit(ctx, NULL, NULL, NULL, key) == 1 &&
        EVP_DigestSign(ctx, signature, &signature_len, message, timestamp_len + body_len) == 1 &&
        signature_len == sizeof(signature)) {
        interaction_hex_encode(signature, sizeof(signature), signature_hex);
        ok = 1;
    }
    EVP_MD_CTX_free(ctx);
    free(message);
    return ok;
}

static int interaction_connect(uint16_t port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }
#if defined(SO_NOSIGPIPE)
    int yes = 1;
    (void)setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(yes));
#endif
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) != 1 ||
        connect(fd, (const struct sockaddr *)&addr, (socklen_t)sizeof(addr)) != 0) {
        close(fd);
        return -1;
    }
    return fd;
}

static int interaction_read_response(int fd, interaction_http_response_t *out) {
    size_t len = 0;
    memset(out, 0, sizeof(*out));
    while (len + 1U < sizeof(out->raw)) {
        ssize_t nread = read(fd, out->raw + len, sizeof(out->raw) - len - 1U);
        if (nread < 0 && errno == EINTR) {
            continue;
        }
        if (nread <= 0) {
            break;
        }
        len += (size_t)nread;
    }
    out->raw[len] = '\0';
    if (sscanf(out->raw, "HTTP/1.1 %d", &out->status) != 1) {
        return 0;
    }
    char *body = strstr(out->raw, "\r\n\r\n");
    if (body == NULL) {
        return 0;
    }
    body += 4;
    size_t body_len = len - (size_t)(body - out->raw);
    if (body_len >= sizeof(out->body)) {
        return 0;
    }
    memcpy(out->body, body, body_len);
    out->body[body_len] = '\0';
    return 1;
}

int interaction_send_request(
    uint16_t port,
    const char *method,
    const char *path,
    const char *body,
    const char *timestamp,
    const char *signature_hex,
    interaction_http_response_t *out
) {
    int fd = interaction_connect(port);
    if (fd < 0) {
        perror("connect interaction server");
        return 0;
    }

    char request[4096];
    const char *body_value = body != NULL ? body : "";
    size_t body_len = strlen(body_value);
    int len = 0;
    if (signature_hex != NULL && timestamp != NULL) {
        len = snprintf(
            request,
            sizeof(request),
            "%s %s HTTP/1.1\r\n"
            "Host: 127.0.0.1:%u\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n"
            "X-Signature-Ed25519: %s\r\n"
            "X-Signature-Timestamp: %s\r\n"
            "X-Test: yes\r\n"
            "\r\n"
            "%s",
            method,
            path,
            (unsigned)port,
            body_len,
            signature_hex,
            timestamp,
            body_value
        );
    } else {
        len = snprintf(
            request,
            sizeof(request),
            "%s %s HTTP/1.1\r\n"
            "Host: 127.0.0.1:%u\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %zu\r\n"
            "X-Test: yes\r\n"
            "\r\n"
            "%s",
            method,
            path,
            (unsigned)port,
            body_len,
            body_value
        );
    }
    if (len < 0 || (size_t)len >= sizeof(request) ||
        interaction_write_all(fd, request, (size_t)len) != 0 ||
        !interaction_read_response(fd, out)) {
        close(fd);
        return 0;
    }
    close(fd);
    return 1;
}

int interaction_send_raw_request(
    uint16_t port,
    const char *request,
    interaction_http_response_t *out
) {
    int fd = interaction_connect(port);
    if (fd < 0) {
        perror("connect interaction server");
        return 0;
    }
    size_t request_len = strlen(request);
    if (interaction_write_all(fd, request, request_len) != 0 ||
        !interaction_read_response(fd, out)) {
        close(fd);
        return 0;
    }
    close(fd);
    return 1;
}

int interaction_send_oversized_header(uint16_t port, interaction_http_response_t *out) {
    char bloat[1500];
    char request[2048];
    memset(bloat, 'a', sizeof(bloat) - 1U);
    bloat[sizeof(bloat) - 1U] = '\0';
    int len = snprintf(
        request,
        sizeof(request),
        "GET /too-large HTTP/1.1\r\n"
        "Host: 127.0.0.1:%u\r\n"
        "X-Bloat: %s\r\n"
        "Content-Length: 0\r\n"
        "\r\n",
        (unsigned)port,
        bloat
    );
    if (len < 0 || (size_t)len >= sizeof(request)) {
        return 0;
    }
    return interaction_send_raw_request(port, request, out);
}

int interaction_send_interaction_request(
    uint16_t port,
    const char *body,
    const char *timestamp,
    const char *signature_hex,
    interaction_http_response_t *out
) {
    return interaction_send_request(port, "POST", "/interactions", body, timestamp, signature_hex, out);
}

#endif
