#include <dcc/message.h>

dcc_status_t dcc_message_builder_set_sticker_ids(
    dcc_message_builder_t *builder,
    const dcc_snowflake_t *sticker_ids,
    size_t sticker_ids_count
) {
    if (builder == NULL || (sticker_ids_count != 0 && sticker_ids == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->sticker_ids = sticker_ids;
    builder->sticker_ids_count = sticker_ids_count;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_embeds(
    dcc_message_builder_t *builder,
    const dcc_embed_builder_t *embeds,
    size_t embeds_count
) {
    if (builder == NULL || (embeds_count != 0 && embeds == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->embeds = embeds;
    builder->embeds_count = embeds_count;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_components(
    dcc_message_builder_t *builder,
    const dcc_component_builder_t *components,
    size_t components_count
) {
    if (builder == NULL || (components_count != 0 && components == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components = components;
    builder->components_count = components_count;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_poll(
    dcc_message_builder_t *builder,
    const dcc_poll_builder_t *poll
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->poll = poll;
    return DCC_OK;
}
