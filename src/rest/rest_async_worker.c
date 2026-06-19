#include "internal/rest/dcc_rest_async_retry_internal.h"
#include "internal/rest/dcc_rest_async_worker_lifecycle_internal.h"
#include "internal/rest/dcc_rest_async_worker_task_internal.h"
#include "internal/rest/dcc_rest_capture_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_request_raw_internal.h"

#include <string.h>

void dcc_rest_async_worker_task(void *arg) {
    dcc_rest_async_request_t *request = (dcc_rest_async_request_t *)arg;
    if (request == NULL) {
        return;
    }

    dcc_client_t *client = request->client;
    dcc_status_t status = DCC_OK;
    dcc_rest_captured_response_t captured;
    memset(&captured, 0, sizeof(captured));
    if (atomic_load_explicit(&request->cancel_requested, memory_order_acquire)) {
        status = DCC_ERR_CANCELED;
    } else {
        size_t body_len = request->body != NULL ? strlen(request->body) : 0U;
        status = dcc_rest_request_raw_impl(
            client,
            request->method,
            request->path,
            request->body,
            body_len,
            body_len != 0U ? "application/json" : NULL,
            0,
            0,
            dcc_rest_capture_cb,
            &captured,
            dcc_rest_async_request_canceled,
            dcc_rest_async_request_swap_fd,
            request
        );
    }

    if (atomic_load_explicit(&request->cancel_requested, memory_order_acquire)) {
        status = DCC_ERR_CANCELED;
    }

    if (status == DCC_OK &&
        captured.called &&
        captured.error == DCC_ERR_RATE_LIMITED &&
        request->retry_attempt < DCC_REST_MAX_RATE_LIMIT_RETRIES &&
        !atomic_load_explicit(&request->cancel_requested, memory_order_acquire)) {
        request->retry_attempt++;
        uint64_t not_before_ms = dcc_rest_async_retry_not_before_ms(request);
        dcc_rest_captured_response_deinit(&captured);
        if (dcc_rest_async_requeue_retry(request, not_before_ms)) {
            return;
        }
        status = DCC_ERR_CANCELED;
    }

    if (status == DCC_OK && captured.called && !request->callback_called && request->cb != NULL) {
        request->callback_called = 1;
        dcc_rest_forward_captured_response(client, &captured, captured.error, request->cb, request->user_data);
    } else if (status != DCC_OK && !request->callback_called && request->cb != NULL) {
        dcc_rest_response_t response = {
            .size = sizeof(response),
            .status = 0,
            .error = status,
            .body = NULL,
            .body_len = 0,
        };
        request->callback_called = 1;
        request->cb(client, &response, request->user_data);
    }

    (void)atomic_exchange_explicit(&request->active_fd, LLAM_INVALID_FD, memory_order_acq_rel);
    dcc_rest_captured_response_deinit(&captured);
    dcc_rest_async_complete(request);
    dcc_rest_async_request_free(request);
}
