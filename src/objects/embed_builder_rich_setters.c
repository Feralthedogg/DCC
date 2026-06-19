#include <dcc/embed.h>

dcc_status_t dcc_embed_builder_set_footer(
    dcc_embed_builder_t *builder,
    const char *text,
    const char *icon_url
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->footer.text = text;
    builder->footer.icon_url = icon_url;
    builder->has_footer = text != NULL;
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_image(dcc_embed_builder_t *builder, const char *url) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->image.url = url;
    builder->has_image = url != NULL;
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_thumbnail(dcc_embed_builder_t *builder, const char *url) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->thumbnail.url = url;
    builder->has_thumbnail = url != NULL;
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_author(
    dcc_embed_builder_t *builder,
    const char *name,
    const char *url,
    const char *icon_url
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->author.name = name;
    builder->author.url = url;
    builder->author.icon_url = icon_url;
    builder->has_author = name != NULL;
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_fields(
    dcc_embed_builder_t *builder,
    const dcc_embed_field_t *fields,
    size_t field_count
) {
    if (builder == NULL || (field_count != 0 && fields == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->fields = fields;
    builder->field_count = field_count;
    return DCC_OK;
}
