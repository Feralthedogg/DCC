#include "internal/objects/dcc_embed_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

dcc_status_t dcc_embed_validate(const dcc_embed_builder_t *builder) {
    dcc_builder_abi_view_t view;
    if (dcc_embed_builder_abi_validate(builder, &view) != DCC_OK ||
        (dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_TITLE) && builder->title == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_TYPE) && builder->type == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_DESCRIPTION) && builder->description == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_URL) && builder->url == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_TIMESTAMP) && builder->timestamp == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_FOOTER) && builder->footer.text == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_IMAGE) && builder->image.url == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_THUMBNAIL) && builder->thumbnail.url == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_AUTHOR) && builder->author.name == NULL) ||
        (dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_FIELDS) &&
            builder->field_count != 0 && builder->fields == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t field_count = dcc_builder_abi_view_has(&view, DCC_EMBED_BUILDER_PRESENT_FIELDS)
        ? builder->field_count : 0U;
    for (size_t i = 0; i < field_count; ++i) {
        if (builder->fields[i].name == NULL || builder->fields[i].value == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    return DCC_OK;
}
