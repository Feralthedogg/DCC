#include "internal/hot_reload/dcc_hot_reload_internal.h"

#if defined(_WIN32)
#include <io.h>
#include <windows.h>

void dcc_hot_reload_worker_process_close_fds_windows(dcc_hot_reload_worker_process_t *worker) {
    if (worker == NULL) {
        return;
    }
    if (worker->in_fd >= 0) {
        _close(worker->in_fd);
        worker->in_fd = -1;
    }
    if (worker->out_fd >= 0) {
        _close(worker->out_fd);
        worker->out_fd = -1;
    }
}

void dcc_hot_reload_worker_process_close(dcc_hot_reload_worker_process_t *worker) {
    if (worker == NULL) {
        return;
    }
    dcc_hot_reload_worker_process_io_lock(worker);
    dcc_hot_reload_worker_process_close_fds_windows(worker);
    if (worker->process != 0) {
        CloseHandle((HANDLE)worker->process);
        worker->process = 0;
    }
    worker->loaded = 0U;
    dcc_hot_reload_worker_process_io_unlock(worker);
    dcc_hot_reload_worker_process_io_deinit(worker);
}

uint8_t dcc_hot_reload_worker_process_is_alive(dcc_hot_reload_worker_process_t *worker) {
    if (worker == NULL || !worker->loaded || worker->process == 0) {
        return 0U;
    }
    DWORD rc = WaitForSingleObject((HANDLE)worker->process, 0);
    if (rc == WAIT_TIMEOUT) {
        return 1U;
    }
    if (rc == WAIT_OBJECT_0) {
        worker->loaded = 0U;
    }
    return 0U;
}
#endif
