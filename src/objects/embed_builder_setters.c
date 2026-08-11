#include "internal/objects/dcc_builder_abi_internal.h"

#include <string.h>

void dcc_embed_builder_init(dcc_embed_builder_t *builder) {
    if (builder != NULL) {
        memset(builder, 0, sizeof(*builder));
        builder->size = sizeof(*builder);
        builder->version = DCC_EMBED_BUILDER_VERSION;
    }
}

static dcc_status_t dcc_embed_setter_target(
    dcc_embed_builder_t *builder,
    size_t offset,
    size_t width
) {
    dcc_builder_abi_view_t view;
    dcc_status_t status = dcc_embed_builder_abi_validate(builder, &view);
    return status == DCC_OK && dcc_builder_abi_field_covered(view.size, offset, width)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

static void dcc_embed_set_pointer_presence(
    dcc_embed_builder_t *builder,
    uint64_t bit,
    const void *value
) {
    if (value != NULL) {
        builder->present |= bit;
    } else {
        builder->present &= ~bit;
    }
}

#define DCC_EMBED_TARGET(builder_, field_) \
    dcc_embed_setter_target( \
        (builder_), offsetof(dcc_embed_builder_t, field_), \
        sizeof(((dcc_embed_builder_t *)0)->field_) \
    )

dcc_status_t dcc_embed_builder_set_title(dcc_embed_builder_t *builder, const char *title) {
    if (DCC_EMBED_TARGET(builder, title) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->title = title;
    dcc_embed_set_pointer_presence(builder, DCC_EMBED_BUILDER_PRESENT_TITLE, title);
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_type(dcc_embed_builder_t *builder, const char *type) {
    if (DCC_EMBED_TARGET(builder, type) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->type = type;
    dcc_embed_set_pointer_presence(builder, DCC_EMBED_BUILDER_PRESENT_TYPE, type);
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_description(dcc_embed_builder_t *builder, const char *description) {
    if (DCC_EMBED_TARGET(builder, description) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->description = description;
    dcc_embed_set_pointer_presence(builder, DCC_EMBED_BUILDER_PRESENT_DESCRIPTION, description);
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_url(dcc_embed_builder_t *builder, const char *url) {
    if (DCC_EMBED_TARGET(builder, url) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->url = url;
    dcc_embed_set_pointer_presence(builder, DCC_EMBED_BUILDER_PRESENT_URL, url);
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_timestamp(dcc_embed_builder_t *builder, const char *timestamp) {
    if (DCC_EMBED_TARGET(builder, timestamp) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->timestamp = timestamp;
    dcc_embed_set_pointer_presence(builder, DCC_EMBED_BUILDER_PRESENT_TIMESTAMP, timestamp);
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_set_color(dcc_embed_builder_t *builder, uint32_t color) {
    if (DCC_EMBED_TARGET(builder, color) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->color = color;
    builder->present |= DCC_EMBED_BUILDER_PRESENT_COLOR;
    return DCC_OK;
}
