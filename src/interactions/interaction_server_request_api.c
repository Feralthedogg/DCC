#include "internal/interactions/dcc_interaction_server_internal.h"

const char *dcc_interaction_request_method(const dcc_interaction_request_t *request) {
    return request != NULL ? request->method : "";
}

const char *dcc_interaction_request_path(const dcc_interaction_request_t *request) {
    return request != NULL ? request->path : "";
}

const char *dcc_interaction_request_header(const dcc_interaction_request_t *request, const char *name) {
    if (request == NULL || name == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < request->header_count; ++i) {
        if (dcc_interaction_ascii_eq(request->headers[i].name, name)) {
            return request->headers[i].value;
        }
    }
    return NULL;
}

const dcc_interaction_http_header_t *dcc_interaction_request_headers(
    const dcc_interaction_request_t *request,
    size_t *out_count
) {
    if (out_count != NULL) {
        *out_count = request != NULL ? request->header_count : 0U;
    }
    return request != NULL ? request->headers : NULL;
}

const char *dcc_interaction_request_body(const dcc_interaction_request_t *request) {
    return request != NULL ? request->body : NULL;
}

size_t dcc_interaction_request_body_len(const dcc_interaction_request_t *request) {
    return request != NULL ? request->body_len : 0U;
}

const dcc_interaction_t *dcc_interaction_request_interaction(const dcc_interaction_request_t *request) {
    if (request == NULL || !request->parsed || request->payload == NULL || !request->payload->has_interaction) {
        return NULL;
    }
    return &request->payload->interaction;
}

dcc_event_type_t dcc_interaction_request_event_type(const dcc_interaction_request_t *request) {
    return request != NULL ? request->event_type : DCC_EVENT_INTERACTION_CREATE;
}

uint8_t dcc_interaction_request_is_ping(const dcc_interaction_request_t *request) {
    const dcc_interaction_t *interaction = dcc_interaction_request_interaction(request);
    return interaction != NULL && interaction->type == 1U ? 1U : 0U;
}

uint64_t dcc_interaction_request_elapsed_ms(const dcc_interaction_request_t *request) {
    if (request == NULL || request->started_at_ns == 0U) {
        return 0U;
    }
    uint64_t now_ns = llam_now_ns();
    return now_ns > request->started_at_ns
        ? (now_ns - request->started_at_ns) / UINT64_C(1000000)
        : 0U;
}

uint64_t dcc_interaction_request_deadline_remaining_ms(const dcc_interaction_request_t *request) {
    if (request == NULL || request->deadline_at_ns == 0U) {
        return 0U;
    }
    uint64_t now_ns = llam_now_ns();
    return now_ns < request->deadline_at_ns
        ? (request->deadline_at_ns - now_ns) / UINT64_C(1000000)
        : 0U;
}
