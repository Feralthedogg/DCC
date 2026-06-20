#include <dcc/message.h>

dcc_status_t dcc_message_builder_set_message_reference_json(
    dcc_message_builder_t *builder,
    const char *message_reference_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->message_reference_json = message_reference_json;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_allowed_mentions_json(
    dcc_message_builder_t *builder,
    const char *allowed_mentions_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->allowed_mentions_json = allowed_mentions_json;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_embeds_json(dcc_message_builder_t *builder, const char *embeds_json) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->embeds_json = embeds_json;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_components_json(
    dcc_message_builder_t *builder,
    const char *components_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_json = components_json;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_components_v2_json(
    dcc_message_builder_t *builder,
    const char *components_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_v2_json = components_json;
    if (components_json != NULL) {
        builder->flags |= DCC_MESSAGE_FLAG_IS_COMPONENTS_V2;
        builder->has_flags = 1;
    }
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_attachments_json(
    dcc_message_builder_t *builder,
    const char *attachments_json
) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->attachments_json = attachments_json;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_poll_json(dcc_message_builder_t *builder, const char *poll_json) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->poll_json = poll_json;
    return DCC_OK;
}
