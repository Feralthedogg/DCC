#include "internal/hot_reload/dcc_hot_reload_internal.h"

#if !defined(_WIN32)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *dcc_worker_path_join(
    const char *dir,
    size_t dir_len,
    const char *name,
    size_t name_len
) {
    if (dir_len > SIZE_MAX - name_len - 2U) {
        return NULL;
    }
    char *path = (char *)malloc(dir_len + 1U + name_len + 1U);
    if (path == NULL) {
        return NULL;
    }
    memcpy(path, dir, dir_len);
    path[dir_len] = '/';
    memcpy(path + dir_len + 1U, name, name_len);
    path[dir_len + 1U + name_len] = '\0';
    return path;
}

char *dcc_hot_reload_worker_resolve_exec_path_posix(
    const char *worker_path,
    char *error,
    size_t error_size
) {
    if (worker_path == NULL || worker_path[0] == '\0') {
        snprintf(error, error_size, "%s", "hot reload worker path is empty");
        return NULL;
    }
    if (strchr(worker_path, '/') != NULL) {
        return dcc_strdup(worker_path);
    }

    const char *path_env = getenv("PATH");
    const char *cursor = path_env != NULL ? path_env : "";
    size_t name_len = strlen(worker_path);
    for (;;) {
        const char *colon = strchr(cursor, ':');
        size_t dir_len = colon != NULL ? (size_t)(colon - cursor) : strlen(cursor);
        const char *dir = cursor;
        if (dir_len == 0U) {
            dir = ".";
            dir_len = 1U;
        }

        char *candidate = dcc_worker_path_join(dir, dir_len, worker_path, name_len);
        if (candidate != NULL && access(candidate, X_OK) == 0) {
            return candidate;
        }
        free(candidate);
        if (colon == NULL) {
            break;
        }
        cursor = colon + 1;
    }

    snprintf(error, error_size, "hot reload worker executable not found in PATH: %s", worker_path);
    return NULL;
}
#endif
