#include "internal/dcc_platform_datagram.h"

#include <limits.h>

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
) {
#if defined(_WIN32)
    if (len > (size_t)INT_MAX) {
        len = (size_t)INT_MAX;
    }
    return (ssize_t)sendto((SOCKET)fd, (const char *)buf, (int)len, flags, dest_addr, (int)addrlen);
#else
    return sendto(fd, buf, len, flags, dest_addr, addrlen);
#endif
}

ssize_t dcc_platform_recvfrom(
    llam_fd_t fd,
    void *buf,
    size_t len,
    int flags,
    struct sockaddr *src_addr,
    socklen_t *addrlen
) {
#if defined(_WIN32)
    int win_len = addrlen != NULL ? (int)*addrlen : 0;
    int *win_len_ptr = addrlen != NULL ? &win_len : NULL;
    if (len > (size_t)INT_MAX) {
        len = (size_t)INT_MAX;
    }
    ssize_t result =
        (ssize_t)recvfrom((SOCKET)fd, (char *)buf, (int)len, flags, src_addr, win_len_ptr);
    if (addrlen != NULL && win_len >= 0) {
        *addrlen = (socklen_t)win_len;
    }
    return result;
#else
    return recvfrom(fd, buf, len, flags, src_addr, addrlen);
#endif
}
