#include "internal/objects/dcc_embed_serialize_internal.h"

dcc_status_t dcc_embed_builder_append_json(
    const dcc_embed_builder_t *builder,
    dcc_embed_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_embed_validate(builder);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_embed_json_append_cstr(buffer, "{");
    if (status != DCC_OK) {
        return status;
    }

    int first = 1;
    if (builder->title != NULL) {
        status = dcc_embed_json_append_string_member(buffer, &first, "title", builder->title);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->type != NULL) {
        status = dcc_embed_json_append_string_member(buffer, &first, "type", builder->type);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->description != NULL) {
        status = dcc_embed_json_append_string_member(buffer, &first, "description", builder->description);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->url != NULL) {
        status = dcc_embed_json_append_string_member(buffer, &first, "url", builder->url);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->timestamp != NULL) {
        status = dcc_embed_json_append_string_member(buffer, &first, "timestamp", builder->timestamp);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_color) {
        status = dcc_embed_json_member_prefix(buffer, &first, "color");
        if (status == DCC_OK) {
            status = dcc_embed_json_append_u32(buffer, builder->color);
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_footer) {
        status = dcc_embed_append_footer_json(builder, buffer, &first);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_image) {
        status = dcc_embed_append_media_object(buffer, &first, "image", &builder->image);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_thumbnail) {
        status = dcc_embed_append_media_object(buffer, &first, "thumbnail", &builder->thumbnail);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_author) {
        status = dcc_embed_append_author_json(builder, buffer, &first);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->field_count != 0) {
        status = dcc_embed_append_fields_json(builder, buffer, &first);
        if (status != DCC_OK) {
            return status;
        }
    }

    return dcc_embed_json_append_cstr(buffer, "}");
}
