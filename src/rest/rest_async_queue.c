#include "internal/rest/dcc_rest_async_queue_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

void dcc_rest_async_push_tail_locked(dcc_client_t *client, dcc_rest_async_request_t *request) {
    uint32_t priority = (uint32_t)request->priority;
    request->next = NULL;
    if (client->rest_async_tails[priority] != NULL) {
        client->rest_async_tails[priority]->next = request;
    } else {
        client->rest_async_heads[priority] = request;
    }
    client->rest_async_tails[priority] = request;
}

void dcc_rest_async_push_head_locked(dcc_client_t *client, dcc_rest_async_request_t *request) {
    uint32_t priority = (uint32_t)request->priority;
    request->next = client->rest_async_heads[priority];
    client->rest_async_heads[priority] = request;
    if (client->rest_async_tails[priority] == NULL) {
        client->rest_async_tails[priority] = request;
    }
}

dcc_rest_async_request_t *dcc_rest_async_take_next_locked(
    dcc_client_t *client,
    dcc_rest_async_queue_position_t *position
) {
    uint64_t now = dcc_rest_now_ms();
    for (int priority = (int)DCC_REST_PRIORITY_HIGH; priority >= (int)DCC_REST_PRIORITY_LOW; --priority) {
        dcc_rest_async_request_t *prev = NULL;
        dcc_rest_async_request_t *request = client->rest_async_heads[priority];
        while (request != NULL) {
            int canceled = atomic_load_explicit(
                &request->cancel_requested,
                memory_order_acquire
            );
            if (canceled ||
                (!dcc_rest_async_route_active_locked(client, request->route) &&
                 dcc_rest_async_request_wait_ms_locked(client, request, now, 1) == 0)) {
                if (position != NULL) {
                    position->priority = (uint32_t)priority;
                    position->previous = prev;
                    position->next = request->next;
                }
                if (prev != NULL) {
                    prev->next = request->next;
                } else {
                    client->rest_async_heads[priority] = request->next;
                }
                if (client->rest_async_tails[priority] == request) {
                    client->rest_async_tails[priority] = prev;
                }
                request->next = NULL;
                return request;
            }
            prev = request;
            request = request->next;
        }
    }
    return NULL;
}

void dcc_rest_async_restore_locked(
    dcc_client_t *client,
    dcc_rest_async_request_t *request,
    const dcc_rest_async_queue_position_t *position
) {
    if (client == NULL || request == NULL || position == NULL ||
        position->priority >= DCC_REST_PRIORITY_LEVELS) {
        return;
    }
    request->next = position->next;
    if (position->previous != NULL) {
        position->previous->next = request;
    } else {
        client->rest_async_heads[position->priority] = request;
    }
    if (position->next == NULL) {
        client->rest_async_tails[position->priority] = request;
    }
}
