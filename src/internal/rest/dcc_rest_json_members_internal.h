#ifndef DCC_REST_JSON_MEMBERS_INTERNAL_H
#define DCC_REST_JSON_MEMBERS_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_json_member_sep(dcc_rest_buffer_t *body, int *first);
dcc_status_t dcc_rest_json_append_member_key(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key
);
dcc_status_t dcc_rest_json_append_string_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char *value
);
dcc_status_t dcc_rest_json_append_nullable_string_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char *value
);
dcc_status_t dcc_rest_json_append_u64_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    uint64_t value
);
dcc_status_t dcc_rest_json_append_u64_string_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    uint64_t value
);
dcc_status_t dcc_rest_json_append_bool_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    uint8_t value
);
dcc_status_t dcc_rest_json_append_raw_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char *value
);

#ifdef __cplusplus
}
#endif

#endif
