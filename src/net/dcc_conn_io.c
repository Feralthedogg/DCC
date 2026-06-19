#include "internal/net/dcc_conn.h"

#include <llam/io.h>
#include <openssl/ssl.h>

#include <errno.h>
#include <stdint.h>

int dcc_conn_write_all(dcc_conn_t *conn, const void *data, size_t len) {
    if (conn == NULL || data == NULL) {
        errno = EINVAL;
        return -1;
    }

    const unsigned char *cursor = (const unsigned char *)data;
    size_t remaining = len;
    while (remaining > 0) {
        ssize_t written = 0;
        if (!conn->use_tls) {
            written = llam_write(conn->fd, cursor, remaining);
        } else {
            SSL *ssl = (SSL *)conn->ssl;
            int rc = SSL_write(ssl, cursor, remaining > INT32_MAX ? INT32_MAX : (int)remaining);
            if (rc <= 0) {
                int err = SSL_get_error(ssl, rc);
                if (dcc_conn_wait_ssl(conn, err) == 0) {
                    continue;
                }
                return -1;
            }
            written = rc;
        }

        if (written <= 0) {
            return -1;
        }
        cursor += (size_t)written;
        remaining -= (size_t)written;
    }
    return 0;
}

ssize_t dcc_conn_read(dcc_conn_t *conn, void *buf, size_t len) {
    if (conn == NULL || buf == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (!conn->use_tls) {
        return llam_read(conn->fd, buf, len);
    }

    SSL *ssl = (SSL *)conn->ssl;
    for (;;) {
        int rc = SSL_read(ssl, buf, len > INT32_MAX ? INT32_MAX : (int)len);
        if (rc > 0) {
            return rc;
        }
        if (rc == 0) {
            return 0;
        }

        int err = SSL_get_error(ssl, rc);
        if (err == SSL_ERROR_ZERO_RETURN) {
            return 0;
        }
        if (dcc_conn_wait_ssl(conn, err) == 0) {
            continue;
        }
        return -1;
    }
}
