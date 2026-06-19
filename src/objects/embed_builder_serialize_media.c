#include "internal/objects/dcc_embed_serialize_internal.h"

dcc_status_t dcc_embed_append_media_object(
    dcc_embed_json_buffer_t *buffer,
    int *first,
    const char *name,
    const dcc_embed_media_t *media
) {
    dcc_status_t status = dcc_embed_json_member_prefix(buffer, first, name);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_embed_json_append_cstr(buffer, "{\"url\":");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_embed_json_append_escaped_string(buffer, media->url);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_embed_json_append_cstr(buffer, "}");
}
