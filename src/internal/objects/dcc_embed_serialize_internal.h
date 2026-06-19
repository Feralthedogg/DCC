#ifndef DCC_EMBED_SERIALIZE_INTERNAL_H
#define DCC_EMBED_SERIALIZE_INTERNAL_H

#include "internal/objects/dcc_embed_builder_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_embed_append_media_object(
    dcc_embed_json_buffer_t *buffer,
    int *first,
    const char *name,
    const dcc_embed_media_t *media
);
dcc_status_t dcc_embed_append_footer_json(
    const dcc_embed_builder_t *builder,
    dcc_embed_json_buffer_t *buffer,
    int *first
);
dcc_status_t dcc_embed_append_author_json(
    const dcc_embed_builder_t *builder,
    dcc_embed_json_buffer_t *buffer,
    int *first
);
dcc_status_t dcc_embed_append_fields_json(
    const dcc_embed_builder_t *builder,
    dcc_embed_json_buffer_t *buffer,
    int *first
);

#ifdef __cplusplus
}
#endif

#endif
