#include <dcc/message.h>

dcc_status_t dcc_message_builder_set_content(dcc_message_builder_t *builder, const char *content) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->content = content;
    builder->has_content = content != NULL;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_nonce(dcc_message_builder_t *builder, const char *nonce) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->nonce = nonce;
    builder->has_nonce = nonce != NULL;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_tts(dcc_message_builder_t *builder, uint8_t tts) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->tts = tts ? 1U : 0U;
    builder->has_tts = 1;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_set_flags(dcc_message_builder_t *builder, uint64_t flags) {
    if (builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    builder->flags = flags;
    builder->has_flags = 1;
    return DCC_OK;
}
