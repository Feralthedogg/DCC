#ifndef DCC_HOT_RELOAD_WORKER_PROCESS_INTERNAL_H
#define DCC_HOT_RELOAD_WORKER_PROCESS_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_worker_types_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

void dcc_hot_reload_worker_process_close(dcc_hot_reload_worker_process_t *worker);
void dcc_hot_reload_worker_process_io_init(dcc_hot_reload_worker_process_t *worker);
void dcc_hot_reload_worker_process_io_deinit(dcc_hot_reload_worker_process_t *worker);
void dcc_hot_reload_worker_process_io_lock(dcc_hot_reload_worker_process_t *worker);
void dcc_hot_reload_worker_process_io_unlock(dcc_hot_reload_worker_process_t *worker);
uint8_t dcc_hot_reload_worker_process_is_alive(dcc_hot_reload_worker_process_t *worker);
void dcc_hot_reload_worker_process_stop(dcc_hot_reload_worker_process_t *worker, uint32_t timeout_ms);
dcc_status_t dcc_hot_reload_worker_process_spawn(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    dcc_hot_reload_worker_process_t *out,
    dcc_event_type_t *handler_types,
    size_t *handler_count,
    size_t handler_cap
);
#if !defined(_WIN32)
char *dcc_hot_reload_worker_resolve_exec_path_posix(
    const char *worker_path,
    char *error,
    size_t error_size
);
void dcc_hot_reload_worker_close_fd_posix(int *fd);
int dcc_hot_reload_worker_socketpair_posix(int fds[2]);
void dcc_hot_reload_worker_child_exec_posix(
    dcc_hot_reload_t *hot_reload,
    const char *exec_path,
    int in_fd,
    int out_fd,
    uint64_t generation
);
#else
void dcc_hot_reload_worker_close_fd_windows(int *fd);
int dcc_hot_reload_worker_pipe_windows(int fds[2]);
void dcc_hot_reload_worker_process_close_fds_windows(dcc_hot_reload_worker_process_t *worker);
intptr_t dcc_hot_reload_worker_spawn_child_windows(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    int *child_in_fd,
    int *child_out_fd
);
#endif
dcc_status_t dcc_hot_reload_worker_process_read_ready(
    dcc_hot_reload_worker_process_t *worker,
    dcc_event_type_t *handler_types,
    size_t *handler_count,
    size_t handler_cap,
    uint32_t timeout_ms
);
dcc_status_t dcc_hot_reload_worker_process_read_ready_direct(
    dcc_hot_reload_worker_process_t *worker,
    dcc_event_type_t *handler_types,
    size_t *handler_count,
    size_t handler_cap,
    uint32_t timeout_ms
);
dcc_status_t dcc_hot_reload_worker_process_send_event(
    dcc_hot_reload_worker_process_t *worker,
    const dcc_event_t *event,
    uint32_t timeout_ms,
    dcc_hot_reload_worker_result_set_t *out
);
dcc_status_t dcc_hot_reload_worker_process_healthcheck(
    dcc_hot_reload_worker_process_t *worker,
    uint32_t timeout_ms
);

#ifdef __cplusplus
}
#endif

#endif
