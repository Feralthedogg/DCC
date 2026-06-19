#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <llam/runtime.h>

#include <errno.h>

typedef struct dcc_worker_healthcheck_call {
    dcc_hot_reload_worker_process_t *worker;
    uint32_t timeout_ms;
    dcc_status_t status;
} dcc_worker_healthcheck_call_t;

static dcc_status_t dcc_worker_healthcheck_direct(
    dcc_hot_reload_worker_process_t *worker,
    uint32_t timeout_ms
) {
    if (worker == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_hot_reload_worker_process_io_lock(worker);
    if (dcc_hot_reload_worker_send_header(
            worker->in_fd,
            DCC_HOT_RELOAD_WORKER_MSG_HEALTH,
            0U
        ) != 0) {
        dcc_hot_reload_worker_process_io_unlock(worker);
        return DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_worker_header_t header;
    if (dcc_hot_reload_worker_read_header(worker->out_fd, &header, timeout_ms) != 0) {
        dcc_status_t status = errno == ETIMEDOUT ? DCC_ERR_TIMEOUT : DCC_ERR_RUNTIME;
        dcc_hot_reload_worker_process_io_unlock(worker);
        return status;
    }
    if (header.kind != DCC_HOT_RELOAD_WORKER_MSG_HEALTH_RESULT ||
        header.size != sizeof(dcc_hot_reload_worker_health_t)) {
        errno = EPROTO;
        dcc_hot_reload_worker_process_io_unlock(worker);
        return DCC_ERR_RUNTIME;
    }

    dcc_hot_reload_worker_health_t health;
    if (dcc_hot_reload_worker_read_all_timeout(worker->out_fd, &health, sizeof(health), timeout_ms) != 0) {
        dcc_hot_reload_worker_process_io_unlock(worker);
        return DCC_ERR_RUNTIME;
    }
    if (health.generation != 0U) {
        worker->generation = health.generation;
    }

    dcc_status_t status = (dcc_status_t)health.status;
    dcc_hot_reload_worker_process_io_unlock(worker);
    return status;
}

static void *dcc_worker_healthcheck_blocking(void *arg) {
    dcc_worker_healthcheck_call_t *call = (dcc_worker_healthcheck_call_t *)arg;
    call->status = dcc_worker_healthcheck_direct(call->worker, call->timeout_ms);
    return NULL;
}

dcc_status_t dcc_hot_reload_worker_process_healthcheck(
    dcc_hot_reload_worker_process_t *worker,
    uint32_t timeout_ms
) {
    if (llam_current_task() == NULL) {
        return dcc_worker_healthcheck_direct(worker, timeout_ms);
    }

    dcc_worker_healthcheck_call_t call = {
        .worker = worker,
        .timeout_ms = timeout_ms,
        .status = DCC_ERR_RUNTIME,
    };
    void *ignored = NULL;
    if (llam_call_blocking_result(dcc_worker_healthcheck_blocking, &call, &ignored) != 0) {
        return errno == ECANCELED ? DCC_ERR_CANCELED : DCC_ERR_RUNTIME;
    }
    return call.status;
}
