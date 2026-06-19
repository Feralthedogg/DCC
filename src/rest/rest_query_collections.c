#include "internal/rest/dcc_rest_query_collections_internal.h"

dcc_status_t dcc_rest_query_append_u64_repeated(
    dcc_rest_buffer_t *query,
    const char *key,
    const dcc_snowflake_t *values,
    size_t count
) {
    if (query == NULL || key == NULL || key[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (count == 0) {
        return DCC_OK;
    }
    if (values == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = DCC_OK;
    for (size_t i = 0; i < count && status == DCC_OK; ++i) {
        status = dcc_rest_query_append_u64(query, key, values[i]);
    }
    return status;
}

dcc_status_t dcc_rest_query_append_string_repeated(
    dcc_rest_buffer_t *query,
    const char *key,
    const char * const *values,
    size_t count
) {
    if (query == NULL || key == NULL || key[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    if (count == 0) {
        return DCC_OK;
    }
    if (values == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = DCC_OK;
    for (size_t i = 0; i < count && status == DCC_OK; ++i) {
        status = dcc_rest_query_append_string(query, key, values[i]);
    }
    return status;
}
