#include "internal/dcc_hot_reload_host.h"

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

int dcc_hot_reload_host_worker_executable_direct(const char *path) {
    if (path == NULL || path[0] == '\0') {
        return 0;
    }
#if defined(_WIN32)
    return _access(path, 0) == 0;
#else
    return access(path, X_OK) == 0;
#endif
}

#if defined(_WIN32)
static int dcc_hot_reload_host_has_extension(const char *path) {
    const char *name = path;
    const char *dot = NULL;
    if (path == NULL) {
        return 0;
    }
    while (*path != '\0') {
        if (dcc_hot_reload_host_is_path_separator(*path)) {
            name = path + 1;
            dot = NULL;
        } else if (*path == '.') {
            dot = path;
        }
        ++path;
    }
    return dot != NULL && dot > name && dot[1] != '\0';
}
#endif

int dcc_hot_reload_host_worker_found_in_directory(
    const char *directory,
    size_t directory_len,
    const char *worker_name
) {
#if defined(_WIN32)
    static const char *const suffixes[] = { "", ".exe", ".cmd", ".bat", ".com" };
    size_t i;
    size_t count = dcc_hot_reload_host_has_extension(worker_name) ? 1U : sizeof(suffixes) / sizeof(suffixes[0]);
    for (i = 0; i < count; ++i) {
        if (dcc_hot_reload_host_worker_candidate_exists(directory, directory_len, worker_name, suffixes[i])) {
            return 1;
        }
    }
    return 0;
#else
    return dcc_hot_reload_host_worker_candidate_exists(directory, directory_len, worker_name, "");
#endif
}
