#ifndef DCC_AUTOCOMPLETE_JSON_MEMBERS_INTERNAL_H
#define DCC_AUTOCOMPLETE_JSON_MEMBERS_INTERNAL_H

#include "internal/objects/dcc_autocomplete_json_buffer_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_autocomplete_json_member_prefix(
    dcc_autocomplete_json_buffer_t *buffer,
    int *first,
    const char *name
);
dcc_status_t dcc_autocomplete_json_append_string_member(
    dcc_autocomplete_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
);
dcc_status_t dcc_autocomplete_json_append_raw_member(
    dcc_autocomplete_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
);

#ifdef __cplusplus
}
#endif

#endif
