#ifndef DCC_COMPONENT_JSON_BUFFER_INTERNAL_H
#define DCC_COMPONENT_JSON_BUFFER_INTERNAL_H

#include <dcc/component.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_component_json_buffer {
    char *data;
    size_t len;
    size_t cap;
} dcc_component_json_buffer_t;

void dcc_component_json_buffer_deinit(dcc_component_json_buffer_t *buffer);
dcc_status_t dcc_component_json_append(
    dcc_component_json_buffer_t *buffer,
    const void *data,
    size_t len
);
dcc_status_t dcc_component_json_append_cstr(dcc_component_json_buffer_t *buffer, const char *value);
dcc_status_t dcc_component_json_append_u32(dcc_component_json_buffer_t *buffer, uint32_t value);
dcc_status_t dcc_component_json_append_snowflake_string(
    dcc_component_json_buffer_t *buffer,
    dcc_snowflake_t value
);
dcc_status_t dcc_component_json_append_escaped_string(
    dcc_component_json_buffer_t *buffer,
    const char *value
);

#ifdef __cplusplus
}
#endif

#endif
