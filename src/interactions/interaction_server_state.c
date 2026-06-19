#include "internal/interactions/dcc_interaction_server_internal.h"

#include <string.h>

void dcc_interaction_server_fill_state(
    const dcc_interaction_server_t *server,
    dcc_interaction_server_state_t *out
) {
    size_t size = out->size;
    memset(out, 0, sizeof(*out));
    out->size = size;

    llam_fd_t listener = atomic_load_explicit(&server->listener_fd, memory_order_acquire);
    uint64_t active = atomic_load_explicit(&server->active_requests, memory_order_acquire);
    uint8_t stopping = atomic_load_explicit(&server->stopping, memory_order_acquire) ? 1U : 0U;
    uint8_t listener_open = LLAM_FD_IS_INVALID(listener) ? 0U : 1U;

    out->started = atomic_load_explicit(&server->started, memory_order_acquire) ? 1U : 0U;
    out->stopping = stopping;
    out->listening = listener_open && !stopping ? 1U : 0U;
    out->draining = stopping && active != 0U ? 1U : 0U;
    out->requested_port = server->requested_port;
    out->bound_port = server->bound_port;
    out->backlog = server->backlog;
    out->route_count = server->route_count;
    out->max_header_size = server->max_header_size;
    out->max_body_size = server->max_body_size;
    out->accepted_connections = atomic_load_explicit(&server->accepted_connections, memory_order_acquire);
    out->active_requests = active;
    out->completed_requests = atomic_load_explicit(&server->completed_requests, memory_order_acquire);
}
