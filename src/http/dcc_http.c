#include "internal/http/dcc_http.h"

#include <string.h>

dcc_status_t dcc_http_perform(const dcc_http_request_t *request, dcc_http_response_t *out) {
    if (request == NULL || out == NULL || request->url == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status;
    dcc_http_url_parts_t url;
    dcc_http_buffer_t wire_request = {0};
    dcc_http_buffer_t wire_response = {0};

    memset(out, 0, sizeof(*out));

    if (dcc_http_request_canceled(request)) {
        return DCC_ERR_CANCELED;
    }

    status = dcc_http_parse_url(request->url, &url);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_http_build_request(request, &url, &wire_request);
    if (status != DCC_OK) {
        dcc_http_url_parts_deinit(&url);
        return status;
    }

    if (dcc_http_request_canceled(request)) {
        dcc_http_buffer_deinit(&wire_request);
        dcc_http_url_parts_deinit(&url);
        return DCC_ERR_CANCELED;
    }

    status = dcc_http_exchange_wire(request, &url, &wire_request, &wire_response);
    if (status != DCC_OK) {
        dcc_http_buffer_deinit(&wire_response);
        dcc_http_buffer_deinit(&wire_request);
        dcc_http_url_parts_deinit(&url);
        return status;
    }

    status = dcc_http_parse_response(&wire_response, out);
    if (dcc_http_request_canceled(request)) {
        dcc_http_response_deinit(out);
        status = DCC_ERR_CANCELED;
    }

    dcc_http_buffer_deinit(&wire_response);
    dcc_http_buffer_deinit(&wire_request);
    dcc_http_url_parts_deinit(&url);
    return status;
}
