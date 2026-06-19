#ifndef DCC_REST_ASYNC_DRAIN_INTERNAL_H
#define DCC_REST_ASYNC_DRAIN_INTERNAL_H

#include "internal/rest/dcc_rest_async_queue_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_async_drain_locked(dcc_client_t *client);

#ifdef __cplusplus
}
#endif

#endif
