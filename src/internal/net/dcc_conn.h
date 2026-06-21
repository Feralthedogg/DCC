#ifndef DCC_CONN_H
#define DCC_CONN_H

#include <dcc/error.h>
#include <llam/platform.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_conn {
    llam_fd_t fd;
    int use_tls;
    int timeout_ms;
    void *ssl_ctx;
    void *ssl;
} dcc_conn_t;

typedef struct dcc_conn_options {
    const char *host;
    const char *port;
    int use_tls;
    int timeout_ms;
} dcc_conn_options_t;

dcc_status_t dcc_conn_open(dcc_conn_t *conn, const dcc_conn_options_t *options);
dcc_status_t dcc_conn_tcp_open(dcc_conn_t *conn, const dcc_conn_options_t *options);
dcc_status_t dcc_conn_tls_open(dcc_conn_t *conn, const dcc_conn_options_t *options);
int dcc_conn_set_nonblocking(llam_fd_t fd);
int dcc_conn_wait_ssl(dcc_conn_t *conn, int ssl_error);
int dcc_conn_write_all(dcc_conn_t *conn, const void *data, size_t len);
ssize_t dcc_conn_read(dcc_conn_t *conn, void *buf, size_t len);
void dcc_conn_close(dcc_conn_t *conn);

#ifdef __cplusplus
}
#endif

#endif
