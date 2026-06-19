#include "internal/hot_reload/dcc_hot_reload_internal.h"

#if !defined(_WIN32)
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>

void dcc_hot_reload_worker_process_close(dcc_hot_reload_worker_process_t *worker) {
    if (worker == NULL) {
        return;
    }
    dcc_hot_reload_worker_process_io_lock(worker);
    if (worker->in_fd >= 0) {
        close(worker->in_fd);
        worker->in_fd = -1;
    }
    if (worker->out_fd >= 0) {
        close(worker->out_fd);
        worker->out_fd = -1;
    }
    worker->loaded = 0;
    dcc_hot_reload_worker_process_io_unlock(worker);
    dcc_hot_reload_worker_process_io_deinit(worker);
}

uint8_t dcc_hot_reload_worker_process_is_alive(dcc_hot_reload_worker_process_t *worker) {
    if (worker == NULL || !worker->loaded) {
        return 0U;
    }
    int status = 0;
    pid_t rc = waitpid(worker->pid, &status, WNOHANG);
    if (rc == 0) {
        return 1U;
    }
    if (rc == worker->pid || (rc < 0 && errno == ECHILD)) {
        worker->loaded = 0U;
        return 0U;
    }
    return 1U;
}
#endif
