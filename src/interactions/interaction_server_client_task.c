#include "internal/interactions/dcc_interaction_server_internal.h"

#include <llam/io.h>

#include <stdlib.h>

void dcc_interaction_client_task_run(void *arg) {
    dcc_interaction_client_task_t *task = (dcc_interaction_client_task_t *)arg;
    dcc_interaction_server_t *server = task->server;
    llam_fd_t fd = task->fd;
    free(task);

    atomic_fetch_add_explicit(&server->active_requests, 1U, memory_order_acq_rel);
    dcc_interaction_request_t *request = (dcc_interaction_request_t *)calloc(1U, sizeof(*request));
    if (request == NULL) {
        atomic_fetch_add_explicit(&server->read_errors, 1U, memory_order_relaxed);
        atomic_fetch_sub_explicit(&server->active_requests, 1U, memory_order_acq_rel);
        (void)llam_close(fd);
        return;
    }
    request->server = server;

    dcc_status_t st = dcc_interaction_read_request(server, fd, request);
    if (st != DCC_OK) {
        atomic_fetch_add_explicit(&server->read_errors, 1U, memory_order_relaxed);
    }
    if (st == DCC_OK) {
        st = dcc_interaction_handle_request(request);
    }
    if (st != DCC_OK && !request->response_set) {
        if (st == DCC_ERR_INVALID_ARG) {
            (void)dcc_interaction_request_reply_text(request, 413, "Payload too large");
        } else {
            (void)dcc_interaction_request_reply_text(request, 400, "Bad request");
        }
    }

    uint16_t response_status = request->response_set ? request->response_status : 500U;
    dcc_interaction_server_note_response(server, response_status);
    if (dcc_interaction_write_response(fd, request) != 0) {
        atomic_fetch_add_explicit(&server->write_errors, 1U, memory_order_relaxed);
    }
    atomic_fetch_add_explicit(&server->completed_requests, 1U, memory_order_release);
    atomic_fetch_sub_explicit(&server->active_requests, 1U, memory_order_acq_rel);
    (void)llam_close(fd);
    dcc_interaction_request_free(request);
}
