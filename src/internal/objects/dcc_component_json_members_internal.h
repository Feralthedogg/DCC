#ifndef DCC_COMPONENT_JSON_MEMBERS_INTERNAL_H
#define DCC_COMPONENT_JSON_MEMBERS_INTERNAL_H

#include "internal/objects/dcc_component_json_buffer_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_component_json_member_prefix(
    dcc_component_json_buffer_t *buffer,
    int *first,
    const char *name
);
dcc_status_t dcc_component_json_append_string_member(
    dcc_component_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
);
dcc_status_t dcc_component_json_append_bool_member(
    dcc_component_json_buffer_t *buffer,
    int *first,
    const char *name,
    uint8_t value
);
dcc_status_t dcc_component_json_append_u32_member(
    dcc_component_json_buffer_t *buffer,
    int *first,
    const char *name,
    uint32_t value
);
dcc_status_t dcc_component_json_append_snowflake_string_member(
    dcc_component_json_buffer_t *buffer,
    int *first,
    const char *name,
    dcc_snowflake_t value
);

#ifdef __cplusplus
}
#endif

#endif
