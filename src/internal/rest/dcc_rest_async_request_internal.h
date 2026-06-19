#ifndef DCC_REST_ASYNC_REQUEST_INTERNAL_H
#define DCC_REST_ASYNC_REQUEST_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"
#include "internal/rest/dcc_rest_state_internal.h"

#include <dcc/rest.h>
#include <llam/io.h>

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dcc_rest_async_request {
    dcc_client_t *client;
    char *method;
    char *path;
    char *body;
    char route[DCC_REST_ROUTE_KEY_CAP];
    dcc_rest_priority_t priority;
    dcc_rest_cb cb;
    void *user_data;
    int callback_called;
    uint32_t retry_attempt;
    uint64_t not_before_ms;
    atomic_bool cancel_requested;
    _Atomic(llam_fd_t) active_fd;
    dcc_rest_async_request_t *next;
    dcc_rest_async_request_t *active_next;
};

dcc_rest_async_request_t *dcc_rest_async_request_new(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const char *body,
    dcc_rest_priority_t priority,
    dcc_rest_cb cb,
    void *user_data
);
void dcc_rest_async_request_free(dcc_rest_async_request_t *request);
uint32_t dcc_rest_async_concurrency(const dcc_client_t *client);
int dcc_rest_priority_valid(dcc_rest_priority_t priority);
int dcc_rest_async_request_canceled(void *user_data);
llam_fd_t dcc_rest_async_request_swap_fd(void *user_data, llam_fd_t fd);
int dcc_rest_async_request_cancel(dcc_rest_async_request_t *request, llam_fd_t *fds, size_t *fd_count);

#ifdef __cplusplus
}
#endif

#endif
