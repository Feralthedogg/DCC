#include "internal/hot_reload/dcc_hot_reload_internal.h"

#if !defined(_WIN32)
#include <llam/runtime.h>

#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void dcc_worker_process_sleep_10ms(void) {
    if (llam_sleep_ns(UINT64_C(10000000)) != 0) {
        struct timespec req = {
            .tv_sec = 0,
            .tv_nsec = 10000000L,
        };
        while (nanosleep(&req, &req) != 0 && errno == EINTR) {
        }
    }
}

typedef struct dcc_worker_process_stop_call {
    dcc_hot_reload_worker_process_t *worker;
    uint32_t timeout_ms;
} dcc_worker_process_stop_call_t;

static void dcc_hot_reload_worker_process_stop_direct(
    dcc_hot_reload_worker_process_t *worker,
    uint32_t timeout_ms
) {
    if (worker == NULL) {
        return;
    }
    if (!worker->loaded) {
        dcc_hot_reload_worker_process_close(worker);
        return;
    }
    if (!dcc_hot_reload_worker_process_is_alive(worker)) {
        dcc_hot_reload_worker_process_close(worker);
        return;
    }
    dcc_hot_reload_worker_process_io_lock(worker);
    (void)dcc_hot_reload_worker_send_header(worker->in_fd, DCC_HOT_RELOAD_WORKER_MSG_STOP, 0);
    dcc_hot_reload_worker_process_io_unlock(worker);
    dcc_hot_reload_worker_process_close(worker);

    uint32_t waited_ms = 0;
    while (waited_ms <= timeout_ms) {
        int status = 0;
        pid_t rc = waitpid(worker->pid, &status, WNOHANG);
        if (rc == worker->pid || (rc < 0 && errno == ECHILD)) {
            return;
        }
        dcc_worker_process_sleep_10ms();
        waited_ms += 10U;
    }
    (void)kill(worker->pid, SIGTERM);
    (void)waitpid(worker->pid, NULL, 0);
}

static void *dcc_worker_process_stop_blocking(void *arg) {
    dcc_worker_process_stop_call_t *call = (dcc_worker_process_stop_call_t *)arg;
    dcc_hot_reload_worker_process_stop_direct(call->worker, call->timeout_ms);
    return NULL;
}

void dcc_hot_reload_worker_process_stop(dcc_hot_reload_worker_process_t *worker, uint32_t timeout_ms) {
    if (llam_current_task() == NULL) {
        dcc_hot_reload_worker_process_stop_direct(worker, timeout_ms);
        return;
    }

    dcc_worker_process_stop_call_t call = {
        .worker = worker,
        .timeout_ms = timeout_ms,
    };
    void *ignored = NULL;
    if (llam_call_blocking_result(dcc_worker_process_stop_blocking, &call, &ignored) != 0) {
        dcc_hot_reload_worker_process_stop_direct(worker, timeout_ms);
    }
}
#endif
