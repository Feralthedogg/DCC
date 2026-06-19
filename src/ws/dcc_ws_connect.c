#include "internal/ws/dcc_ws.h"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_ws_connect(dcc_ws_t **out, const char *url, int timeout_ms) {
    if (out == NULL || url == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_ws_url_t parsed;
    dcc_status_t status = dcc_ws_parse_url(url, &parsed);
    if (status != DCC_OK) {
        return status;
    }

    dcc_ws_t *ws = (dcc_ws_t *)calloc(1, sizeof(*ws));
    if (ws == NULL) {
        dcc_ws_url_deinit(&parsed);
        return DCC_ERR_NOMEM;
    }
    atomic_init(&ws->ref_count, 1U);
    atomic_flag_clear(&ws->send_lock);
    ws->host = dcc_ws_strndup(parsed.host, strlen(parsed.host));
    ws->path = dcc_ws_strndup(parsed.path, strlen(parsed.path));
    if (ws->host == NULL || ws->path == NULL) {
        dcc_ws_destroy(ws);
        dcc_ws_url_deinit(&parsed);
        return DCC_ERR_NOMEM;
    }

    dcc_conn_options_t conn_options = {
        .host = parsed.host,
        .port = parsed.port,
        .use_tls = parsed.use_tls,
        .timeout_ms = timeout_ms,
    };
    status = dcc_conn_open(&ws->conn, &conn_options);
    if (status != DCC_OK) {
        dcc_ws_destroy(ws);
        dcc_ws_url_deinit(&parsed);
        return status;
    }

    char key[32];
    if (dcc_ws_generate_key(key) != 0) {
        dcc_ws_destroy(ws);
        dcc_ws_url_deinit(&parsed);
        return DCC_ERR_TLS;
    }

    char request[2048];
    int request_len = snprintf(
        request,
        sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "User-Agent: DCC/0.1\r\n"
        "\r\n",
        parsed.path,
        parsed.host,
        key
    );
    if (request_len <= 0 || (size_t)request_len >= sizeof(request)) {
        dcc_ws_destroy(ws);
        dcc_ws_url_deinit(&parsed);
        return DCC_ERR_NOMEM;
    }
    if (dcc_conn_write_all(&ws->conn, request, (size_t)request_len) != 0) {
        dcc_ws_destroy(ws);
        dcc_ws_url_deinit(&parsed);
        return DCC_ERR_NETWORK;
    }

    status = dcc_ws_read_handshake(ws, key);
    if (status != DCC_OK) {
        dcc_ws_destroy(ws);
        dcc_ws_url_deinit(&parsed);
        return status;
    }

    atomic_store_explicit(&ws->open, 1, memory_order_release);
    *out = ws;
    dcc_ws_url_deinit(&parsed);
    return DCC_OK;
}
