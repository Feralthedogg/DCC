#include "internal/interactions/dcc_interaction_server_internal.h"

#include <string.h>

dcc_status_t dcc_interaction_request_set_response(
    dcc_interaction_request_t *request,
    uint16_t status,
    const char *content_type,
    const void *body,
    size_t body_len
) {
    return dcc_interaction_request_set_response_copy(request, status, content_type, body, body_len);
}

dcc_status_t dcc_interaction_request_reply_json(dcc_interaction_request_t *request, const char *json_body) {
    if (json_body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_interaction_request_set_response_copy(
        request,
        200,
        "application/json",
        json_body,
        strlen(json_body)
    );
}

dcc_status_t dcc_interaction_request_reply_text(
    dcc_interaction_request_t *request,
    uint16_t status,
    const char *text
) {
    if (text == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_interaction_request_set_response_copy(
        request,
        status,
        "text/plain; charset=utf-8",
        text,
        strlen(text)
    );
}
