#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_request_handle_internal.h"
#include "internal/rest/dcc_rest_sensitive_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_rest_async_request_free(dcc_rest_async_request_t *request) {
    if (request == NULL) {
        return;
    }
    free(request->method);
    free(request->operation);
    dcc_rest_sensitive_free(
        request->wire_path,
        request->wire_path != NULL ? strlen(request->wire_path) + 1U : 0U
    );
    if ((request->flags & DCC_REST_CALL_FLAG_SENSITIVE_REQUEST_BODY) != 0U) {
        dcc_rest_sensitive_free(request->body, request->body_len);
    } else {
        free(request->body);
    }
    free(request->content_type);
    dcc_rest_sensitive_free(
        request->audit_log_reason,
        request->audit_log_reason != NULL
            ? strlen(request->audit_log_reason) + 1U
            : 0U
    );
    dcc_rest_sensitive_free(
        request->auth_token,
        request->auth_token != NULL ? strlen(request->auth_token) + 1U : 0U
    );
    dcc_rest_request_handle_release(request->request_handle);
    dcc_endpoint_secure_zero(request, sizeof(*request));
    free(request);
}

uint32_t dcc_rest_async_concurrency(const dcc_client_t *client) {
    uint32_t concurrency = client->rest_concurrency != 0 ? client->rest_concurrency : 1U;
    if (concurrency > DCC_REST_BUCKET_CAP) {
        concurrency = DCC_REST_BUCKET_CAP;
    }
    return concurrency != 0 ? concurrency : 1U;
}

int dcc_rest_priority_valid(dcc_rest_priority_t priority) {
    return priority == DCC_REST_PRIORITY_LOW ||
        priority == DCC_REST_PRIORITY_NORMAL ||
        priority == DCC_REST_PRIORITY_HIGH;
}

int dcc_rest_async_request_canceled(void *user_data) {
    dcc_rest_async_request_t *request = (dcc_rest_async_request_t *)user_data;
    return request != NULL && atomic_load_explicit(&request->cancel_requested, memory_order_acquire);
}

llam_fd_t dcc_rest_async_request_swap_fd(void *user_data, llam_fd_t fd) {
    dcc_rest_async_request_t *request = (dcc_rest_async_request_t *)user_data;
    if (request == NULL) {
        return LLAM_INVALID_FD;
    }
    return atomic_exchange_explicit(&request->active_fd, fd, memory_order_acq_rel);
}

int dcc_rest_async_request_cancel(dcc_rest_async_request_t *request, llam_fd_t *fds, size_t *fd_count) {
    if (request == NULL) {
        return 0;
    }
    bool expected = false;
    if (!atomic_compare_exchange_strong_explicit(
            &request->cancel_requested,
            &expected,
            true,
            memory_order_acq_rel,
            memory_order_acquire
        )) {
        return 0;
    }
    llam_fd_t fd = atomic_exchange_explicit(&request->active_fd, LLAM_INVALID_FD, memory_order_acq_rel);
    if (!LLAM_FD_IS_INVALID(fd) && fds != NULL && fd_count != NULL && *fd_count < DCC_REST_BUCKET_CAP) {
        fds[*fd_count] = fd;
        (*fd_count)++;
    }
    return 1;
}
