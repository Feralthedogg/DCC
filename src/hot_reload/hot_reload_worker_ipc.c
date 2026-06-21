#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <llam/runtime.h>

#include <errno.h>

typedef struct dcc_worker_send_event_call {
    dcc_hot_reload_worker_process_t *worker;
    const dcc_event_t *event;
    uint32_t timeout_ms;
    dcc_hot_reload_worker_result_set_t *out;
    dcc_status_t status;
} dcc_worker_send_event_call_t;

static dcc_status_t dcc_worker_send_event_direct(
    dcc_hot_reload_worker_process_t *worker,
    const dcc_event_t *event,
    uint32_t timeout_ms,
    dcc_hot_reload_worker_result_set_t *out
) {
    if (worker == NULL || event == NULL || out == NULL ||
        event->raw.json == NULL || event->raw.json_len == 0U ||
        event->raw.json_len > UINT64_MAX ||
        event->raw.json_len > DCC_HOT_RELOAD_WORKER_MAX_EVENT_JSON_LEN) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_hot_reload_worker_event_t message = {
        .type = (int32_t)event->type,
        .shard_id = event->shard_id,
        .json_len = (uint64_t)event->raw.json_len,
    };
    dcc_hot_reload_worker_process_io_lock(worker);
    if (dcc_hot_reload_worker_send_header_timeout(
            worker->in_fd,
            DCC_HOT_RELOAD_WORKER_MSG_EVENT,
            sizeof(message),
            timeout_ms
        ) != 0 ||
        dcc_hot_reload_worker_write_all_timeout(worker->in_fd, &message, sizeof(message), timeout_ms) != 0 ||
        dcc_hot_reload_worker_write_all_timeout(worker->in_fd, event->raw.json, event->raw.json_len, timeout_ms) != 0) {
        dcc_status_t status = errno == ETIMEDOUT ? DCC_ERR_TIMEOUT : DCC_ERR_RUNTIME;
        dcc_hot_reload_worker_process_io_unlock(worker);
        return status;
    }
    dcc_status_t status = dcc_hot_reload_worker_result_set_read(worker, timeout_ms, out);
    dcc_hot_reload_worker_process_io_unlock(worker);
    return status;
}

static void *dcc_worker_send_event_blocking(void *arg) {
    dcc_worker_send_event_call_t *call = (dcc_worker_send_event_call_t *)arg;
    call->status = dcc_worker_send_event_direct(
        call->worker,
        call->event,
        call->timeout_ms,
        call->out
    );
    return NULL;
}

dcc_status_t dcc_hot_reload_worker_process_send_event(
    dcc_hot_reload_worker_process_t *worker,
    const dcc_event_t *event,
    uint32_t timeout_ms,
    dcc_hot_reload_worker_result_set_t *out
) {
    if (llam_current_task() == NULL) {
        return dcc_worker_send_event_direct(worker, event, timeout_ms, out);
    }

    dcc_worker_send_event_call_t call = {
        .worker = worker,
        .event = event,
        .timeout_ms = timeout_ms,
        .out = out,
        .status = DCC_ERR_RUNTIME,
    };
    void *ignored = NULL;
    if (llam_call_blocking_result(dcc_worker_send_event_blocking, &call, &ignored) != 0) {
        return errno == ECANCELED ? DCC_ERR_CANCELED : DCC_ERR_RUNTIME;
    }
    return call.status;
}
