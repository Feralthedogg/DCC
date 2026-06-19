#include "internal/interactions/dcc_interaction_server_internal.h"

#include <errno.h>
#include <stdio.h>

static const char *dcc_interaction_http_reason(uint16_t status) {
    switch (status) {
        case 200:
            return "OK";
        case 204:
            return "No Content";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 413:
            return "Payload Too Large";
        case 500:
            return "Internal Server Error";
        default:
            return "OK";
    }
}

static int dcc_interaction_write_all(llam_fd_t fd, const void *data, size_t len) {
    const char *cursor = (const char *)data;
    size_t remaining = len;
    while (remaining > 0U) {
        ssize_t written = llam_write(fd, cursor, remaining);
        if (written < 0 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) {
            continue;
        }
        if (written <= 0) {
            return -1;
        }
        cursor += (size_t)written;
        remaining -= (size_t)written;
    }
    return 0;
}

int dcc_interaction_write_response(llam_fd_t fd, const dcc_interaction_request_t *request) {
    const char *content_type = request->response_content_type != NULL
        ? request->response_content_type
        : "text/plain; charset=utf-8";
    const char *body = request->response_body != NULL ? request->response_body : "";
    size_t body_len = request->response_body != NULL ? request->response_body_len : 0U;
    uint16_t status = request->response_set ? request->response_status : 500U;
    char header[512];
    int header_len = snprintf(
        header,
        sizeof(header),
        "HTTP/1.1 %u %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        (unsigned)status,
        dcc_interaction_http_reason(status),
        content_type,
        body_len
    );
    if (header_len < 0 || (size_t)header_len >= sizeof(header)) {
        return -1;
    }
    if (dcc_interaction_write_all(fd, header, (size_t)header_len) != 0) {
        return -1;
    }
    if (body_len > 0U && dcc_interaction_write_all(fd, body, body_len) != 0) {
        return -1;
    }
    return 0;
}
