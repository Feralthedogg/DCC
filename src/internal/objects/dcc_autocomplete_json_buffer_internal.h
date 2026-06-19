#ifndef DCC_AUTOCOMPLETE_JSON_BUFFER_INTERNAL_H
#define DCC_AUTOCOMPLETE_JSON_BUFFER_INTERNAL_H

#include <dcc/error.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_autocomplete_json_buffer {
    char *data;
    size_t len;
    size_t cap;
} dcc_autocomplete_json_buffer_t;

void dcc_autocomplete_json_buffer_deinit(dcc_autocomplete_json_buffer_t *buffer);
dcc_status_t dcc_autocomplete_json_append(
    dcc_autocomplete_json_buffer_t *buffer,
    const void *data,
    size_t len
);
dcc_status_t dcc_autocomplete_json_append_cstr(
    dcc_autocomplete_json_buffer_t *buffer,
    const char *value
);
dcc_status_t dcc_autocomplete_json_append_i64(
    dcc_autocomplete_json_buffer_t *buffer,
    int64_t value
);
dcc_status_t dcc_autocomplete_json_append_double(
    dcc_autocomplete_json_buffer_t *buffer,
    double value
);
dcc_status_t dcc_autocomplete_json_append_escaped_string(
    dcc_autocomplete_json_buffer_t *buffer,
    const char *value
);

#ifdef __cplusplus
}
#endif

#endif
