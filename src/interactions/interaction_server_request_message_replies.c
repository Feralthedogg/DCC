#include "internal/interactions/dcc_interaction_server_internal.h"
#include "internal/rest/dcc_rest_interaction_response_body_internal.h"

#include <stdlib.h>

dcc_status_t dcc_interaction_request_reply_message_builder(
    dcc_interaction_request_t *request,
    dcc_interaction_response_type_t type,
    const dcc_message_builder_t *message
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_interaction_response_message_body(type, message, &body);
    if (status == DCC_OK) {
        status = dcc_interaction_request_reply_json(request, body);
    }
    free(body);
    return status;
}

dcc_status_t dcc_interaction_request_reply_message(
    dcc_interaction_request_t *request,
    const dcc_message_builder_t *message
) {
    return dcc_interaction_request_reply_message_builder(
        request,
        DCC_INTERACTION_RESPONSE_CHANNEL_MESSAGE_WITH_SOURCE,
        message
    );
}

dcc_status_t dcc_interaction_request_reply_update_message(
    dcc_interaction_request_t *request,
    const dcc_message_builder_t *message
) {
    return dcc_interaction_request_reply_message_builder(
        request,
        DCC_INTERACTION_RESPONSE_UPDATE_MESSAGE,
        message
    );
}
