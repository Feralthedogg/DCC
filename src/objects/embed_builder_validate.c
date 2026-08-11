#include "internal/objects/dcc_embed_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <stdint.h>

#define DCC_EMBED_MAX_FIELDS 25U

static int dcc_embed_array_span_valid(
    const void *values,
    size_t count,
    size_t item_size
) {
    if (count == 0U) {
        return 1;
    }
    if (values == NULL || item_size == 0U || count > SIZE_MAX / item_size) {
        return 0;
    }
    size_t span = count * item_size;
    uintptr_t address = (uintptr_t)values;
    return address <= UINTPTR_MAX - (span - 1U);
}

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
            (builder->field_count > DCC_EMBED_MAX_FIELDS ||
             !dcc_embed_array_span_valid(
                 builder->fields,
                 builder->field_count,
                 sizeof(*builder->fields)
             )))) {
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

dcc_status_t dcc_embed_builder_validate_array(
    const dcc_embed_builder_t *builders,
    size_t builder_count
) {
    size_t stride = 0U;
    dcc_status_t status = dcc_embed_builder_array_begin(
        builders,
        builder_count,
        &stride
    );
    if (status != DCC_OK) {
        return status;
    }
    for (size_t i = 0U; i < builder_count; ++i) {
        const dcc_embed_builder_t *builder = (const dcc_embed_builder_t *)
            dcc_builder_abi_array_at(builders, stride, i);
        dcc_builder_abi_view_t view;
        status = dcc_embed_builder_abi_validate(builder, &view);
        if (status != DCC_OK || view.size != stride ||
            dcc_embed_validate(builder) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
    }
    return DCC_OK;
}
