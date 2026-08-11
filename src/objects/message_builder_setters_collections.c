#include "internal/objects/dcc_builder_abi_internal.h"

static int dcc_message_collection_target(
    dcc_message_builder_t *builder,
    size_t first_offset,
    size_t first_width,
    size_t second_offset,
    size_t second_width
) {
    dcc_builder_abi_view_t view;
    return dcc_message_builder_abi_validate(builder, &view) == DCC_OK &&
        dcc_builder_abi_field_covered(view.size, first_offset, first_width) &&
        dcc_builder_abi_field_covered(view.size, second_offset, second_width);
}

#define DCC_MESSAGE_COLLECTION_TARGET(builder_, first_, second_) \
    dcc_message_collection_target( \
        (builder_), offsetof(dcc_message_builder_t, first_), \
        sizeof(((dcc_message_builder_t *)0)->first_), \
        offsetof(dcc_message_builder_t, second_), \
        sizeof(((dcc_message_builder_t *)0)->second_) \
    )

dcc_status_t dcc_message_builder_set_sticker_ids(
    dcc_message_builder_t *builder,
    const dcc_snowflake_t *sticker_ids,
    size_t sticker_ids_count
) {
    if (!DCC_MESSAGE_COLLECTION_TARGET(builder, sticker_ids, sticker_ids_count) ||
        (sticker_ids_count != 0 && sticker_ids == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->sticker_ids = sticker_ids;
    builder->sticker_ids_count = sticker_ids_count;
    if (sticker_ids_count != 0U) {
        builder->present |= DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS;
    } else {
        builder->present &= ~DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS;
    }
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_embeds(
    dcc_message_builder_t *builder,
    const dcc_embed_builder_t *embeds,
    size_t embeds_count
) {
    if (!DCC_MESSAGE_COLLECTION_TARGET(builder, embeds, embeds_count) ||
        (embeds_count != 0 && embeds == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->embeds = embeds;
    builder->embeds_count = embeds_count;
    if (embeds_count != 0U) {
        builder->present |= DCC_MESSAGE_BUILDER_PRESENT_EMBEDS;
    } else {
        builder->present &= ~DCC_MESSAGE_BUILDER_PRESENT_EMBEDS;
    }
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_components(
    dcc_message_builder_t *builder,
    const dcc_component_builder_t *components,
    size_t components_count
) {
    if (!DCC_MESSAGE_COLLECTION_TARGET(builder, components, components_count) ||
        (components_count != 0 && components == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components = components;
    builder->components_count = components_count;
    if (components_count != 0U) {
        builder->present |= DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS;
    } else {
        builder->present &= ~DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS;
    }
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_components_v2(
    dcc_message_builder_t *builder,
    const dcc_component_v2_builder_t *components,
    size_t components_count
) {
    dcc_builder_abi_view_t view;
    if (dcc_message_builder_abi_validate(builder, &view) != DCC_OK ||
        !dcc_builder_abi_field_covered(
            view.size, offsetof(dcc_message_builder_t, components_v2),
            sizeof(builder->components_v2)
        ) ||
        !dcc_builder_abi_field_covered(
            view.size, offsetof(dcc_message_builder_t, components_v2_count),
            sizeof(builder->components_v2_count)
        ) ||
        !dcc_builder_abi_field_covered(
            view.size, offsetof(dcc_message_builder_t, flags), sizeof(builder->flags)
        ) ||
        (components_count != 0 && components == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_v2 = components;
    builder->components_v2_count = components_count;
    if (components_count != 0) {
        builder->flags |= DCC_MESSAGE_FLAG_IS_COMPONENTS_V2;
        builder->present |= DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2 |
            DCC_MESSAGE_BUILDER_PRESENT_FLAGS;
    } else {
        builder->present &= ~DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2;
    }
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_poll(
    dcc_message_builder_t *builder,
    const dcc_poll_builder_t *poll
) {
    dcc_builder_abi_view_t view;
    if (dcc_message_builder_abi_validate(builder, &view) != DCC_OK ||
        !DCC_BUILDER_ABI_FIELD(dcc_message_builder_t, &view, poll)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->poll = poll;
    if (poll != NULL) {
        builder->present |= DCC_MESSAGE_BUILDER_PRESENT_POLL;
    } else {
        builder->present &= ~DCC_MESSAGE_BUILDER_PRESENT_POLL;
    }
    return DCC_OK;
}
