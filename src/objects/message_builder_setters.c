#include "internal/objects/dcc_builder_abi_internal.h"

static dcc_status_t dcc_message_setter_target(
    dcc_message_builder_t *builder,
    size_t offset,
    size_t width
) {
    dcc_builder_abi_view_t view;
    dcc_status_t status = dcc_message_builder_abi_validate(builder, &view);
    return status == DCC_OK && dcc_builder_abi_field_covered(view.size, offset, width)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

#define DCC_MESSAGE_SETTER_TARGET(builder_, field_) \
    dcc_message_setter_target( \
        (builder_), offsetof(dcc_message_builder_t, field_), \
        sizeof(((dcc_message_builder_t *)0)->field_) \
    )

dcc_status_t dcc_message_builder_set_content(dcc_message_builder_t *builder, const char *content) {
    if (DCC_MESSAGE_SETTER_TARGET(builder, content) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->content = content;
    if (content != NULL) {
        builder->present |= DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
    } else {
        builder->present &= ~DCC_MESSAGE_BUILDER_PRESENT_CONTENT;
    }
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_nonce(dcc_message_builder_t *builder, const char *nonce) {
    if (DCC_MESSAGE_SETTER_TARGET(builder, nonce) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->nonce = nonce;
    if (nonce != NULL) {
        builder->present |= DCC_MESSAGE_BUILDER_PRESENT_NONCE;
    } else {
        builder->present &= ~DCC_MESSAGE_BUILDER_PRESENT_NONCE;
    }
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_tts(dcc_message_builder_t *builder, uint8_t tts) {
    if (DCC_MESSAGE_SETTER_TARGET(builder, tts) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->tts = tts ? 1U : 0U;
    builder->present |= DCC_MESSAGE_BUILDER_PRESENT_TTS;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_flags(dcc_message_builder_t *builder, uint64_t flags) {
    if (DCC_MESSAGE_SETTER_TARGET(builder, flags) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->flags = flags;
    builder->present |= DCC_MESSAGE_BUILDER_PRESENT_FLAGS;
    return DCC_OK;
}
