#include "internal/rest/dcc_rest_async_retry_internal.h"
#include "internal/rest/dcc_rest_async_worker_lifecycle_internal.h"
#include "internal/rest/dcc_rest_async_worker_task_internal.h"
#include "internal/rest/dcc_rest_capture_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"
#include "internal/rest/dcc_rest_request_handle_internal.h"
#include "internal/rest/dcc_rest_request_raw_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

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
    captured.expected_client = client;
    if (atomic_load_explicit(&request->cancel_requested, memory_order_acquire)) {
        status = DCC_ERR_CANCELED;
    } else {
        status = dcc_rest_request_raw_impl(
            client,
            request->method,
            request->path,
            request->body,
            request->body_len,
            request->content_type,
            0,
            0,
            dcc_rest_capture_cb,
            &captured,
            dcc_rest_async_request_canceled,
            dcc_rest_async_request_swap_fd,
            request,
            0,
            0
        );
    }

    if (atomic_load_explicit(&request->cancel_requested, memory_order_acquire)) {
        status = DCC_ERR_CANCELED;
    }

    if (status == DCC_OK && captured.called && captured.copy_error != DCC_OK) {
        status = captured.copy_error;
    }
    if (status == DCC_OK && captured.called && captured.status == 0U) {
        status = captured.error != DCC_OK ? captured.error : DCC_ERR_RUNTIME;
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

    if (status == DCC_OK && !captured.called) {
        status = DCC_ERR_RUNTIME;
    }

    if (!request->callback_called) {
        uint64_t retry_after_ms = 0U;
        if (status == DCC_OK && captured.called &&
            captured.error == DCC_ERR_RATE_LIMITED) {
            uint64_t now_ms = dcc_rest_now_ms();
            uint64_t not_before_ms = dcc_rest_async_retry_not_before_ms(request);
            retry_after_ms = not_before_ms > now_ms ? not_before_ms - now_ms : 1U;
        }
        dcc_rest_terminal_completion_t completion = {
            .operation = request->path,
            .transport_status = status,
            .http_status = status == DCC_OK ? captured.status : 0U,
            .legacy_error = status == DCC_OK ? captured.error : status,
            .body = status == DCC_OK ? captured.body : NULL,
            .body_len = status == DCC_OK ? captured.body_len : 0U,
            .retry_after_ms = retry_after_ms,
        };
        request->callback_called = 1;
        if (request->request_handle != NULL) {
            dcc_rest_request_handle_finalize(request->request_handle, &completion);
        } else {
            dcc_rest_deliver_terminal(
                client,
                &completion,
                request->cb,
                request->user_data
            );
        }
    }

    (void)atomic_exchange_explicit(&request->active_fd, LLAM_INVALID_FD, memory_order_acq_rel);
    dcc_rest_captured_response_deinit(&captured);
    dcc_rest_async_complete(request);
    dcc_rest_async_request_free(request);
}
