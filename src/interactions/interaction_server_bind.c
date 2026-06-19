#include "internal/interactions/dcc_interaction_server_internal.h"

#include <llam/io.h>

#include <stdio.h>
#include <string.h>

#if LLAM_PLATFORM_POSIX
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

dcc_status_t dcc_interaction_bind_listener(dcc_interaction_server_t *server, llam_fd_t *out_fd) {
#if LLAM_PLATFORM_POSIX
    char port_text[16];
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    int gai_error = 0;
    dcc_status_t status = DCC_ERR_NETWORK;
    snprintf(port_text, sizeof(port_text), "%u", (unsigned)server->requested_port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (llam_getaddrinfo_result(server->address, port_text, &hints, &result, &gai_error) != 0 ||
        gai_error != 0 || result == NULL) {
        llam_freeaddrinfo_result(result);
        return DCC_ERR_NETWORK;
    }

    for (struct addrinfo *ai = result; ai != NULL; ai = ai->ai_next) {
        llam_fd_t fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (LLAM_FD_IS_INVALID(fd)) {
            continue;
        }
        int yes = 1;
        (void)setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
#if defined(SO_NOSIGPIPE)
        (void)setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(yes));
#endif

        if (bind(fd, ai->ai_addr, (socklen_t)ai->ai_addrlen) == 0 &&
            listen(fd, (int)server->backlog) == 0) {
            struct sockaddr_storage bound;
            socklen_t bound_len = (socklen_t)sizeof(bound);
            if (getsockname(fd, (struct sockaddr *)&bound, &bound_len) == 0) {
                if (bound.ss_family == AF_INET) {
                    const struct sockaddr_in *addr = (const struct sockaddr_in *)&bound;
                    server->bound_port = ntohs(addr->sin_port);
                } else if (bound.ss_family == AF_INET6) {
                    const struct sockaddr_in6 *addr6 = (const struct sockaddr_in6 *)&bound;
                    server->bound_port = ntohs(addr6->sin6_port);
                } else {
                    server->bound_port = server->requested_port;
                }
            } else {
                server->bound_port = server->requested_port;
            }
            *out_fd = fd;
            status = DCC_OK;
            break;
        }
        (void)llam_close(fd);
    }

    llam_freeaddrinfo_result(result);
    return status;
#else
    (void)server;
    (void)out_fd;
    return DCC_ERR_NETWORK;
#endif
}
