#ifndef DCC_REST_QUERY_APPEND_INTERNAL_H
#define DCC_REST_QUERY_APPEND_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/rest.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_query_append_u64_value(
    dcc_rest_buffer_t *query,
    const char *key,
    uint64_t value
);
dcc_status_t dcc_rest_query_append_u64(dcc_rest_buffer_t *query, const char *key, uint64_t value);
dcc_status_t dcc_rest_query_append_string(
    dcc_rest_buffer_t *query,
    const char *key,
    const char *value
);
dcc_status_t dcc_rest_query_append_bool(dcc_rest_buffer_t *query, const char *key, uint8_t value);
dcc_status_t dcc_rest_query_append_bool_true(dcc_rest_buffer_t *query, const char *key, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif
