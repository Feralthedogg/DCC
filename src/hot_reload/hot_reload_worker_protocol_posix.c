#include "internal/hot_reload/dcc_hot_reload_worker_protocol_internal.h"

#if !defined(_WIN32)
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
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

static uint64_t dcc_worker_now_ms(void) {
#if defined(CLOCK_MONOTONIC)
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return (uint64_t)ts.tv_sec * UINT64_C(1000) + (uint64_t)ts.tv_nsec / UINT64_C(1000000);
    }
#endif
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == 0) {
        return (uint64_t)tv.tv_sec * UINT64_C(1000) + (uint64_t)tv.tv_usec / UINT64_C(1000);
    }
    return 0U;
}

static int dcc_worker_remaining_timeout_ms(uint64_t start_ms, uint32_t timeout_ms) {
    uint64_t now_ms = dcc_worker_now_ms();
    uint64_t elapsed_ms = now_ms >= start_ms ? now_ms - start_ms : 0U;
    if (elapsed_ms >= (uint64_t)timeout_ms) {
        errno = ETIMEDOUT;
        return -1;
    }
    uint64_t remaining_ms = (uint64_t)timeout_ms - elapsed_ms;
    return remaining_ms > (uint64_t)INT_MAX ? INT_MAX : (int)remaining_ms;
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

int dcc_hot_reload_worker_write_all_timeout(int fd, const void *data, size_t len, uint32_t timeout_ms) {
    if (timeout_ms == 0U) {
        return dcc_hot_reload_worker_write_all(fd, data, len);
    }

    const unsigned char *cursor = (const unsigned char *)data;
    uint64_t start_ms = dcc_worker_now_ms();
    int flags = fcntl(fd, F_GETFL, 0);
    int restore_flags = 0;
    if (flags >= 0 && (flags & O_NONBLOCK) == 0) {
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) != 0) {
            return -1;
        }
        restore_flags = 1;
    }

    int rc = 0;
    while (len > 0U) {
        int remaining_ms = dcc_worker_remaining_timeout_ms(start_ms, timeout_ms);
        if (remaining_ms < 0) {
            rc = -1;
            break;
        }

        struct pollfd pfd = { .fd = fd, .events = POLLOUT };
        int poll_rc = poll(&pfd, 1, remaining_ms);
        if (poll_rc < 0) {
            if (errno == EINTR) {
                continue;
            }
            rc = -1;
            break;
        }
        if (poll_rc == 0) {
            errno = ETIMEDOUT;
            rc = -1;
            break;
        }
        if ((pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) != 0) {
            errno = ECONNRESET;
            rc = -1;
            break;
        }

        size_t chunk = len > 65536U ? 65536U : len;
        ssize_t n = dcc_worker_write_no_sigpipe(fd, cursor, chunk);
        if (n < 0) {
            if (errno == EINTR ||
                errno == EAGAIN
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
                || errno == EWOULDBLOCK
#endif
            ) {
                continue;
            }
            rc = -1;
            break;
        }
        if (n == 0) {
            errno = ECONNRESET;
            rc = -1;
            break;
        }
        cursor += (size_t)n;
        len -= (size_t)n;
    }

    int saved_errno = errno;
    if (restore_flags && fcntl(fd, F_SETFL, flags) != 0) {
        if (rc == 0) {
            return -1;
        }
        errno = saved_errno;
    }
    return rc;
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
    if (timeout_ms == 0U) {
        return dcc_hot_reload_worker_read_all(fd, data, len);
    }

    unsigned char *cursor = (unsigned char *)data;
    uint64_t start_ms = dcc_worker_now_ms();
    while (len > 0U) {
        int remaining_ms = dcc_worker_remaining_timeout_ms(start_ms, timeout_ms);
        if (remaining_ms < 0) {
            return -1;
        }
        struct pollfd pfd = { .fd = fd, .events = POLLIN };
        int rc = poll(&pfd, 1, remaining_ms);
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
        if ((pfd.revents & (POLLERR | POLLNVAL)) != 0) {
            errno = ECONNRESET;
            return -1;
        }
        if ((pfd.revents & POLLIN) == 0) {
            if ((pfd.revents & POLLHUP) != 0) {
                errno = ECONNRESET;
                return -1;
            }
            continue;
        }
        ssize_t n = read(fd, cursor, len);
        if (n <= 0) {
            if (n < 0 &&
                (errno == EINTR ||
                 errno == EAGAIN
#if defined(EWOULDBLOCK) && EWOULDBLOCK != EAGAIN
                 || errno == EWOULDBLOCK
#endif
                )) {
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
