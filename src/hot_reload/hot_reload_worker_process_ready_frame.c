#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <errno.h>

dcc_status_t dcc_hot_reload_worker_process_read_ready_direct(
    dcc_hot_reload_worker_process_t *worker,
    dcc_event_type_t *handler_types,
    size_t *handler_count,
    size_t handler_cap,
    uint32_t timeout_ms
) {
    dcc_hot_reload_worker_header_t header;
    if (dcc_hot_reload_worker_read_header(worker->out_fd, &header, timeout_ms) != 0) {
        int read_errno = errno;
        if (read_errno == ETIMEDOUT && dcc_hot_reload_worker_process_is_alive(worker)) {
            return DCC_ERR_TIMEOUT;
        }
        if (read_errno == ETIMEDOUT) {
            dcc_hot_reload_copy_text(
                worker->last_error,
                sizeof(worker->last_error),
                "hot reload worker exited before READY"
            );
            return DCC_ERR_RUNTIME;
        }
        dcc_hot_reload_copy_text(
            worker->last_error,
            sizeof(worker->last_error),
            read_errno == EPROTO
                ? "hot reload worker sent invalid READY frame"
                : "hot reload worker exited before READY"
        );
        return DCC_ERR_RUNTIME;
    }
    if (header.kind != DCC_HOT_RELOAD_WORKER_MSG_READY ||
        header.size != sizeof(dcc_hot_reload_worker_ready_t)) {
        dcc_hot_reload_copy_text(
            worker->last_error,
            sizeof(worker->last_error),
            "hot reload worker sent invalid READY frame"
        );
        return DCC_ERR_RUNTIME;
    }
    dcc_hot_reload_worker_ready_t ready;
    if (dcc_hot_reload_worker_read_all_timeout(worker->out_fd, &ready, sizeof(ready), timeout_ms) != 0) {
        return DCC_ERR_RUNTIME;
    }
    worker->generation = ready.generation;
    dcc_hot_reload_copy_text(worker->last_error, sizeof(worker->last_error), ready.last_error);
    *handler_count = 0U;
    if (ready.status != DCC_OK) {
        return (dcc_status_t)ready.status;
    }
    if (ready.handler_count > handler_cap) {
        return DCC_ERR_STATE;
    }
    for (uint32_t i = 0; i < ready.handler_count; ++i) {
        int32_t raw_type = 0;
        if (dcc_hot_reload_worker_read_all_timeout(worker->out_fd, &raw_type, sizeof(raw_type), timeout_ms) != 0) {
            return DCC_ERR_RUNTIME;
        }
        if (raw_type < 0 || raw_type >= DCC_EVENT_MAX) {
            return DCC_ERR_STATE;
        }
        handler_types[i] = (dcc_event_type_t)raw_type;
    }
    *handler_count = ready.handler_count;
    return DCC_OK;
}
