#ifndef DCC_EMBED_BUILDER_INTERNAL_H
#define DCC_EMBED_BUILDER_INTERNAL_H

#include <dcc/embed.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_embed_json_buffer {
    char *data;
    size_t len;
    size_t cap;
} dcc_embed_json_buffer_t;

void dcc_embed_json_buffer_deinit(dcc_embed_json_buffer_t *buffer);
dcc_status_t dcc_embed_json_append(dcc_embed_json_buffer_t *buffer, const void *data, size_t len);
dcc_status_t dcc_embed_json_append_cstr(dcc_embed_json_buffer_t *buffer, const char *value);
dcc_status_t dcc_embed_json_append_u32(dcc_embed_json_buffer_t *buffer, uint32_t value);
dcc_status_t dcc_embed_json_append_escaped_string(
    dcc_embed_json_buffer_t *buffer,
    const char *value
);
dcc_status_t dcc_embed_json_member_prefix(
    dcc_embed_json_buffer_t *buffer,
    int *first,
    const char *name
);
dcc_status_t dcc_embed_json_append_string_member(
    dcc_embed_json_buffer_t *buffer,
    int *first,
    const char *name,
    const char *value
);
dcc_status_t dcc_embed_validate(const dcc_embed_builder_t *builder);
dcc_status_t dcc_embed_builder_append_json(
    const dcc_embed_builder_t *builder,
    dcc_embed_json_buffer_t *buffer
);

#ifdef __cplusplus
}
#endif

#endif
