#include "internal/interactions/dcc_interaction_server_request_internal.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_interaction_read_request(
    dcc_interaction_server_t *server,
    llam_fd_t fd,
    dcc_interaction_request_t *request
) {
    dcc_interaction_buffer_t buffer;
    char chunk[DCC_INTERACTION_READ_CHUNK];
    size_t header_len = 0;
    size_t content_len = 0;
    memset(&buffer, 0, sizeof(buffer));

    for (;;) {
        header_len = dcc_interaction_find_header_end(buffer.data, buffer.len);
        if (header_len != 0U) {
            dcc_status_t st = dcc_interaction_parse_content_length(buffer.data, header_len, &content_len);
            if (st != DCC_OK) {
                free(buffer.data);
                return st;
            }
            if (content_len > server->max_body_size) {
                free(buffer.data);
                return DCC_ERR_INVALID_ARG;
            }
            if (buffer.len - header_len >= content_len) {
                break;
            }
        } else if (buffer.len > server->max_header_size) {
            free(buffer.data);
            return DCC_ERR_INVALID_ARG;
        }

        ssize_t nread = llam_read_when_ready(fd, chunk, sizeof(chunk), DCC_INTERACTION_READ_TIMEOUT_MS);
        if (nread > 0) {
            dcc_status_t st = dcc_interaction_buffer_append(&buffer, chunk, (size_t)nread);
            if (st != DCC_OK) {
                free(buffer.data);
                return st;
            }
            continue;
        }
        if (nread < 0 && (errno == EINTR || errno == ETIMEDOUT || errno == EAGAIN || errno == EWOULDBLOCK)) {
            continue;
        }
        free(buffer.data);
        return DCC_ERR_NETWORK;
    }

    if (header_len > server->max_header_size) {
        free(buffer.data);
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t st = dcc_interaction_parse_headers(request, buffer.data, header_len);
    if (st != DCC_OK) {
        free(buffer.data);
        return st;
    }

    if (content_len == SIZE_MAX) {
        free(buffer.data);
        return DCC_ERR_NOMEM;
    }
    request->body = (char *)malloc(content_len + 1U);
    if (request->body == NULL) {
        free(buffer.data);
        return DCC_ERR_NOMEM;
    }
    if (content_len > 0U) {
        memcpy(request->body, buffer.data + header_len, content_len);
    }
    request->body[content_len] = '\0';
    request->body_len = content_len;

    free(buffer.data);
    return DCC_OK;
}
