#include "internal/hot_reload/dcc_hot_reload_internal.h"

#if !defined(_WIN32)
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

dcc_status_t dcc_hot_reload_worker_process_spawn(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    dcc_hot_reload_worker_process_t *out,
    dcc_event_type_t *handler_types,
    size_t *handler_count,
    size_t handler_cap
) {
    if (hot_reload == NULL || out == NULL || handler_types == NULL || handler_count == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    char *exec_path = dcc_hot_reload_worker_resolve_exec_path_posix(
        hot_reload->worker_path,
        out->last_error,
        sizeof(out->last_error)
    );
    if (exec_path == NULL) {
        return DCC_ERR_RUNTIME;
    }
    int to_child[2] = { -1, -1 };
    int from_child[2] = { -1, -1 };
    if (dcc_hot_reload_worker_socketpair_posix(to_child) != 0 ||
        dcc_hot_reload_worker_socketpair_posix(from_child) != 0) {
        dcc_hot_reload_worker_close_fd_posix(&to_child[0]);
        dcc_hot_reload_worker_close_fd_posix(&to_child[1]);
        dcc_hot_reload_worker_close_fd_posix(&from_child[0]);
        dcc_hot_reload_worker_close_fd_posix(&from_child[1]);
        free(exec_path);
        return DCC_ERR_RUNTIME;
    }

    pid_t pid = fork();
    if (pid < 0) {
        dcc_hot_reload_worker_close_fd_posix(&to_child[0]);
        dcc_hot_reload_worker_close_fd_posix(&to_child[1]);
        dcc_hot_reload_worker_close_fd_posix(&from_child[0]);
        dcc_hot_reload_worker_close_fd_posix(&from_child[1]);
        free(exec_path);
        return DCC_ERR_RUNTIME;
    }
    if (pid == 0) {
        close(to_child[1]);
        close(from_child[0]);
        dcc_hot_reload_worker_child_exec_posix(hot_reload, exec_path, to_child[0], from_child[1], generation);
    }
    free(exec_path);

    close(to_child[0]);
    close(from_child[1]);
    memset(out, 0, sizeof(*out));
    out->pid = pid;
    out->in_fd = to_child[1];
    out->out_fd = from_child[0];
    out->process_id = (uint64_t)pid;
    out->loaded = 1U;
    dcc_hot_reload_worker_process_io_init(out);
    *handler_count = 0U;

    dcc_status_t status = dcc_hot_reload_worker_process_read_ready(
        out,
        handler_types,
        handler_count,
        handler_cap,
        hot_reload->worker_health_timeout_ms
    );
    if (status != DCC_OK) {
        dcc_hot_reload_worker_process_stop(out, hot_reload->worker_drain_timeout_ms);
    }
    return status;
}
#endif
