#ifndef DCC_PLATFORM_DATAGRAM_H
#define DCC_PLATFORM_DATAGRAM_H

#include <llam/io.h>

#include <stddef.h>

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

ssize_t dcc_platform_sendto(
    llam_fd_t fd,
    const void *buf,
    size_t len,
    int flags,
    const struct sockaddr *dest_addr,
    socklen_t addrlen
);
ssize_t dcc_platform_recvfrom(
    llam_fd_t fd,
    void *buf,
    size_t len,
    int flags,
    struct sockaddr *src_addr,
    socklen_t *addrlen
);

#endif
