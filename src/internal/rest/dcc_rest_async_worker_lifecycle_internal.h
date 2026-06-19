#ifndef DCC_REST_ASYNC_WORKER_LIFECYCLE_INTERNAL_H
#define DCC_REST_ASYNC_WORKER_LIFECYCLE_INTERNAL_H

#include "internal/rest/dcc_rest_async_queue_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

void dcc_rest_async_finish_active_locked(dcc_client_t *client, dcc_rest_async_request_t *request);
void dcc_rest_async_complete(dcc_rest_async_request_t *request);

#ifdef __cplusplus
}
#endif

#endif
