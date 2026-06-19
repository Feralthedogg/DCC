#include "internal/objects/dcc_embed_serialize_internal.h"

dcc_status_t dcc_embed_append_footer_json(
    const dcc_embed_builder_t *builder,
    dcc_embed_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = dcc_embed_json_member_prefix(buffer, first, "footer");
    if (status == DCC_OK) {
        status = dcc_embed_json_append_cstr(buffer, "{\"text\":");
    }
    if (status == DCC_OK) {
        status = dcc_embed_json_append_escaped_string(buffer, builder->footer.text);
    }
    if (status == DCC_OK && builder->footer.icon_url != NULL) {
        status = dcc_embed_json_append_cstr(buffer, ",\"icon_url\":");
        if (status == DCC_OK) {
            status = dcc_embed_json_append_escaped_string(buffer, builder->footer.icon_url);
        }
    }
    if (status == DCC_OK) {
        status = dcc_embed_json_append_cstr(buffer, "}");
    }
    return status;
}
