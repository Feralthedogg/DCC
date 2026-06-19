#include "internal/interactions/dcc_interaction_server_internal.h"

#include <stdint.h>

#define DCC_INTERACTION_RESPONSE_FLAG_EPHEMERAL UINT64_C(64)

static dcc_status_t dcc_interaction_request_reply_message_text_with_type(
    dcc_interaction_request_t *request,
    dcc_interaction_response_type_t type,
    const char *content,
    uint64_t flags,
    uint8_t has_flags
) {
    if (content == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_message_builder_t message;
    dcc_message_builder_init(&message);
    dcc_status_t status = dcc_message_builder_set_content(&message, content);
    if (status == DCC_OK && has_flags) {
        status = dcc_message_builder_set_flags(&message, flags);
    }
    return status == DCC_OK
        ? dcc_interaction_request_reply_message_builder(request, type, &message)
        : status;
}

dcc_status_t dcc_interaction_request_reply_message_text(
    dcc_interaction_request_t *request,
    const char *content
) {
    return dcc_interaction_request_reply_message_text_with_type(
        request,
        DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE,
        content,
        0,
        0
    );
}

dcc_status_t dcc_interaction_request_reply_ephemeral_message_text(
    dcc_interaction_request_t *request,
    const char *content
) {
    return dcc_interaction_request_reply_message_text_with_type(
        request,
        DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE,
        content,
        DCC_INTERACTION_RESPONSE_FLAG_EPHEMERAL,
        1
    );
}

dcc_status_t dcc_interaction_request_reply_update_message_text(
    dcc_interaction_request_t *request,
    const char *content
) {
    return dcc_interaction_request_reply_message_text_with_type(
        request,
        DCC_INTERACTION_RESPONSE_UPDATE_MESSAGE,
        content,
        0,
        0
    );
}
