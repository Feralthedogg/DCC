#include "internal/objects/dcc_builder_abi_internal.h"

static dcc_status_t dcc_message_json_setter_target(
    dcc_message_builder_t *builder,
    size_t offset,
    size_t width
) {
    dcc_builder_abi_view_t view;
    dcc_status_t status = dcc_message_builder_abi_validate(builder, &view);
    return status == DCC_OK && dcc_builder_abi_field_covered(view.size, offset, width)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

static void dcc_message_set_pointer_presence(
    dcc_message_builder_t *builder,
    uint64_t bit,
    const void *value
) {
    if (value != NULL) {
        builder->present |= bit;
    } else {
        builder->present &= ~bit;
    }
}

#define DCC_MESSAGE_JSON_TARGET(builder_, field_) \
    dcc_message_json_setter_target( \
        (builder_), offsetof(dcc_message_builder_t, field_), \
        sizeof(((dcc_message_builder_t *)0)->field_) \
    )

dcc_status_t dcc_message_builder_set_message_reference_json(
    dcc_message_builder_t *builder,
    const char *message_reference_json
) {
    if (DCC_MESSAGE_JSON_TARGET(builder, message_reference_json) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->message_reference_json = message_reference_json;
    dcc_message_set_pointer_presence(
        builder, DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE_JSON, message_reference_json
    );
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_allowed_mentions_json(
    dcc_message_builder_t *builder,
    const char *allowed_mentions_json
) {
    if (DCC_MESSAGE_JSON_TARGET(builder, allowed_mentions_json) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->allowed_mentions_json = allowed_mentions_json;
    dcc_message_set_pointer_presence(
        builder, DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS_JSON, allowed_mentions_json
    );
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_allowed_mentions(
    dcc_message_builder_t *builder,
    const dcc_allowed_mentions_builder_t *allowed_mentions
) {
    if (DCC_MESSAGE_JSON_TARGET(builder, allowed_mentions) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->allowed_mentions = allowed_mentions;
    dcc_message_set_pointer_presence(
        builder, DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS, allowed_mentions
    );
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_embeds_json(dcc_message_builder_t *builder, const char *embeds_json) {
    if (DCC_MESSAGE_JSON_TARGET(builder, embeds_json) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->embeds_json = embeds_json;
    dcc_message_set_pointer_presence(
        builder, DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON, embeds_json
    );
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_components_json(
    dcc_message_builder_t *builder,
    const char *components_json
) {
    if (DCC_MESSAGE_JSON_TARGET(builder, components_json) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_json = components_json;
    dcc_message_set_pointer_presence(
        builder, DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON, components_json
    );
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_components_v2_json(
    dcc_message_builder_t *builder,
    const char *components_json
) {
    dcc_builder_abi_view_t view;
    if (dcc_message_builder_abi_validate(builder, &view) != DCC_OK ||
        !DCC_BUILDER_ABI_FIELD(dcc_message_builder_t, &view, components_v2_json) ||
        !DCC_BUILDER_ABI_FIELD(dcc_message_builder_t, &view, flags)) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->components_v2_json = components_json;
    if (components_json != NULL) {
        builder->flags |= DCC_MESSAGE_FLAG_IS_COMPONENTS_V2;
        builder->present |= DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON |
            DCC_MESSAGE_BUILDER_PRESENT_FLAGS;
    } else {
        builder->present &= ~DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON;
    }
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_attachments_json(
    dcc_message_builder_t *builder,
    const char *attachments_json
) {
    if (DCC_MESSAGE_JSON_TARGET(builder, attachments_json) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->attachments_json = attachments_json;
    dcc_message_set_pointer_presence(
        builder, DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON, attachments_json
    );
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_poll_json(dcc_message_builder_t *builder, const char *poll_json) {
    if (DCC_MESSAGE_JSON_TARGET(builder, poll_json) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->poll_json = poll_json;
    dcc_message_set_pointer_presence(builder, DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON, poll_json);
    return DCC_OK;
}
