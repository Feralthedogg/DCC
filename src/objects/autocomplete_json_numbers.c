#include "internal/objects/dcc_autocomplete_json_buffer_internal.h"

#include <math.h>
#include <stdio.h>

dcc_status_t dcc_autocomplete_json_append_i64(
    dcc_autocomplete_json_buffer_t *buffer,
    int64_t value
) {
    char tmp[32];
    int written = snprintf(tmp, sizeof(tmp), "%lld", (long long)value);
    if (written < 0 || (size_t)written >= sizeof(tmp)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_autocomplete_json_append(buffer, tmp, (size_t)written);
}

dcc_status_t dcc_autocomplete_json_append_double(
    dcc_autocomplete_json_buffer_t *buffer,
    double value
) {
    if (!isfinite(value)) {
        return DCC_ERR_INVALID_ARG;
    }

    char tmp[64];
    int written = snprintf(tmp, sizeof(tmp), "%.17g", value);
    if (written < 0 || (size_t)written >= sizeof(tmp)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_autocomplete_json_append(buffer, tmp, (size_t)written);
}
