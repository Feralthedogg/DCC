#include <dcc/embed.h>

#include <string.h>

void dcc_embed_builder_init(dcc_embed_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
    }
}

dcc_status_t dcc_embed_builder_set_title(dcc_embed_builder_t *builder, const char *title) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->title = title;
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_type(dcc_embed_builder_t *builder, const char *type) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->type = type;
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_description(dcc_embed_builder_t *builder, const char *description) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->description = description;
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_url(dcc_embed_builder_t *builder, const char *url) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->url = url;
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_timestamp(dcc_embed_builder_t *builder, const char *timestamp) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->timestamp = timestamp;
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_color(dcc_embed_builder_t *builder, uint32_t color) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->color = color;
    builder->has_color = 1;
    return DCC_OK;
}
