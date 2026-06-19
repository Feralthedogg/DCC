#ifndef DCC_REST_ASYNC_WAIT_INTERNAL_H
#define DCC_REST_ASYNC_WAIT_INTERNAL_H

#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_async_worker_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

#include <dcc/rest.h>

#include <stdatomic.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_async_wait(dcc_client_t *client, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
