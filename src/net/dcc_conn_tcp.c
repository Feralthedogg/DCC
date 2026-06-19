#include "internal/net/dcc_conn.h"
#include "internal/dcc_platform_resolve.h"

#include <llam/io.h>

#include <string.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

dcc_status_t dcc_conn_tcp_open(dcc_conn_t *conn, const dcc_conn_options_t *options) {
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;

    if (dcc_platform_getaddrinfo(options->host, options->port, &hints, &result) != DCC_OK) {
        return DCC_ERR_NETWORK;
    }

    dcc_status_t status = DCC_ERR_NETWORK;
    for (struct addrinfo *ai = result; ai != NULL; ai = ai->ai_next) {
        llam_fd_t fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (LLAM_FD_IS_INVALID(fd)) {
            continue;
        }
#if defined(SO_NOSIGPIPE)
        int no_sigpipe = 1;
        (void)setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe, sizeof(no_sigpipe));
#endif

        if (llam_connect(fd, ai->ai_addr, (socklen_t)ai->ai_addrlen) == 0) {
            conn->fd = fd;
            status = DCC_OK;
            break;
        }

        (void)llam_close(fd);
    }

    dcc_platform_freeaddrinfo(result);
    return status;
}
