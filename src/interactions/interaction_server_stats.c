#include "internal/interactions/dcc_interaction_server_internal.h"

#include <string.h>

dcc_status_t dcc_interaction_server_stats(
    const dcc_interaction_server_t *server,
    dcc_interaction_server_stats_t *out
) {
    if (server == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t size = out->size;
    memset(out, 0, sizeof(*out));
    out->size = size;
    out->accepted_connections = atomic_load_explicit(&server->accepted_connections, memory_order_acquire);
    out->active_requests = atomic_load_explicit(&server->active_requests, memory_order_acquire);
    out->completed_requests = atomic_load_explicit(&server->completed_requests, memory_order_acquire);
    out->read_errors = atomic_load_explicit(&server->read_errors, memory_order_acquire);
    out->write_errors = atomic_load_explicit(&server->write_errors, memory_order_acquire);
    out->spawn_errors = atomic_load_explicit(&server->spawn_errors, memory_order_acquire);
    out->response_2xx = atomic_load_explicit(&server->response_2xx, memory_order_acquire);
    out->response_3xx = atomic_load_explicit(&server->response_3xx, memory_order_acquire);
    out->response_4xx = atomic_load_explicit(&server->response_4xx, memory_order_acquire);
    out->response_5xx = atomic_load_explicit(&server->response_5xx, memory_order_acquire);
    out->bad_request_responses = atomic_load_explicit(&server->bad_request_responses, memory_order_acquire);
    out->unauthorized_responses = atomic_load_explicit(&server->unauthorized_responses, memory_order_acquire);
    out->not_found_responses = atomic_load_explicit(&server->not_found_responses, memory_order_acquire);
    out->method_not_allowed_responses =
        atomic_load_explicit(&server->method_not_allowed_responses, memory_order_acquire);
    out->payload_too_large_responses =
        atomic_load_explicit(&server->payload_too_large_responses, memory_order_acquire);
    return DCC_OK;
}

dcc_status_t dcc_interaction_server_get_state(
    const dcc_interaction_server_t *server,
    dcc_interaction_server_state_t *out
) {
    if (server == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_interaction_server_fill_state(server, out);
    return DCC_OK;
}
