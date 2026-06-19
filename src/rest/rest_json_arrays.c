#include "internal/rest/dcc_rest_buffer_internal.h"

#include <stdio.h>

dcc_status_t dcc_rest_buffer_append_u64_text(dcc_rest_buffer_t *buffer, uint64_t value) {
    char value_text[32];
    int written = snprintf(value_text, sizeof(value_text), "%llu", (unsigned long long)value);
    if (written < 0 || (size_t)written >= sizeof(value_text)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_buffer_append_cstr(buffer, value_text);
}

dcc_status_t dcc_rest_buffer_append_snowflake_json_string_array(
    dcc_rest_buffer_t *buffer,
    const dcc_snowflake_t *values,
    size_t count
) {
    if (buffer == NULL || (count != 0 && values == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(buffer, "[");
    for (size_t i = 0; i < count && status == DCC_OK; ++i) {
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(buffer, "\"");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_u64_text(buffer, values[i]);
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(buffer, "\"");
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(buffer, "]");
    }
    return status;
}

dcc_status_t dcc_rest_buffer_append_json_string_array(
    dcc_rest_buffer_t *buffer,
    const char * const *values,
    size_t count
) {
    if (buffer == NULL || (count != 0 && values == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(buffer, "[");
    for (size_t i = 0; i < count && status == DCC_OK; ++i) {
        if (values[i] == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_json_string(buffer, values[i]);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(buffer, "]");
    }
    return status;
}

dcc_status_t dcc_rest_buffer_append_automod_preset_array(
    dcc_rest_buffer_t *buffer,
    const dcc_auto_moderation_preset_type_t *values,
    size_t count
) {
    if (buffer == NULL || (count != 0 && values == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(buffer, "[");
    for (size_t i = 0; i < count && status == DCC_OK; ++i) {
        if (values[i] < DCC_AUTO_MODERATION_PRESET_PROFANITY ||
            values[i] > DCC_AUTO_MODERATION_PRESET_SLURS) {
            return DCC_ERR_INVALID_ARG;
        }
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_u64_text(buffer, (uint64_t)values[i]);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(buffer, "]");
    }
    return status;
}
