#include "internal/objects/dcc_builder_abi_internal.h"

static int dcc_embed_rich_target(
    dcc_embed_builder_t *builder,
    size_t first_offset,
    size_t first_width,
    size_t second_offset,
    size_t second_width
) {
    dcc_builder_abi_view_t view;
    return dcc_embed_builder_abi_validate(builder, &view) == DCC_OK &&
        dcc_builder_abi_field_covered(view.size, first_offset, first_width) &&
        dcc_builder_abi_field_covered(view.size, second_offset, second_width);
}

static void dcc_embed_rich_presence(dcc_embed_builder_t *builder, uint64_t bit, int present) {
    if (present) {
        builder->present |= bit;
    } else {
        builder->present &= ~bit;
    }
}

dcc_status_t dcc_embed_builder_set_footer(
    dcc_embed_builder_t *builder,
    const char *text,
    const char *icon_url
) {
    dcc_builder_abi_view_t view;
    if (dcc_embed_builder_abi_validate(builder, &view) != DCC_OK ||
        !DCC_BUILDER_ABI_FIELD(dcc_embed_builder_t, &view, footer)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->footer.text = text;
    builder->footer.icon_url = icon_url;
    dcc_embed_rich_presence(builder, DCC_EMBED_BUILDER_PRESENT_FOOTER, text != NULL);
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_image(dcc_embed_builder_t *builder, const char *url) {
    dcc_builder_abi_view_t view;
    if (dcc_embed_builder_abi_validate(builder, &view) != DCC_OK ||
        !DCC_BUILDER_ABI_FIELD(dcc_embed_builder_t, &view, image)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->image.url = url;
    dcc_embed_rich_presence(builder, DCC_EMBED_BUILDER_PRESENT_IMAGE, url != NULL);
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_thumbnail(dcc_embed_builder_t *builder, const char *url) {
    dcc_builder_abi_view_t view;
    if (dcc_embed_builder_abi_validate(builder, &view) != DCC_OK ||
        !DCC_BUILDER_ABI_FIELD(dcc_embed_builder_t, &view, thumbnail)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->thumbnail.url = url;
    dcc_embed_rich_presence(builder, DCC_EMBED_BUILDER_PRESENT_THUMBNAIL, url != NULL);
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_author(
    dcc_embed_builder_t *builder,
    const char *name,
    const char *url,
    const char *icon_url
) {
    dcc_builder_abi_view_t view;
    if (dcc_embed_builder_abi_validate(builder, &view) != DCC_OK ||
        !DCC_BUILDER_ABI_FIELD(dcc_embed_builder_t, &view, author)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->author.name = name;
    builder->author.url = url;
    builder->author.icon_url = icon_url;
    dcc_embed_rich_presence(builder, DCC_EMBED_BUILDER_PRESENT_AUTHOR, name != NULL);
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_fields(
    dcc_embed_builder_t *builder,
    const dcc_embed_field_t *fields,
    size_t field_count
) {
    if (!dcc_embed_rich_target(
            builder,
            offsetof(dcc_embed_builder_t, fields), sizeof(builder->fields),
            offsetof(dcc_embed_builder_t, field_count), sizeof(builder->field_count)
        ) || (field_count != 0 && fields == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->fields = fields;
    builder->field_count = field_count;
    dcc_embed_rich_presence(builder, DCC_EMBED_BUILDER_PRESENT_FIELDS, field_count != 0U);
    return DCC_OK;
}
