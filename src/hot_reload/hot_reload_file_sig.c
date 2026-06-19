#include "internal/hot_reload/dcc_hot_reload_file_sig_internal.h"

#include <stddef.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

dcc_status_t dcc_hot_reload_file_sig(const char *path, dcc_hot_reload_file_sig_t *out) {
    if (path == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
#if defined(_WIN32)
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &data)) {
        return DCC_ERR_STATE;
    }
    ULARGE_INTEGER file_time;
    file_time.LowPart = data.ftLastWriteTime.dwLowDateTime;
    file_time.HighPart = data.ftLastWriteTime.dwHighDateTime;
    ULARGE_INTEGER file_size;
    file_size.LowPart = data.nFileSizeLow;
    file_size.HighPart = data.nFileSizeHigh;
    out->mtime_ns = file_time.QuadPart * 100U;
    out->size = file_size.QuadPart;
#else
    struct stat st;
    if (stat(path, &st) != 0) {
        return DCC_ERR_STATE;
    }
#if defined(__APPLE__)
    out->mtime_ns = (uint64_t)st.st_mtimespec.tv_sec * UINT64_C(1000000000) +
        (uint64_t)st.st_mtimespec.tv_nsec;
#elif defined(_BSD_SOURCE) || defined(_DEFAULT_SOURCE) || defined(_POSIX_C_SOURCE)
    out->mtime_ns = (uint64_t)st.st_mtim.tv_sec * UINT64_C(1000000000) +
        (uint64_t)st.st_mtim.tv_nsec;
#else
    out->mtime_ns = (uint64_t)st.st_mtime * UINT64_C(1000000000);
#endif
    out->size = (uint64_t)st.st_size;
#endif
    return DCC_OK;
}

int dcc_hot_reload_same_sig(
    const dcc_hot_reload_file_sig_t *left,
    const dcc_hot_reload_file_sig_t *right
) {
    return left != NULL && right != NULL && left->mtime_ns == right->mtime_ns && left->size == right->size;
}
