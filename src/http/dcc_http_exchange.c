#include "internal/http/dcc_http.h"

#include <string.h>

dcc_status_t dcc_http_exchange_wire(
    const dcc_http_request_t *request,
    const dcc_http_url_parts_t *url,
    const dcc_http_buffer_t *wire_request,
    dcc_http_buffer_t *wire_response
) {
    dcc_conn_t conn;
    memset(&conn, 0, sizeof(conn));

    dcc_conn_options_t conn_options = {
        .host = url->host,
        .port = url->port,
        .use_tls = url->use_tls,
        .timeout_ms = request->timeout_ms,
    };
    dcc_status_t status = dcc_conn_open(&conn, &conn_options);
    if (status != DCC_OK) {
        return status;
    }
    dcc_http_track_fd(request, conn.fd);

    if (dcc_http_request_canceled(request)) {
        dcc_http_clear_tracked_fd(request, &conn);
        dcc_conn_close(&conn);
        return DCC_ERR_CANCELED;
    }

    if (dcc_conn_write_all(&conn, wire_request->data, wire_request->len) != 0) {
        status = dcc_http_request_canceled(request) ? DCC_ERR_CANCELED : DCC_ERR_NETWORK;
        dcc_http_clear_tracked_fd(request, &conn);
        dcc_conn_close(&conn);
        return status;
    }

    char chunk[8192];
    for (;;) {
        if (dcc_http_request_canceled(request)) {
            dcc_http_clear_tracked_fd(request, &conn);
            dcc_conn_close(&conn);
            return DCC_ERR_CANCELED;
        }
        ssize_t n = dcc_conn_read(&conn, chunk, sizeof(chunk));
        if (n == 0) {
            break;
        }
        if (n < 0) {
            status = dcc_http_request_canceled(request) ? DCC_ERR_CANCELED : DCC_ERR_NETWORK;
            dcc_http_clear_tracked_fd(request, &conn);
            dcc_conn_close(&conn);
            return status;
        }
        if (dcc_http_buffer_append(wire_response, chunk, (size_t)n) != 0) {
            dcc_http_clear_tracked_fd(request, &conn);
            dcc_conn_close(&conn);
            return DCC_ERR_NOMEM;
        }
    }

    dcc_http_clear_tracked_fd(request, &conn);
    dcc_conn_close(&conn);
    return DCC_OK;
}
