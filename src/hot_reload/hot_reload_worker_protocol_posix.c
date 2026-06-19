#include "internal/hot_reload/dcc_hot_reload_worker_protocol_internal.h"

#if !defined(_WIN32)
#include <errno.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#if !defined(MSG_NOSIGNAL)
#define DCC_WORKER_MSG_NOSIGNAL 0
#else
#define DCC_WORKER_MSG_NOSIGNAL MSG_NOSIGNAL
#endif

static ssize_t dcc_worker_write_no_sigpipe(int fd, const void *data, size_t len) {
#if defined(MSG_NOSIGNAL)
    return send(fd, data, len, DCC_WORKER_MSG_NOSIGNAL);
#else
    return write(fd, data, len);
#endif
}

int dcc_hot_reload_worker_write_all(int fd, const void *data, size_t len) {
    const unsigned char *cursor = (const unsigned char *)data;
    while (len > 0U) {
        ssize_t n = dcc_worker_write_no_sigpipe(fd, cursor, len);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (n == 0) {
            return -1;
        }
        cursor += (size_t)n;
        len -= (size_t)n;
    }
    return 0;
}

int dcc_hot_reload_worker_read_all(int fd, void *data, size_t len) {
    unsigned char *cursor = (unsigned char *)data;
    while (len > 0U) {
        ssize_t n = read(fd, cursor, len);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (n == 0) {
            return -1;
        }
        cursor += (size_t)n;
        len -= (size_t)n;
    }
    return 0;
}

int dcc_hot_reload_worker_read_all_timeout(int fd, void *data, size_t len, uint32_t timeout_ms) {
    unsigned char *cursor = (unsigned char *)data;
    while (len > 0U) {
        struct pollfd pfd = { .fd = fd, .events = POLLIN };
        int rc = poll(&pfd, 1, (int)timeout_ms);
        if (rc < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        if (rc == 0) {
            errno = ETIMEDOUT;
            return -1;
        }
        if ((pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
            errno = ECONNRESET;
            return -1;
        }
        ssize_t n = read(fd, cursor, len);
        if (n <= 0) {
            if (n < 0 && errno == EINTR) {
                continue;
            }
            if (n == 0) {
                errno = ECONNRESET;
            }
            return -1;
        }
        cursor += (size_t)n;
        len -= (size_t)n;
    }
    return 0;
}
#endif
