#ifndef DCC_REST_QUERY_COLLECTIONS_INTERNAL_H
#define DCC_REST_QUERY_COLLECTIONS_INTERNAL_H

#include "internal/rest/dcc_rest_query_append_internal.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_query_append_snowflake_csv(
    dcc_rest_buffer_t *query,
    const char *key,
    const dcc_snowflake_t *values,
    size_t count
);
dcc_status_t dcc_rest_query_append_u64_repeated(
    dcc_rest_buffer_t *query,
    const char *key,
    const dcc_snowflake_t *values,
    size_t count
);
dcc_status_t dcc_rest_query_append_string_repeated(
    dcc_rest_buffer_t *query,
    const char *key,
    const char * const *values,
    size_t count
);
dcc_status_t dcc_rest_query_append_search_has_repeated(
    dcc_rest_buffer_t *query,
    const dcc_message_search_has_t *values,
    size_t count
);

#ifdef __cplusplus
}
#endif

#endif
