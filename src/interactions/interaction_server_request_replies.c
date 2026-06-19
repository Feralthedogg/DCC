#include "internal/interactions/dcc_interaction_server_internal.h"

#include <stdio.h>

dcc_status_t dcc_interaction_request_reply_type(
    dcc_interaction_request_t *request,
    dcc_interaction_response_type_t type
) {
    if (type == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    char body[32];
    int len = snprintf(body, sizeof(body), "{\"type\":%u}", (unsigned)type);
    if (len <= 0 || (size_t)len >= sizeof(body)) {
        return DCC_ERR_NOMEM;
    }
    return dcc_interaction_request_reply_json(request, body);
}

dcc_status_t dcc_interaction_request_reply_pong(dcc_interaction_request_t *request) {
    return dcc_interaction_request_reply_type(request, DCC_INTERACTION_RESPONSE_PONG);
}

dcc_status_t dcc_interaction_request_reply_deferred_message(
    dcc_interaction_request_t *request
) {
    return dcc_interaction_request_reply_type(
        request,
        DCC_INTERACTION_RESPONSE_DEFERRED_CHANNEL_MESSAGE_WITH_SOURCE
    );
}

dcc_status_t dcc_interaction_request_reply_deferred_update(
    dcc_interaction_request_t *request
) {
    return dcc_interaction_request_reply_type(
        request,
        DCC_INTERACTION_RESPONSE_DEFERRED_UPDATE_MESSAGE
    );
}

dcc_status_t dcc_interaction_request_reply_premium_required(
    dcc_interaction_request_t *request
) {
    return dcc_interaction_request_reply_type(
        request,
        DCC_INTERACTION_RESPONSE_PREMIUM_REQUIRED
    );
}
