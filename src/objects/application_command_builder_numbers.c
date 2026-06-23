#include "internal/objects/dcc_application_command_builder_internal.h"

#include <stdio.h>

dcc_status_t dcc_command_json_append_u32(dcc_application_command_json_buffer_t *buffer, uint32_t value) {
    char tmp[16];
    int written = snprintf(tmp, sizeof(tmp), "%u", (unsigned)value);
    if (written < 0 || (size_t)written >= sizeof(tmp)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_command_json_append(buffer, tmp, (size_t)written);
}

dcc_status_t dcc_command_json_append_u64_string(
    dcc_application_command_json_buffer_t *buffer,
    uint64_t value
) {
    char tmp[32];
    int written = snprintf(tmp, sizeof(tmp), "\"%llu\"", (unsigned long long)value);
    if (written < 0 || (size_t)written >= sizeof(tmp)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_command_json_append(buffer, tmp, (size_t)written);
}

dcc_status_t dcc_command_json_append_i64(
    dcc_application_command_json_buffer_t *buffer,
    int64_t value
) {
    char tmp[32];
    int written = snprintf(tmp, sizeof(tmp), "%lld", (long long)value);
    if (written < 0 || (size_t)written >= sizeof(tmp)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_command_json_append(buffer, tmp, (size_t)written);
}

dcc_status_t dcc_command_json_append_double(
    dcc_application_command_json_buffer_t *buffer,
    double value
) {
    char tmp[64];
    int written = snprintf(tmp, sizeof(tmp), "%.17g", value);
    if (written < 0 || (size_t)written >= sizeof(tmp)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_command_json_append(buffer, tmp, (size_t)written);
}
