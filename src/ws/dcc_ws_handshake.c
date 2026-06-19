#include "internal/ws/dcc_ws.h"
#include "internal/ws/dcc_ws_handshake_helpers_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_ws_read_handshake(dcc_ws_t *ws, const char *key) {
    dcc_ws_buffer_t response = {0};
    char chunk[512];
    char *header_end = NULL;
    dcc_status_t status = DCC_ERR_NETWORK;

    while (response.len < DCC_WS_MAX_HEADER_BYTES) {
        ssize_t n = dcc_conn_read(&ws->conn, chunk, sizeof(chunk));
        if (n <= 0) {
            goto done;
        }
        if (dcc_ws_buffer_append(&response, chunk, (size_t)n) != 0) {
            status = DCC_ERR_NOMEM;
            goto done;
        }
        header_end = dcc_ws_find_header_end(response.data, response.len);
        if (header_end != NULL) {
            break;
        }
    }

    if (header_end == NULL) {
        goto done;
    }

    size_t wire_len = response.len;
    if (dcc_ws_buffer_append(&response, "", 1) != 0) {
        status = DCC_ERR_NOMEM;
        goto done;
    }

    unsigned code = 0;
    if (sscanf((const char *)response.data, "HTTP/%*u.%*u %u", &code) != 1 || code != 101) {
        goto done;
    }

    char expected[64];
    if (dcc_ws_accept_for_key(key, expected) != 0) {
        status = DCC_ERR_TLS;
        goto done;
    }

    char *accept = dcc_ws_header_value((char *)response.data, "Sec-WebSocket-Accept");
    if (accept == NULL) {
        goto done;
    }
    char *accept_end = strstr(accept, "\r\n");
    if (accept_end != NULL) {
        *accept_end = '\0';
    }
    if (strcmp(accept, expected) != 0) {
        goto done;
    }

    size_t header_bytes = (size_t)((unsigned char *)header_end - response.data) + 4U;
    if (wire_len > header_bytes) {
        size_t leftover = wire_len - header_bytes;
        ws->rx_buf = (unsigned char *)malloc(leftover);
        if (ws->rx_buf == NULL) {
            status = DCC_ERR_NOMEM;
            goto done;
        }
        memcpy(ws->rx_buf, response.data + header_bytes, leftover);
        ws->rx_len = leftover;
        ws->rx_pos = 0;
    }

    status = DCC_OK;

done:
    dcc_ws_buffer_deinit(&response);
    return status;
}
