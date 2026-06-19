#ifndef DCC_REST_BUFFER_INTERNAL_H
#define DCC_REST_BUFFER_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_rest_buffer {
    char *data;
    size_t len;
    size_t cap;
} dcc_rest_buffer_t;

void dcc_rest_buffer_deinit(dcc_rest_buffer_t *buffer);
dcc_status_t dcc_rest_buffer_append(dcc_rest_buffer_t *buffer, const void *data, size_t len);
dcc_status_t dcc_rest_buffer_append_cstr(dcc_rest_buffer_t *buffer, const char *data);
dcc_status_t dcc_rest_buffer_append_json_string(dcc_rest_buffer_t *buffer, const char *value);
dcc_status_t dcc_rest_buffer_append_u64_text(dcc_rest_buffer_t *buffer, uint64_t value);
dcc_status_t dcc_rest_buffer_append_snowflake_json_string_array(
    dcc_rest_buffer_t *buffer,
    const dcc_snowflake_t *values,
    size_t count
);
dcc_status_t dcc_rest_buffer_append_json_string_array(
    dcc_rest_buffer_t *buffer,
    const char * const *values,
    size_t count
);
dcc_status_t dcc_rest_buffer_append_automod_preset_array(
    dcc_rest_buffer_t *buffer,
    const dcc_auto_moderation_preset_type_t *values,
    size_t count
);

#ifdef __cplusplus
}
#endif

#endif
