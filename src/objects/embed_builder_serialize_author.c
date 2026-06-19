#include "internal/objects/dcc_embed_serialize_internal.h"

dcc_status_t dcc_embed_append_author_json(
    const dcc_embed_builder_t *builder,
    dcc_embed_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = dcc_embed_json_member_prefix(buffer, first, "author");
    if (status == DCC_OK) {
        status = dcc_embed_json_append_cstr(buffer, "{\"name\":");
    }
    if (status == DCC_OK) {
        status = dcc_embed_json_append_escaped_string(buffer, builder->author.name);
    }
    if (status == DCC_OK && builder->author.url != NULL) {
        status = dcc_embed_json_append_cstr(buffer, ",\"url\":");
        if (status == DCC_OK) {
            status = dcc_embed_json_append_escaped_string(buffer, builder->author.url);
        }
    }
    if (status == DCC_OK && builder->author.icon_url != NULL) {
        status = dcc_embed_json_append_cstr(buffer, ",\"icon_url\":");
        if (status == DCC_OK) {
            status = dcc_embed_json_append_escaped_string(buffer, builder->author.icon_url);
        }
    }
    if (status == DCC_OK) {
        status = dcc_embed_json_append_cstr(buffer, "}");
    }
    return status;
}
