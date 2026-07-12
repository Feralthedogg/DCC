#include "internal/interactions/dcc_interaction_server_internal.h"

#include <stdlib.h>

dcc_status_t dcc_interaction_server_create(
    const dcc_interaction_server_options_t *options,
    dcc_interaction_server_t **out
) {
    if (options == NULL || out == NULL ||
        options->size < offsetof(dcc_interaction_server_options_t, public_key_hex) + sizeof(options->public_key_hex) ||
        options->public_key_hex == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    unsigned char public_key[32];
    if (dcc_interaction_hex_decode_exact(options->public_key_hex, public_key, sizeof(public_key)) != 0) {
        return DCC_ERR_INVALID_ARG;
    }

#define DCC_INTERACTION_OPT_HAS(field) \
    (options->size >= offsetof(dcc_interaction_server_options_t, field) + sizeof(options->field))

    dcc_interaction_server_t *server = (dcc_interaction_server_t *)calloc(1U, sizeof(*server));
    if (server == NULL) {
        return DCC_ERR_NOMEM;
    }
    server->public_key_hex = dcc_strdup(options->public_key_hex);
    server->address = dcc_strdup(
        DCC_INTERACTION_OPT_HAS(address) && options->address != NULL ? options->address : "127.0.0.1"
    );
    if (server->public_key_hex == NULL || server->address == NULL) {
        dcc_interaction_server_destroy(server);
        return DCC_ERR_NOMEM;
    }

    server->requested_port = DCC_INTERACTION_OPT_HAS(port) ? options->port : 0U;
    server->backlog = DCC_INTERACTION_OPT_HAS(backlog) && options->backlog != 0U
        ? options->backlog
        : DCC_INTERACTION_DEFAULT_BACKLOG;
    server->max_header_size = DCC_INTERACTION_OPT_HAS(max_header_size) && options->max_header_size != 0U
        ? options->max_header_size
        : DCC_INTERACTION_DEFAULT_MAX_HEADER_SIZE;
    server->max_body_size = DCC_INTERACTION_OPT_HAS(max_body_size) && options->max_body_size != 0U
        ? options->max_body_size
        : DCC_INTERACTION_DEFAULT_MAX_BODY_SIZE;
    server->max_active_requests = DCC_INTERACTION_OPT_HAS(max_active_requests) &&
        options->max_active_requests != 0U
        ? options->max_active_requests
        : DCC_INTERACTION_DEFAULT_MAX_ACTIVE_REQUESTS;
    server->response_deadline_ms = DCC_INTERACTION_OPT_HAS(response_deadline_ms) &&
        options->response_deadline_ms != 0U
        ? options->response_deadline_ms
        : DCC_INTERACTION_DEFAULT_RESPONSE_DEADLINE_MS;
    server->replay_window_ms = DCC_INTERACTION_OPT_HAS(replay_window_ms) &&
        options->replay_window_ms != 0U
        ? options->replay_window_ms
        : DCC_INTERACTION_DEFAULT_REPLAY_WINDOW_MS;
    server->callback = DCC_INTERACTION_OPT_HAS(callback) ? options->callback : NULL;
    server->user_data = DCC_INTERACTION_OPT_HAS(user_data) ? options->user_data : NULL;
    atomic_init(&server->listener_fd, LLAM_INVALID_FD);
    atomic_init(&server->started, false);
    atomic_init(&server->stopping, false);
    atomic_init(&server->accepted_connections, 0);
    atomic_init(&server->active_requests, 0);
    atomic_init(&server->completed_requests, 0);
    atomic_init(&server->read_errors, 0);
    atomic_init(&server->write_errors, 0);
    atomic_init(&server->spawn_errors, 0);
    atomic_init(&server->response_2xx, 0);
    atomic_init(&server->response_3xx, 0);
    atomic_init(&server->response_4xx, 0);
    atomic_init(&server->response_5xx, 0);
    atomic_init(&server->bad_request_responses, 0);
    atomic_init(&server->unauthorized_responses, 0);
    atomic_init(&server->not_found_responses, 0);
    atomic_init(&server->method_not_allowed_responses, 0);
    atomic_init(&server->payload_too_large_responses, 0);
    atomic_init(&server->overloaded_responses, 0);
    atomic_init(&server->replayed_requests, 0);
    atomic_init(&server->deadline_exceeded_requests, 0);
    atomic_flag_clear(&server->replay_lock);
    *out = server;
#undef DCC_INTERACTION_OPT_HAS
    return DCC_OK;
}

void dcc_interaction_server_destroy(dcc_interaction_server_t *server) {
    if (server == NULL) {
        return;
    }
    (void)dcc_interaction_server_stop(server);
    dcc_runtime_shutdown(&server->runtime);
    dcc_interaction_server_routes_free(server);
    free(server->public_key_hex);
    free(server->address);
    free(server);
}

uint16_t dcc_interaction_server_port(const dcc_interaction_server_t *server) {
    return server != NULL ? server->bound_port : 0U;
}

const char *dcc_interaction_server_last_error(const dcc_interaction_server_t *server) {
    return server != NULL ? server->last_error : "";
}
