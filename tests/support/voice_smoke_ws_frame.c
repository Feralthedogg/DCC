#include "voice_smoke_support.h"
#include "voice_smoke_ws_internal.h"

#if !defined(_WIN32)

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

int voice_ws_read_exact_fd(int fd, void *buf, size_t len) {
    unsigned char *cursor = (unsigned char *)buf;
    size_t remaining = len;
    while (remaining > 0) {
        ssize_t n = read(fd, cursor, remaining);
        if (n <= 0) {
            return -1;
        }
        cursor += (size_t)n;
        remaining -= (size_t)n;
    }
    return 0;
}

char *voice_ws_find_header(char *headers, const char *name) {
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

int voice_ws_accept_for_key(const char *key, char out[64]) {
    char material[128];
    unsigned char digest[SHA_DIGEST_LENGTH];
    int material_len = snprintf(material, sizeof(material), "%s%s", key, VOICE_WS_GUID);
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

int voice_ws_read_http_upgrade(int client, char *buf, size_t cap) {
    size_t used = 0;
    while (used + 1U < cap) {
        ssize_t n = read(client, buf + used, cap - used - 1U);
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

int voice_ws_read_client_text_frame(int client, char *out, size_t out_cap) {
    unsigned char header[2];
    if (voice_ws_read_exact_fd(client, header, sizeof(header)) != 0 ||
        (header[0] & 0x0fU) != 0x1U ||
        (header[1] & 0x80U) == 0) {
        return -1;
    }

    size_t len = header[1] & 0x7fU;
    if (len == 126U) {
        unsigned char ext[2];
        if (voice_ws_read_exact_fd(client, ext, sizeof(ext)) != 0) {
            return -1;
        }
        len = ((size_t)ext[0] << 8U) | (size_t)ext[1];
    } else if (len == 127U) {
        return -1;
    }
    if (len + 1U > out_cap) {
        return -1;
    }

    unsigned char mask[4];
    if (voice_ws_read_exact_fd(client, mask, sizeof(mask)) != 0) {
        return -1;
    }
    unsigned char payload[512];
    if (len > sizeof(payload) || voice_ws_read_exact_fd(client, payload, len) != 0) {
        return -1;
    }
    for (size_t i = 0; i < len; ++i) {
        out[i] = (char)(payload[i] ^ mask[i % 4U]);
    }
    out[len] = '\0';
    return 0;
}

int voice_ws_read_client_text_frame_timeout(int client, char *out, size_t out_cap, unsigned timeout_ms) {
    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(client, &read_set);

    struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = (time_t)(timeout_ms / 1000U);
    timeout.tv_usec = (suseconds_t)((timeout_ms % 1000U) * 1000U);

    int ready = select(client + 1, &read_set, NULL, NULL, &timeout);
    if (ready <= 0 || !FD_ISSET(client, &read_set)) {
        return -1;
    }
    return voice_ws_read_client_text_frame(client, out, out_cap);
}

int voice_ws_write_server_text_frame(int client, const char *text) {
    size_t len = strlen(text);
    unsigned char header[4];
    size_t header_len = 0;
    header[header_len++] = 0x81U;
    if (len < 126U) {
        header[header_len++] = (unsigned char)len;
    } else if (len <= UINT16_MAX) {
        header[header_len++] = 126U;
        header[header_len++] = (unsigned char)((len >> 8U) & 0xffU);
        header[header_len++] = (unsigned char)(len & 0xffU);
    } else {
        return -1;
    }
    if (write(client, header, header_len) != (ssize_t)header_len ||
        write(client, text, len) != (ssize_t)len) {
        return -1;
    }
    return 0;
}

int voice_ws_write_server_close_frame_reason(int client, uint16_t code, const char *reason) {
    size_t reason_len = reason != NULL ? strlen(reason) : 0;
    if (reason_len > 123U) {
        reason_len = 123U;
    }

    unsigned char frame[127];
    size_t payload_len = 2U + reason_len;
    frame[0] = 0x88U;
    frame[1] = (unsigned char)payload_len;
    frame[2] = (unsigned char)((code >> 8U) & 0xffU);
    frame[3] = (unsigned char)(code & 0xffU);
    if (reason_len > 0) {
        memcpy(frame + 4U, reason, reason_len);
    }
    return write(client, frame, payload_len + 2U) == (ssize_t)(payload_len + 2U) ? 0 : -1;
}

int voice_ws_write_server_close_frame(int client, uint16_t code) {
    return voice_ws_write_server_close_frame_reason(client, code, NULL);
}

int voice_ws_accept_upgrade(int server_fd) {
    int client = accept(server_fd, NULL, NULL);
    if (client < 0) {
        return -1;
    }

    char request[4096];
    if (voice_ws_read_http_upgrade(client, request, sizeof(request)) != 0) {
        close(client);
        return -1;
    }

    char *key = voice_ws_find_header(request, "Sec-WebSocket-Key");
    char accept_value[64];
    if (key == NULL || voice_ws_accept_for_key(key, accept_value) != 0) {
        close(client);
        return -1;
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
        return -1;
    }
    return client;
}

#endif
