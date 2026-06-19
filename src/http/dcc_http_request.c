#include "internal/http/dcc_http.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int dcc_http_header_name_equal(const char *lhs, const char *rhs) {
    while (*lhs != '\0' && *rhs != '\0') {
        if (tolower((unsigned char)*lhs) != tolower((unsigned char)*rhs)) {
            return 0;
        }
        lhs++;
        rhs++;
    }
    return *lhs == '\0' && *rhs == '\0';
}

static int dcc_http_request_has_header(const dcc_http_request_t *request, const char *name) {
    for (size_t i = 0; i < request->header_count; ++i) {
        if (request->headers[i].name != NULL && dcc_http_header_name_equal(request->headers[i].name, name)) {
            return 1;
        }
    }
    return 0;
}

dcc_status_t dcc_http_build_request(
    const dcc_http_request_t *request,
    const dcc_http_url_parts_t *url,
    dcc_http_buffer_t *out
) {
    char content_length[64];
    const char *method = request->method != NULL ? request->method : "GET";

    if (dcc_http_buffer_append_cstr(out, method) != 0 ||
        dcc_http_buffer_append_cstr(out, " ") != 0 ||
        dcc_http_buffer_append_cstr(out, url->path) != 0 ||
        dcc_http_buffer_append_cstr(out, " HTTP/1.1\r\n") != 0 ||
        dcc_http_buffer_append_cstr(out, "Host: ") != 0 ||
        dcc_http_buffer_append_cstr(out, url->host) != 0 ||
        dcc_http_buffer_append_cstr(out, "\r\nUser-Agent: DCC/0.1\r\nConnection: close\r\n") != 0) {
        return DCC_ERR_NOMEM;
    }

    if (!dcc_http_request_has_header(request, "Content-Length")) {
        snprintf(content_length, sizeof(content_length), "Content-Length: %llu\r\n", (unsigned long long)request->body_len);
        if (dcc_http_buffer_append_cstr(out, content_length) != 0) {
            return DCC_ERR_NOMEM;
        }
    }

    for (size_t i = 0; i < request->header_count; ++i) {
        if (request->headers[i].name == NULL || request->headers[i].value == NULL) {
            continue;
        }
        if (dcc_http_buffer_append_cstr(out, request->headers[i].name) != 0 ||
            dcc_http_buffer_append_cstr(out, ": ") != 0 ||
            dcc_http_buffer_append_cstr(out, request->headers[i].value) != 0 ||
            dcc_http_buffer_append_cstr(out, "\r\n") != 0) {
            return DCC_ERR_NOMEM;
        }
    }

    if (dcc_http_buffer_append_cstr(out, "\r\n") != 0) {
        return DCC_ERR_NOMEM;
    }
    if (request->body != NULL &&
        request->body_len > 0 &&
        dcc_http_buffer_append(out, request->body, request->body_len) != 0) {
        return DCC_ERR_NOMEM;
    }
    return DCC_OK;
}

int dcc_http_request_canceled(const dcc_http_request_t *request) {
    return request != NULL &&
        request->is_canceled != NULL &&
        request->is_canceled(request->cancel_user_data);
}

void dcc_http_track_fd(const dcc_http_request_t *request, llam_fd_t fd) {
    if (request != NULL && request->swap_fd != NULL) {
        (void)request->swap_fd(request->cancel_user_data, fd);
    }
}

void dcc_http_clear_tracked_fd(const dcc_http_request_t *request, dcc_conn_t *conn) {
    if (request == NULL || request->swap_fd == NULL || conn == NULL) {
        return;
    }
    llam_fd_t owned = request->swap_fd(request->cancel_user_data, LLAM_INVALID_FD);
    if (LLAM_FD_IS_INVALID(owned)) {
        conn->fd = LLAM_INVALID_FD;
    }
}
