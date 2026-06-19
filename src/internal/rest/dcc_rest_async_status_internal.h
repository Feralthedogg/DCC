#ifndef DCC_REST_ASYNC_STATUS_INTERNAL_H
#define DCC_REST_ASYNC_STATUS_INTERNAL_H

#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_rate_limit_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

#include <dcc/rest.h>

#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_async_status(dcc_client_t *client, dcc_rest_async_status_t *out);
dcc_status_t dcc_rest_async_pending(dcc_client_t *client, size_t *out_pending, size_t *out_active);
void dcc_rest_async_status_snapshot_locked(
    dcc_client_t *client,
    uint64_t now,
    dcc_rest_async_status_t *snapshot
);

#ifdef __cplusplus
}
#endif

#endif
