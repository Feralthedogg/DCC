#include "internal/dcc_hot_reload_host.h"

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

int dcc_hot_reload_host_is_path_separator(char ch) {
    return ch == '/'
#if defined(_WIN32)
        || ch == '\\'
#endif
        ;
}

int dcc_hot_reload_host_has_path_separator(const char *path) {
    if (path == NULL) {
        return 0;
    }
    while (*path != '\0') {
        if (dcc_hot_reload_host_is_path_separator(*path)) {
            return 1;
        }
        ++path;
    }
    return 0;
}

int dcc_hot_reload_host_file_readable(const char *path) {
    if (path == NULL || path[0] == '\0') {
        return 0;
    }
#if defined(_WIN32)
    return _access(path, 4) == 0;
#else
    return access(path, R_OK) == 0;
#endif
}
