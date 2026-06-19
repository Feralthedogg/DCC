#ifndef DCC_REST_REQUEST_RAW_INTERNAL_H
#define DCC_REST_REQUEST_RAW_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"
#include "internal/http/dcc_http.h"

#include <dcc/rest.h>
#include <llam/io.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_rest_request_headers {
    dcc_http_header_t headers[3];
    size_t header_count;
    char *authorization;
} dcc_rest_request_headers_t;

typedef struct dcc_rest_request_response_limits {
    dcc_status_t response_error;
    double retry_after_seconds;
    uint8_t is_global;
} dcc_rest_request_response_limits_t;

typedef struct dcc_rest_prepared_request {
    char *url;
    dcc_rest_request_headers_t headers;
    dcc_http_request_t http;
} dcc_rest_prepared_request_t;

dcc_status_t dcc_rest_request_headers_init(
    dcc_rest_request_headers_t *headers,
    const dcc_client_t *client,
    int absolute_url,
    size_t body_len,
    const char *content_type
);
void dcc_rest_request_headers_deinit(dcc_rest_request_headers_t *headers);
dcc_status_t dcc_rest_prepare_http_request(
    dcc_rest_prepared_request_t *prepared,
    const dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    int (*is_canceled)(void *user_data),
    llam_fd_t (*swap_fd)(void *user_data, llam_fd_t fd),
    void *cancel_user_data
);
void dcc_rest_prepared_request_deinit(dcc_rest_prepared_request_t *prepared);

dcc_rest_request_response_limits_t dcc_rest_request_update_response_limits(
    dcc_client_t *client,
    const char *route,
    const dcc_http_response_t *http_response
);
void dcc_rest_request_emit_rate_limited(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const dcc_http_response_t *http_response,
    const dcc_rest_request_response_limits_t *limits
);

dcc_status_t dcc_rest_request_raw_impl(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    uint32_t max_rate_limit_retries,
    int wait_for_route,
    dcc_rest_cb cb,
    void *user_data,
    int (*is_canceled)(void *user_data),
    llam_fd_t (*swap_fd)(void *user_data, llam_fd_t fd),
    void *cancel_user_data
);

#ifdef __cplusplus
}
#endif

#endif
