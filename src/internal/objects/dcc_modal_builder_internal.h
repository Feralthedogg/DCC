#ifndef DCC_MODAL_BUILDER_INTERNAL_H
#define DCC_MODAL_BUILDER_INTERNAL_H

#include <dcc/modal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_modal_json_buffer {
    char *data;
    size_t len;
    size_t cap;
} dcc_modal_json_buffer_t;

void dcc_modal_json_buffer_deinit(dcc_modal_json_buffer_t *buffer);
dcc_status_t dcc_modal_json_append(dcc_modal_json_buffer_t *buffer, const void *data, size_t len);
dcc_status_t dcc_modal_json_append_cstr(dcc_modal_json_buffer_t *buffer, const char *value);
dcc_status_t dcc_modal_json_append_escaped_string(
    dcc_modal_json_buffer_t *buffer,
    const char *value
);
dcc_status_t dcc_modal_json_member_prefix(
    dcc_modal_json_buffer_t *buffer,
    int *first,
    const char *name
);
dcc_status_t dcc_modal_json_append_string_member(
    dcc_modal_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
);
dcc_status_t dcc_modal_json_append_raw_member(
    dcc_modal_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
);

#ifdef __cplusplus
}
#endif

#endif
