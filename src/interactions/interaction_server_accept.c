#include "internal/interactions/dcc_interaction_server_internal.h"

#include <llam/io.h>

#include <errno.h>
#include <stdlib.h>

#if LLAM_PLATFORM_POSIX
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

void dcc_interaction_accept_task(void *arg) {
    dcc_interaction_server_t *server = (dcc_interaction_server_t *)arg;

    while (!atomic_load_explicit(&server->stopping, memory_order_acquire)) {
        llam_fd_t listener = atomic_load_explicit(&server->listener_fd, memory_order_acquire);
        if (LLAM_FD_IS_INVALID(listener)) {
            break;
        }

        struct sockaddr_storage peer;
        socklen_t peer_len = (socklen_t)sizeof(peer);
        llam_fd_t fd = llam_accept(listener, (struct sockaddr *)&peer, &peer_len);
        if (LLAM_FD_IS_INVALID(fd)) {
            if (atomic_load_explicit(&server->stopping, memory_order_acquire) ||
                errno == EBADF || errno == ECANCELED) {
                break;
            }
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            dcc_interaction_server_set_error(server, "failed to accept interaction connection");
            continue;
        }
        atomic_fetch_add_explicit(&server->accepted_connections, 1U, memory_order_relaxed);

        dcc_interaction_client_task_t *task = (dcc_interaction_client_task_t *)malloc(sizeof(*task));
        if (task == NULL) {
            atomic_fetch_add_explicit(&server->spawn_errors, 1U, memory_order_relaxed);
            (void)llam_close(fd);
            continue;
        }
        task->server = server;
        task->fd = fd;
        if (dcc_runtime_spawn(&server->runtime, dcc_interaction_client_task_run, task) != DCC_OK) {
            free(task);
            (void)llam_close(fd);
            atomic_fetch_add_explicit(&server->spawn_errors, 1U, memory_order_relaxed);
            dcc_interaction_server_set_error(server, "failed to spawn interaction connection task");
        }
    }
}
