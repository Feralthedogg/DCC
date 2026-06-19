#ifndef DCC_REST_ASYNC_CANCEL_INTERNAL_H
#define DCC_REST_ASYNC_CANCEL_INTERNAL_H

#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_async_request_internal.h"
#include "internal/rest/dcc_rest_async_signal_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"

#include <dcc/rest.h>
#include <llam/io.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_async_cancel_pending(dcc_client_t *client, size_t *out_canceled);
dcc_status_t dcc_rest_async_cancel_active(dcc_client_t *client, size_t *out_canceled);
dcc_status_t dcc_rest_async_cancel_all(dcc_client_t *client, size_t *out_canceled);
size_t dcc_rest_async_cancel_pending_list(
    dcc_client_t *client,
    dcc_rest_async_request_t *request
);
size_t dcc_rest_async_cancel_active_locked(
    dcc_client_t *client,
    llam_fd_t *fds,
    size_t *fd_count
);
void dcc_rest_async_close_cancel_fds(const llam_fd_t *fds, size_t fd_count);

#ifdef __cplusplus
}
#endif

#endif
