#include "internal/dcc_hot_reload_host.h"

#include <stdlib.h>

static int dcc_hot_reload_host_worker_found_on_path(const char *worker_name) {
    const char *path;
    const char *cursor;
    if (worker_name == NULL || worker_name[0] == '\0') {
        return 0;
    }
    path = getenv("PATH");
    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    cursor = path;
    for (;;) {
        const char *end = cursor;
        while (*end != '\0' &&
#if defined(_WIN32)
               *end != ';'
#else
               *end != ':'
#endif
        ) {
            ++end;
        }
        if (dcc_hot_reload_host_worker_found_in_directory(cursor, (size_t)(end - cursor), worker_name)) {
            return 1;
        }
        if (*end == '\0') {
            break;
        }
        cursor = end + 1;
    }
    return 0;
}

int dcc_hot_reload_host_worker_available(const char *worker_path) {
    if (worker_path == NULL || worker_path[0] == '\0') {
        return 0;
    }
    if (dcc_hot_reload_host_has_path_separator(worker_path)) {
        return dcc_hot_reload_host_worker_executable_direct(worker_path);
    }
    return dcc_hot_reload_host_worker_found_on_path(worker_path);
}
