#ifndef DCC_REST_ASYNC_QUEUE_INTERNAL_H
#define DCC_REST_ASYNC_QUEUE_INTERNAL_H

#include "internal/rest/dcc_rest_async_request_internal.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_rest_async_queue_position {
    uint32_t priority;
    dcc_rest_async_request_t *previous;
    dcc_rest_async_request_t *next;
} dcc_rest_async_queue_position_t;

int dcc_rest_async_route_active_locked(const dcc_client_t *client, const char *route);
uint64_t dcc_rest_async_request_wait_ms_locked(
    dcc_client_t *client,
    const dcc_rest_async_request_t *request,
    uint64_t now,
    int reset_expired
);
uint64_t dcc_rest_async_next_wake_ms_locked(dcc_client_t *client, uint64_t now);
dcc_status_t dcc_rest_async_mark_route_active_locked(dcc_client_t *client, const char *route);
void dcc_rest_async_unmark_route_active_locked(dcc_client_t *client, const char *route);
void dcc_rest_async_push_active_locked(dcc_client_t *client, dcc_rest_async_request_t *request);
void dcc_rest_async_remove_active_locked(dcc_client_t *client, dcc_rest_async_request_t *request);
void dcc_rest_async_push_tail_locked(dcc_client_t *client, dcc_rest_async_request_t *request);
void dcc_rest_async_push_head_locked(dcc_client_t *client, dcc_rest_async_request_t *request);
dcc_rest_async_request_t *dcc_rest_async_take_next_locked(
    dcc_client_t *client,
    dcc_rest_async_queue_position_t *position
);
void dcc_rest_async_restore_locked(
    dcc_client_t *client,
    dcc_rest_async_request_t *request,
    const dcc_rest_async_queue_position_t *position
);
size_t dcc_rest_async_pending_count_locked(const dcc_client_t *client);
dcc_rest_async_request_t *dcc_rest_async_detach_pending_all_locked(dcc_client_t *client);

#ifdef __cplusplus
}
#endif

#endif
