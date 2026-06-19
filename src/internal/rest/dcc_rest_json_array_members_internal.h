#ifndef DCC_REST_JSON_ARRAY_MEMBERS_INTERNAL_H
#define DCC_REST_JSON_ARRAY_MEMBERS_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_json_append_snowflake_string_array_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const dcc_snowflake_t *values,
    size_t count
);
dcc_status_t dcc_rest_json_append_string_array_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const char * const *values,
    size_t count
);
dcc_status_t dcc_rest_json_append_automod_preset_array_member(
    dcc_rest_buffer_t *body,
    int *first,
    const char *key,
    const dcc_auto_moderation_preset_type_t *values,
    size_t count
);

#ifdef __cplusplus
}
#endif

#endif
