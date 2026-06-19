#ifndef DCC_INTERACTION_SERVER_STATE_INTERNAL_H
#define DCC_INTERACTION_SERVER_STATE_INTERNAL_H

#include "internal/interactions/dcc_interaction_route_internal.h"
#include "internal/runtime/dcc_runtime_internal.h"

#include <dcc/interaction_server.h>

#include <llam/io.h>

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct dcc_interaction_server {
    dcc_runtime_t runtime;
    char *public_key_hex;
    char *address;
    uint16_t requested_port;
    uint16_t bound_port;
    uint32_t backlog;
    size_t max_header_size;
    size_t max_body_size;
    dcc_interaction_server_cb callback;
    void *user_data;
    dcc_interaction_route_entry_t *routes;
    size_t route_count;
    size_t route_cap;
    _Atomic(llam_fd_t) listener_fd;
    atomic_bool started;
    atomic_bool stopping;
    atomic_uint_fast64_t accepted_connections;
    atomic_uint_fast64_t active_requests;
    atomic_uint_fast64_t completed_requests;
    atomic_uint_fast64_t read_errors;
    atomic_uint_fast64_t write_errors;
    atomic_uint_fast64_t spawn_errors;
    atomic_uint_fast64_t response_2xx;
    atomic_uint_fast64_t response_3xx;
    atomic_uint_fast64_t response_4xx;
    atomic_uint_fast64_t response_5xx;
    atomic_uint_fast64_t bad_request_responses;
    atomic_uint_fast64_t unauthorized_responses;
    atomic_uint_fast64_t not_found_responses;
    atomic_uint_fast64_t method_not_allowed_responses;
    atomic_uint_fast64_t payload_too_large_responses;
    char last_error[256];
};

#endif
