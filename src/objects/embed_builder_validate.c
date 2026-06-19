#include "internal/objects/dcc_embed_builder_internal.h"

dcc_status_t dcc_embed_validate(const dcc_embed_builder_t *builder) {
    if (builder == NULL ||
        (builder->has_footer && builder->footer.text == NULL) ||
        (builder->has_image && builder->image.url == NULL) ||
        (builder->has_thumbnail && builder->thumbnail.url == NULL) ||
        (builder->has_author && builder->author.name == NULL) ||
        (builder->field_count != 0 && builder->fields == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0; i < builder->field_count; ++i) {
        if (builder->fields[i].name == NULL || builder->fields[i].value == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    return DCC_OK;
}
