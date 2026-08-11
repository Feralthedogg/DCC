#include "internal/objects/dcc_embed_serialize_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

dcc_status_t dcc_embed_builder_append_json(
    const dcc_embed_builder_t *builder,
    dcc_embed_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_embed_validate(builder);
    if (status != DCC_OK) {
        return status;
    }
    dcc_builder_abi_view_t view;
    status = dcc_embed_builder_abi_validate(builder, &view);
    if (status != DCC_OK) {
        return status;
    }
#define HAS(bit_) dcc_builder_abi_view_has(&view, (bit_))

    status = dcc_embed_json_append_cstr(buffer, "{");
    if (status != DCC_OK) {
        return status;
    }

    int first = 1;
    if (HAS(DCC_EMBED_BUILDER_PRESENT_TITLE)) {
        status = dcc_embed_json_append_string_member(buffer, &first, "title", builder->title);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_EMBED_BUILDER_PRESENT_TYPE)) {
        status = dcc_embed_json_append_string_member(buffer, &first, "type", builder->type);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_EMBED_BUILDER_PRESENT_DESCRIPTION)) {
        status = dcc_embed_json_append_string_member(buffer, &first, "description", builder->description);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_EMBED_BUILDER_PRESENT_URL)) {
        status = dcc_embed_json_append_string_member(buffer, &first, "url", builder->url);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_EMBED_BUILDER_PRESENT_TIMESTAMP)) {
        status = dcc_embed_json_append_string_member(buffer, &first, "timestamp", builder->timestamp);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_EMBED_BUILDER_PRESENT_COLOR)) {
        status = dcc_embed_json_member_prefix(buffer, &first, "color");
        if (status == DCC_OK) {
            status = dcc_embed_json_append_u32(buffer, builder->color);
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_EMBED_BUILDER_PRESENT_FOOTER)) {
        status = dcc_embed_append_footer_json(builder, buffer, &first);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_EMBED_BUILDER_PRESENT_IMAGE)) {
        status = dcc_embed_append_media_object(buffer, &first, "image", &builder->image);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_EMBED_BUILDER_PRESENT_THUMBNAIL)) {
        status = dcc_embed_append_media_object(buffer, &first, "thumbnail", &builder->thumbnail);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_EMBED_BUILDER_PRESENT_AUTHOR)) {
        status = dcc_embed_append_author_json(builder, buffer, &first);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (HAS(DCC_EMBED_BUILDER_PRESENT_FIELDS) && builder->field_count != 0U) {
        status = dcc_embed_append_fields_json(builder, buffer, &first);
        if (status != DCC_OK) {
            return status;
        }
    }

    #undef HAS
    return dcc_embed_json_append_cstr(buffer, "}");
}
