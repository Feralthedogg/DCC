#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stdio.h>
#include <stdlib.h>

dcc_status_t dcc_rest_query_append_u64_value(
    dcc_rest_buffer_t *query,
    const char *key,
    uint64_t value
) {
    if (query == NULL || key == NULL || key[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    char value_text[32];
    int written = snprintf(value_text, sizeof(value_text), "%llu", (unsigned long long)value);
    if (written < 0 || (size_t)written >= sizeof(value_text)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = DCC_OK;
    if (query->len != 0) {
        status = dcc_rest_buffer_append_cstr(query, "&");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(query, key);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(query, "=");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(query, value_text);
    }
    return status;
}

dcc_status_t dcc_rest_query_append_u64(
    dcc_rest_buffer_t *query,
    const char *key,
    uint64_t value
) {
    if (query == NULL || key == NULL || key[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (value == 0) {
        return DCC_OK;
    }
    return dcc_rest_query_append_u64_value(query, key, value);
}

dcc_status_t dcc_rest_query_append_string(
    dcc_rest_buffer_t *query,
    const char *key,
    const char *value
) {
    if (query == NULL || key == NULL || key[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (value == NULL || value[0] == '\0') {
        return DCC_OK;
    }

    char *encoded = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(value, &encoded);
    if (status != DCC_OK) {
        return status;
    }

    if (query->len != 0) {
        status = dcc_rest_buffer_append_cstr(query, "&");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(query, key);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(query, "=");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(query, encoded);
    }
    free(encoded);
    return status;
}

dcc_status_t dcc_rest_query_append_bool(
    dcc_rest_buffer_t *query,
    const char *key,
    uint8_t value
) {
    return dcc_rest_query_append_string(query, key, value != 0 ? "true" : "false");
}

dcc_status_t dcc_rest_query_append_bool_true(
    dcc_rest_buffer_t *query,
    const char *key,
    uint8_t value
) {
    if (query == NULL || key == NULL || key[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (value == 0) {
        return DCC_OK;
    }
    return dcc_rest_query_append_bool(query, key, 1);
}
