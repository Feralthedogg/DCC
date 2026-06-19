#ifndef DCC_HTTP_REQUEST_INTERNAL_H
#define DCC_HTTP_REQUEST_INTERNAL_H

#include "internal/http/dcc_http_buffer_internal.h"
#include "internal/http/dcc_http_url_internal.h"
#include "internal/net/dcc_conn.h"

#include <dcc/error.h>

#include <llam/platform.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_http_header {
    const char *name;
    const char *value;
} dcc_http_header_t;

typedef struct dcc_http_request {
    const char *method;
    const char *url;
    const dcc_http_header_t *headers;
    size_t header_count;
    const void *body;
    size_t body_len;
    int timeout_ms;
    int (*is_canceled)(void *user_data);
    llam_fd_t (*swap_fd)(void *user_data, llam_fd_t fd);
    void *cancel_user_data;
} dcc_http_request_t;

dcc_status_t dcc_http_build_request(
    const dcc_http_request_t *request,
    const dcc_http_url_parts_t *url,
    dcc_http_buffer_t *out
);
int dcc_http_request_canceled(const dcc_http_request_t *request);
void dcc_http_track_fd(const dcc_http_request_t *request, llam_fd_t fd);
void dcc_http_clear_tracked_fd(const dcc_http_request_t *request, dcc_conn_t *conn);

#ifdef __cplusplus
}
#endif

#endif
