#include "internal/hot_reload/dcc_hot_reload_worker_protocol_internal.h"

#if defined(_WIN32)
#include <errno.h>
#include <io.h>
#include <limits.h>
#include <windows.h>

int dcc_hot_reload_worker_write_all(int fd, const void *data, size_t len) {
    const unsigned char *cursor = (const unsigned char *)data;
    while (len > 0U) {
        unsigned chunk = len > (size_t)INT_MAX ? (unsigned)INT_MAX : (unsigned)len;
        int n = _write(fd, cursor, chunk);
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
    (void)timeout_ms;
    return dcc_hot_reload_worker_write_all(fd, data, len);
}

int dcc_hot_reload_worker_read_all(int fd, void *data, size_t len) {
    unsigned char *cursor = (unsigned char *)data;
    while (len > 0U) {
        unsigned chunk = len > (size_t)INT_MAX ? (unsigned)INT_MAX : (unsigned)len;
        int n = _read(fd, cursor, chunk);
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
    intptr_t raw = _get_osfhandle(fd);
    if (raw == -1) {
        errno = EBADF;
        return -1;
    }
    HANDLE handle = (HANDLE)raw;
    unsigned char *cursor = (unsigned char *)data;
    ULONGLONG start = GetTickCount64();
    while (len > 0U) {
        DWORD available = 0;
        if (!PeekNamedPipe(handle, NULL, 0, NULL, &available, NULL)) {
            errno = ECONNRESET;
            return -1;
        }
        if (available == 0U) {
            if ((uint32_t)(GetTickCount64() - start) >= timeout_ms) {
                errno = ETIMEDOUT;
                return -1;
            }
            Sleep(1);
            continue;
        }
        unsigned chunk = len > (size_t)INT_MAX ? (unsigned)INT_MAX : (unsigned)len;
        int n = _read(fd, cursor, chunk);
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
