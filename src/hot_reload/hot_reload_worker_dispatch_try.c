#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <string.h>

static dcc_status_t dcc_worker_execute_rest(
    dcc_hot_reload_t *hot_reload,
    const dcc_hot_reload_worker_result_set_t *result
) {
    dcc_status_t status = result != NULL ? result->status : DCC_ERR_RUNTIME;
    for (size_t i = 0; result != NULL && i < result->count; ++i) {
        const dcc_hot_reload_worker_rest_entry_t *entry = &result->items[i];
        dcc_status_t rest_status = dcc_rest_request_raw(
            hot_reload->client,
            entry->method,
            entry->path,
            entry->body_len != 0U ? entry->body : NULL,
            entry->body_len,
            entry->content_type != NULL && entry->content_type[0] != '\0'
                ? entry->content_type
                : NULL,
            NULL,
            NULL
        );
        if (status == DCC_OK && rest_status != DCC_OK) {
            status = rest_status;
        }
    }
    return status;
}

dcc_status_t dcc_hot_reload_worker_dispatch_try_event(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_worker_process_t *worker,
    const dcc_event_t *event,
    uint8_t *worker_failed
) {
    dcc_hot_reload_worker_result_set_t result;
    memset(&result, 0, sizeof(result));
    if (!dcc_hot_reload_worker_process_is_alive(worker)) {
        if (worker_failed != NULL) {
            *worker_failed = 1U;
        }
        return DCC_ERR_RUNTIME;
    }
    dcc_status_t status = dcc_hot_reload_worker_process_send_event(
        worker,
        event,
        hot_reload->worker_health_timeout_ms,
        &result
    );
    if (status != DCC_OK && worker_failed != NULL) {
        *worker_failed = 1U;
    }
    if (status == DCC_OK) {
        status = dcc_worker_execute_rest(hot_reload, &result);
    }
    dcc_hot_reload_worker_result_set_deinit(&result);
    return status;
}
