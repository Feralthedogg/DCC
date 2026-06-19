#include "internal/net/dcc_conn.h"

#include <llam/io.h>
#include <openssl/ssl.h>

#ifndef POLLIN
#define POLLIN 0x0001
#endif

#ifndef POLLOUT
#define POLLOUT 0x0004
#endif

static void dcc_ssl_init_once(void) {
    static int initialized = 0;
    if (!initialized) {
        OPENSSL_init_ssl(0, NULL);
        initialized = 1;
    }
}

int dcc_conn_wait_ssl(dcc_conn_t *conn, int ssl_error) {
    short events = 0;
    if (ssl_error == SSL_ERROR_WANT_READ) {
        events = POLLIN;
    } else if (ssl_error == SSL_ERROR_WANT_WRITE) {
        events = POLLOUT;
    } else {
        return -1;
    }

    short revents = 0;
    if (llam_poll_fd(conn->fd, events, conn->timeout_ms, &revents) < 0) {
        return -1;
    }
    return (revents & events) != 0 ? 0 : -1;
}

dcc_status_t dcc_conn_tls_open(dcc_conn_t *conn, const dcc_conn_options_t *options) {
    dcc_ssl_init_once();

    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == NULL) {
        return DCC_ERR_TLS;
    }

    SSL *ssl = SSL_new(ctx);
    if (ssl == NULL) {
        SSL_CTX_free(ctx);
        return DCC_ERR_TLS;
    }

    (void)SSL_set_tlsext_host_name(ssl, options->host);
    SSL_set_fd(ssl, (int)conn->fd);

    for (;;) {
        int rc = SSL_connect(ssl);
        if (rc == 1) {
            conn->ssl_ctx = ctx;
            conn->ssl = ssl;
            return DCC_OK;
        }

        int err = SSL_get_error(ssl, rc);
        if (dcc_conn_wait_ssl(conn, err) != 0) {
            break;
        }
    }

    SSL_free(ssl);
    SSL_CTX_free(ctx);
    return DCC_ERR_TLS;
}
