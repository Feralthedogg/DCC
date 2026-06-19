#include "internal/rest/dcc_rest_async_cancel_internal.h"

size_t dcc_rest_async_cancel_active_locked(
    dcc_client_t *client,
    llam_fd_t *fds,
    size_t *fd_count
) {
    size_t canceled = 0;
    for (dcc_rest_async_request_t *request = client->rest_async_active_head;
         request != NULL;
         request = request->active_next) {
        canceled += dcc_rest_async_request_cancel(request, fds, fd_count) ? 1U : 0U;
    }
    return canceled;
}

void dcc_rest_async_close_cancel_fds(const llam_fd_t *fds, size_t fd_count) {
    for (size_t i = 0; i < fd_count; ++i) {
        if (!LLAM_FD_IS_INVALID(fds[i])) {
            (void)llam_close(fds[i]);
        }
    }
}
