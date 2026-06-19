#include "gateway_smoke_ws.h"

#if !defined(_WIN32)

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

char *find_header(char *headers, const char *name) {
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

int ws_accept_for_key(const char *key, char out[64]) {
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

int read_http_upgrade(int client, char *buf, size_t cap) {
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

int write_text_frame(int client, const char *text) {
    size_t len = strlen(text);
    unsigned char header[10];
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
    if (write(client, header, header_len) != (ssize_t)header_len) {
        return -1;
    }
    if (write(client, text, len) != (ssize_t)len) {
        return -1;
    }
    return 0;
}

int read_client_frame(int client, unsigned char *opcode, char *out, size_t out_cap) {
    unsigned char header[2];
    if (read(client, header, sizeof(header)) != (ssize_t)sizeof(header)) {
        return -1;
    }
    if ((header[1] & 0x80U) == 0) {
        return -1;
    }
    size_t len = header[1] & 0x7fU;
    if (len == 126U) {
        unsigned char ext[2];
        if (read(client, ext, sizeof(ext)) != (ssize_t)sizeof(ext)) {
            return -1;
        }
        len = ((size_t)ext[0] << 8U) | (size_t)ext[1];
    } else if (len == 127U) {
        unsigned char ext[8];
        if (read(client, ext, sizeof(ext)) != (ssize_t)sizeof(ext)) {
            return -1;
        }
        len = 0;
        for (size_t i = 0; i < sizeof(ext); ++i) {
            len = (len << 8U) | (size_t)ext[i];
        }
    }
    if (len + 1 > out_cap) {
        return -1;
    }

    unsigned char mask[4];
    if (read(client, mask, sizeof(mask)) != (ssize_t)sizeof(mask)) {
        return -1;
    }

    unsigned char payload[2048];
    if (read(client, payload, len) != (ssize_t)len) {
        return -1;
    }
    for (size_t i = 0; i < len; ++i) {
        out[i] = (char)(payload[i] ^ mask[i % 4]);
    }
    out[len] = '\0';
    if (opcode != NULL) {
        *opcode = header[0] & 0x0fU;
    }
    return 0;
}

int read_text_frame(int client, char *out, size_t out_cap) {
    unsigned char opcode = 0;
    if (read_client_frame(client, &opcode, out, out_cap) != 0) {
        return -1;
    }
    return opcode == 0x1U ? 0 : -1;
}

int read_text_frame_timeout(int client, char *out, size_t out_cap, int timeout_ms) {
    struct pollfd pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.fd = client;
    pfd.events = POLLIN;
    int ready = poll(&pfd, 1, timeout_ms);
    if (ready <= 0 || (pfd.revents & POLLIN) == 0) {
        return -1;
    }
    return read_text_frame(client, out, out_cap);
}

int write_dispatch(int client, const char *name, unsigned seq, const char *payload) {
    char frame[4096];
    int len = snprintf(
        frame,
        sizeof(frame),
        "{\"t\":\"%s\",\"s\":%u,\"op\":0,\"d\":%s}",
        name,
        seq,
        payload
    );
    if (len <= 0 || (size_t)len >= sizeof(frame)) {
        return -1;
    }
    return write_text_frame(client, frame);
}

int write_close_frame(int client, uint16_t code, const char *reason) {
    const char *text = reason != NULL ? reason : "";
    size_t reason_len = strlen(text);
    if (reason_len > 123U) {
        reason_len = 123U;
    }

    unsigned char frame[128];
    size_t len = reason_len + 2U;
    frame[0] = 0x88U;
    frame[1] = (unsigned char)len;
    frame[2] = (unsigned char)((code >> 8U) & 0xffU);
    frame[3] = (unsigned char)(code & 0xffU);
    if (reason_len > 0) {
        memcpy(frame + 4, text, reason_len);
    }

    return write(client, frame, len + 2U) == (ssize_t)(len + 2U) ? 0 : -1;
}

void drain_client_close(int client) {
    char payload[2048];
    (void)shutdown(client, SHUT_WR);
    for (int i = 0; i < 8; ++i) {
        unsigned char opcode = 0;
        if (read_client_frame(client, &opcode, payload, sizeof(payload)) != 0) {
            break;
        }
        if (opcode == 0x8U) {
            break;
        }
    }
}

#endif
