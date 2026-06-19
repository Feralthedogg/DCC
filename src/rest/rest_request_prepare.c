#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_request_url_internal.h"

#include <stdlib.h>
#include <string.h>

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
) {
    if (prepared == NULL || client == NULL || method == NULL || path == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    memset(prepared, 0, sizeof(*prepared));
    int absolute_url = 0;
    dcc_status_t url_status = dcc_rest_build_url(path, &prepared->url, &absolute_url);
    if (url_status != DCC_OK) {
        return url_status;
    }

    dcc_status_t header_status = dcc_rest_request_headers_init(
        &prepared->headers,
        client,
        absolute_url,
        body_len,
        content_type
    );
    if (header_status != DCC_OK) {
        free(prepared->url);
        prepared->url = NULL;
        return header_status;
    }

    prepared->http = (dcc_http_request_t){
        .method = method,
        .url = prepared->url,
        .headers = prepared->headers.headers,
        .header_count = prepared->headers.header_count,
        .body = body,
        .body_len = body_len,
        .timeout_ms = 30000,
        .is_canceled = is_canceled,
        .swap_fd = swap_fd,
        .cancel_user_data = cancel_user_data,
    };
    return DCC_OK;
}

void dcc_rest_prepared_request_deinit(dcc_rest_prepared_request_t *prepared) {
    if (prepared == NULL) {
        return;
    }

    dcc_rest_request_headers_deinit(&prepared->headers);
    free(prepared->url);
    prepared->url = NULL;
    prepared->http.url = NULL;
    prepared->http.headers = NULL;
    prepared->http.header_count = 0;
}
