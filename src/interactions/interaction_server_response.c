#include "internal/interactions/dcc_interaction_server_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_interaction_request_set_response_copy(
    dcc_interaction_request_t *request,
    uint16_t status,
    const char *content_type,
    const void *body,
    size_t body_len
) {
    if (request == NULL || status < 100U || content_type == NULL || (body == NULL && body_len > 0U)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (request->parsed && status < 400U && request->deadline_at_ns != 0U &&
        llam_now_ns() > request->deadline_at_ns) {
        if (!request->deadline_noted) {
            atomic_fetch_add_explicit(
                &request->server->deadline_exceeded_requests,
                1U,
                memory_order_relaxed
            );
            request->deadline_noted = 1U;
        }
        return DCC_ERR_TIMEOUT;
    }
    char *content_type_copy = dcc_strdup(content_type);
    if (content_type_copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    if (body_len == SIZE_MAX) {
        free(content_type_copy);
        return DCC_ERR_NOMEM;
    }
    char *body_copy = (char *)malloc(body_len + 1U);
    if (body_copy == NULL) {
        free(content_type_copy);
        return DCC_ERR_NOMEM;
    }
    if (body_len > 0U) {
        memcpy(body_copy, body, body_len);
    }
    body_copy[body_len] = '\0';

    free(request->response_content_type);
    free(request->response_body);
    request->response_status = status;
    request->response_content_type = content_type_copy;
    request->response_body = body_copy;
    request->response_body_len = body_len;
    request->response_set = 1U;
    return DCC_OK;
}

void dcc_interaction_server_note_response(dcc_interaction_server_t *server, uint16_t status) {
    if (server == NULL) {
        return;
    }
    if (status >= 200U && status < 300U) {
        atomic_fetch_add_explicit(&server->response_2xx, 1U, memory_order_relaxed);
    } else if (status >= 300U && status < 400U) {
        atomic_fetch_add_explicit(&server->response_3xx, 1U, memory_order_relaxed);
    } else if (status >= 400U && status < 500U) {
        atomic_fetch_add_explicit(&server->response_4xx, 1U, memory_order_relaxed);
    } else if (status >= 500U && status < 600U) {
        atomic_fetch_add_explicit(&server->response_5xx, 1U, memory_order_relaxed);
    }

    switch (status) {
        case 400:
            atomic_fetch_add_explicit(&server->bad_request_responses, 1U, memory_order_relaxed);
            break;
        case 401:
            atomic_fetch_add_explicit(&server->unauthorized_responses, 1U, memory_order_relaxed);
            break;
        case 404:
            atomic_fetch_add_explicit(&server->not_found_responses, 1U, memory_order_relaxed);
            break;
        case 405:
            atomic_fetch_add_explicit(&server->method_not_allowed_responses, 1U, memory_order_relaxed);
            break;
        case 413:
            atomic_fetch_add_explicit(&server->payload_too_large_responses, 1U, memory_order_relaxed);
            break;
        default:
            break;
    }
}

void dcc_interaction_request_free(dcc_interaction_request_t *request) {
    if (request == NULL) {
        return;
    }
    free(request->header_block);
    free(request->body);
    free(request->response_content_type);
    free(request->response_body);
    free(request);
}
