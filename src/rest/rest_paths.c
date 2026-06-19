#include "internal/rest/dcc_rest_paths_internal.h"

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

dcc_status_t dcc_rest_format_path(char *out, size_t out_size, const char *fmt, ...) {
    if (out == NULL || out_size == 0 || fmt == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(out, out_size, fmt, args);
    va_end(args);

    if (written < 0 || (size_t)written >= out_size) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_rest_alloc_formatted_path(char **out, const char *fmt, ...) {
    if (out == NULL || fmt == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    va_list args;
    va_start(args, fmt);
    va_list copy;
    va_copy(copy, args);
    int needed = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);
    if (needed < 0) {
        va_end(args);
        return DCC_ERR_INVALID_ARG;
    }

    char *path = (char *)malloc((size_t)needed + 1U);
    if (path == NULL) {
        va_end(args);
        return DCC_ERR_NOMEM;
    }
    int written = vsnprintf(path, (size_t)needed + 1U, fmt, args);
    va_end(args);
    if (written < 0 || written != needed) {
        free(path);
        return DCC_ERR_INVALID_ARG;
    }

    *out = path;
    return DCC_OK;
}
