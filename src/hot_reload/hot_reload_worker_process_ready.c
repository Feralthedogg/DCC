#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <llam/runtime.h>

#include <errno.h>

typedef struct dcc_worker_read_ready_call {
    dcc_hot_reload_worker_process_t *worker;
    dcc_event_type_t *handler_types;
    size_t *handler_count;
    size_t handler_cap;
    uint32_t timeout_ms;
    dcc_status_t status;
} dcc_worker_read_ready_call_t;

static void *dcc_worker_process_read_ready_blocking(void *arg) {
    dcc_worker_read_ready_call_t *call = (dcc_worker_read_ready_call_t *)arg;
    call->status = dcc_hot_reload_worker_process_read_ready_direct(
        call->worker,
        call->handler_types,
        call->handler_count,
        call->handler_cap,
        call->timeout_ms
    );
    return NULL;
}

dcc_status_t dcc_hot_reload_worker_process_read_ready(
    dcc_hot_reload_worker_process_t *worker,
    dcc_event_type_t *handler_types,
    size_t *handler_count,
    size_t handler_cap,
    uint32_t timeout_ms
) {
    if (llam_current_task() == NULL) {
        return dcc_hot_reload_worker_process_read_ready_direct(
            worker,
            handler_types,
            handler_count,
            handler_cap,
            timeout_ms
        );
    }

    dcc_worker_read_ready_call_t call = {
        .worker = worker,
        .handler_types = handler_types,
        .handler_count = handler_count,
        .handler_cap = handler_cap,
        .timeout_ms = timeout_ms,
        .status = DCC_ERR_RUNTIME,
    };
    void *ignored = NULL;
    if (llam_call_blocking_result(dcc_worker_process_read_ready_blocking, &call, &ignored) != 0) {
        return errno == ECANCELED ? DCC_ERR_CANCELED : DCC_ERR_RUNTIME;
    }
    return call.status;
}
