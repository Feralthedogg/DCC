#include "internal/dcc_hot_reload_host.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static char *dcc_hot_reload_host_join_worker_path(
    const char *directory,
    size_t directory_len,
    const char *name,
    const char *suffix
) {
    const char *dir = directory;
    size_t name_len;
    size_t suffix_len;
    size_t separator_len;
    size_t total;
    char *path;
    char *cursor;

    if (name == NULL) {
        return NULL;
    }
    if (suffix == NULL) {
        suffix = "";
    }
    if (directory_len == 0U) {
        dir = ".";
        directory_len = 1U;
    }

    name_len = strlen(name);
    suffix_len = strlen(suffix);
    separator_len = dcc_hot_reload_host_is_path_separator(dir[directory_len - 1U]) ? 0U : 1U;

    if (directory_len > SIZE_MAX - separator_len ||
        directory_len + separator_len > SIZE_MAX - name_len ||
        directory_len + separator_len + name_len > SIZE_MAX - suffix_len ||
        directory_len + separator_len + name_len + suffix_len > SIZE_MAX - 1U) {
        return NULL;
    }

    total = directory_len + separator_len + name_len + suffix_len;
    path = (char *)malloc(total + 1U);
    if (path == NULL) {
        return NULL;
    }

    cursor = path;
    memcpy(cursor, dir, directory_len);
    cursor += directory_len;
    if (separator_len != 0U) {
#if defined(_WIN32)
        *cursor++ = '\\';
#else
        *cursor++ = '/';
#endif
    }
    memcpy(cursor, name, name_len);
    cursor += name_len;
    memcpy(cursor, suffix, suffix_len);
    cursor += suffix_len;
    *cursor = '\0';
    return path;
}

int dcc_hot_reload_host_worker_candidate_exists(
    const char *directory,
    size_t directory_len,
    const char *worker_name,
    const char *suffix
) {
    char *path = dcc_hot_reload_host_join_worker_path(directory, directory_len, worker_name, suffix);
    int found;
    if (path == NULL) {
        return 0;
    }
    found = dcc_hot_reload_host_worker_executable_direct(path);
    free(path);
    return found;
}
