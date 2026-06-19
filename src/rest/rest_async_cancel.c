#include "internal/rest/dcc_rest_async_cancel_internal.h"

dcc_status_t dcc_rest_async_cancel_pending(
    dcc_client_t *client,
    size_t *out_canceled
) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_lock(client);
    dcc_rest_async_request_t *request = dcc_rest_async_detach_pending_all_locked(client);
    dcc_rest_unlock(client);
    dcc_rest_async_signal(client);

    size_t canceled = dcc_rest_async_cancel_pending_list(client, request);

    if (out_canceled != NULL) {
        *out_canceled = canceled;
    }
    return DCC_OK;
}

dcc_status_t dcc_rest_async_cancel_active(
    dcc_client_t *client,
    size_t *out_canceled
) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    llam_fd_t fds[DCC_REST_BUCKET_CAP];
    size_t fd_count = 0;
    dcc_rest_lock(client);
    size_t canceled = dcc_rest_async_cancel_active_locked(client, fds, &fd_count);
    dcc_rest_unlock(client);
    dcc_rest_async_signal(client);

    dcc_rest_async_close_cancel_fds(fds, fd_count);
    if (out_canceled != NULL) {
        *out_canceled = canceled;
    }
    return DCC_OK;
}

dcc_status_t dcc_rest_async_cancel_all(
    dcc_client_t *client,
    size_t *out_canceled
) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    llam_fd_t fds[DCC_REST_BUCKET_CAP];
    size_t fd_count = 0;
    dcc_rest_lock(client);
    size_t active_canceled = dcc_rest_async_cancel_active_locked(client, fds, &fd_count);
    dcc_rest_async_request_t *request = dcc_rest_async_detach_pending_all_locked(client);
    dcc_rest_unlock(client);
    dcc_rest_async_signal(client);

    dcc_rest_async_close_cancel_fds(fds, fd_count);

    size_t pending_canceled = dcc_rest_async_cancel_pending_list(client, request);

    if (out_canceled != NULL) {
        *out_canceled = active_canceled + pending_canceled;
    }
    return DCC_OK;
}
