#ifndef DCC_REST_ASYNC_RETRY_INTERNAL_H
#define DCC_REST_ASYNC_RETRY_INTERNAL_H

#include "internal/rest/dcc_rest_async_request_internal.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t dcc_rest_async_retry_not_before_ms(dcc_rest_async_request_t *request);
int dcc_rest_async_requeue_retry(dcc_rest_async_request_t *request, uint64_t not_before_ms);

#ifdef __cplusplus
}
#endif

#endif
