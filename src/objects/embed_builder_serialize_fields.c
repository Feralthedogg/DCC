#include "internal/objects/dcc_embed_serialize_internal.h"

dcc_status_t dcc_embed_append_fields_json(
    const dcc_embed_builder_t *builder,
    dcc_embed_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = dcc_embed_json_member_prefix(buffer, first, "fields");
    if (status == DCC_OK) {
        status = dcc_embed_json_append_cstr(buffer, "[");
    }
    for (size_t i = 0; status == DCC_OK && i < builder->field_count; ++i) {
        const dcc_embed_field_t *field = &builder->fields[i];
        if (i != 0) {
            status = dcc_embed_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_embed_json_append_cstr(buffer, "{\"name\":");
        }
        if (status == DCC_OK) {
            status = dcc_embed_json_append_escaped_string(buffer, field->name);
        }
        if (status == DCC_OK) {
            status = dcc_embed_json_append_cstr(buffer, ",\"value\":");
        }
        if (status == DCC_OK) {
            status = dcc_embed_json_append_escaped_string(buffer, field->value);
        }
        if (status == DCC_OK && field->has_inline) {
            status = dcc_embed_json_append_cstr(buffer, ",\"inline\":");
            if (status == DCC_OK) {
                status = dcc_embed_json_append_cstr(buffer, field->inline_field ? "true" : "false");
            }
        }
        if (status == DCC_OK) {
            status = dcc_embed_json_append_cstr(buffer, "}");
        }
    }
    if (status == DCC_OK) {
        status = dcc_embed_json_append_cstr(buffer, "]");
    }
    return status;
}
