#include "internal/command_registry/dcc_command_registry_internal.h"

#include <stdarg.h>
#include <stdio.h>

dcc_status_t dcc_command_registry_format_append(
    char *buf,
    size_t buf_len,
    size_t *used,
    const char *fmt,
    ...
) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(
        buf != NULL && *used < buf_len ? buf + *used : NULL,
        buf != NULL && *used < buf_len ? buf_len - *used : 0,
        fmt,
        ap
    );
    va_end(ap);
    if (n < 0) {
        return DCC_ERR_INVALID_ARG;
    }
    *used += (size_t)n;
    return buf == NULL || *used < buf_len ? DCC_OK : DCC_ERR_NOMEM;
}
