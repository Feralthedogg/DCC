#include "internal/net/dcc_conn.h"

#include <llam/io.h>
#include <openssl/ssl.h>

#include <string.h>

dcc_status_t dcc_conn_open(dcc_conn_t *conn, const dcc_conn_options_t *options) {
    if (conn == NULL || options == NULL || options->host == NULL || options->port == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    memset(conn, 0, sizeof(*conn));
    conn->fd = LLAM_INVALID_FD;
    conn->use_tls = options->use_tls;
    conn->timeout_ms = options->timeout_ms > 0 ? options->timeout_ms : 30000;

    dcc_status_t status = dcc_conn_tcp_open(conn, options);
    if (status != DCC_OK) {
        return status;
    }

    if (!conn->use_tls) {
        return DCC_OK;
    }

    status = dcc_conn_tls_open(conn, options);
    if (status != DCC_OK) {
        dcc_conn_close(conn);
    }
    return status;
}

void dcc_conn_close(dcc_conn_t *conn) {
    if (conn == NULL) {
        return;
    }

    if (conn->ssl != NULL) {
        SSL_free((SSL *)conn->ssl);
        conn->ssl = NULL;
    }
    if (conn->ssl_ctx != NULL) {
        SSL_CTX_free((SSL_CTX *)conn->ssl_ctx);
        conn->ssl_ctx = NULL;
    }
    if (conn->fd != LLAM_INVALID_FD) {
        (void)llam_close(conn->fd);
        conn->fd = LLAM_INVALID_FD;
    }
}
