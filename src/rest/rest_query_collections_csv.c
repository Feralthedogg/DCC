#include "internal/rest/dcc_rest_query_collections_internal.h"

#include <stdio.h>

dcc_status_t dcc_rest_query_append_snowflake_csv(
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

    dcc_rest_buffer_t csv = {0};
    dcc_status_t status = DCC_OK;
    for (size_t i = 0; i < count && status == DCC_OK; ++i) {
        char value_text[32];
        int written = snprintf(value_text, sizeof(value_text), "%llu", (unsigned long long)values[i]);
        if (written < 0 || (size_t)written >= sizeof(value_text)) {
            status = DCC_ERR_INVALID_ARG;
            break;
        }
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(&csv, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_buffer_append_cstr(&csv, value_text);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_query_append_string(query, key, csv.data);
    }
    dcc_rest_buffer_deinit(&csv);
    return status;
}
