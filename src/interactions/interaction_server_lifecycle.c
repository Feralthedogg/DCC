#include "internal/interactions/dcc_interaction_server_internal.h"

#include <llam/io.h>

#include <stdbool.h>

void dcc_interaction_server_close_listener(dcc_interaction_server_t *server) {
    if (server == NULL) {
        return;
    }
    llam_fd_t fd = atomic_exchange_explicit(&server->listener_fd, LLAM_INVALID_FD, memory_order_acq_rel);
    if (!LLAM_FD_IS_INVALID(fd)) {
        (void)llam_close(fd);
    }
}

dcc_status_t dcc_interaction_server_start(dcc_interaction_server_t *server) {
    if (server == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    bool expected = false;
    if (!atomic_compare_exchange_strong(&server->started, &expected, true)) {
        dcc_interaction_server_set_error(server, "interaction server is already started");
        return DCC_ERR_STATE;
    }
    atomic_store_explicit(&server->stopping, false, memory_order_release);
    dcc_interaction_server_set_error(server, NULL);

    dcc_status_t st = dcc_runtime_init(&server->runtime);
    if (st != DCC_OK) {
        atomic_store_explicit(&server->started, false, memory_order_release);
        dcc_interaction_server_set_error(server, "failed to initialize LLAM runtime");
        return st;
    }

    llam_fd_t fd = LLAM_INVALID_FD;
    st = dcc_interaction_bind_listener(server, &fd);
    if (st != DCC_OK) {
        atomic_store_explicit(&server->started, false, memory_order_release);
        dcc_runtime_shutdown(&server->runtime);
        dcc_interaction_server_set_error(server, "failed to bind interaction server listener");
        return st;
    }
    atomic_store_explicit(&server->listener_fd, fd, memory_order_release);

    st = dcc_runtime_spawn(&server->runtime, dcc_interaction_accept_task, server);
    if (st != DCC_OK) {
        (void)llam_close(atomic_exchange_explicit(&server->listener_fd, LLAM_INVALID_FD, memory_order_acq_rel));
        atomic_store_explicit(&server->started, false, memory_order_release);
        dcc_runtime_shutdown(&server->runtime);
        dcc_interaction_server_set_error(server, "failed to spawn interaction accept task");
        return st;
    }
    return DCC_OK;
}
