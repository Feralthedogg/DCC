#include "internal/rest/dcc_rest_async_status_internal.h"

void dcc_rest_async_status_snapshot_locked(
    dcc_client_t *client,
    uint64_t now,
    dcc_rest_async_status_t *snapshot
) {
    snapshot->active = client->rest_async_active;
    snapshot->active_routes = client->rest_async_active_route_count;
    snapshot->concurrency = dcc_rest_async_concurrency(client);
    for (dcc_rest_async_request_t *active_request = client->rest_async_active_head;
         active_request != NULL;
         active_request = active_request->active_next) {
        if (atomic_load_explicit(&active_request->cancel_requested, memory_order_acquire)) {
            snapshot->active_cancel_requested++;
        }
    }
    snapshot->active_capacity = snapshot->concurrency > snapshot->active
        ? (size_t)(snapshot->concurrency - snapshot->active)
        : 0U;
    if (client->rest_global_reset_at_ms > now) {
        snapshot->global_wait_ms = client->rest_global_reset_at_ms - now;
    }
    for (uint32_t priority = 0; priority < DCC_REST_PRIORITY_LEVELS; ++priority) {
        size_t count = 0;
        const dcc_rest_async_request_t *request = client->rest_async_heads[priority];
        while (request != NULL) {
            count++;
            uint64_t pending_wait_ms = dcc_rest_async_request_wait_ms_locked(client, request, now, 0);
            if (request->retry_attempt != 0) {
                snapshot->pending_retry++;
                if (pending_wait_ms != 0) {
                    if (snapshot->next_retry_wait_ms == 0 || pending_wait_ms < snapshot->next_retry_wait_ms) {
                        snapshot->next_retry_wait_ms = pending_wait_ms;
                    }
                    if (pending_wait_ms > snapshot->max_retry_wait_ms) {
                        snapshot->max_retry_wait_ms = pending_wait_ms;
                    }
                }
            }
            if (dcc_rest_async_route_active_locked(client, request->route)) {
                snapshot->pending_blocked_by_active_route++;
            } else if (pending_wait_ms != 0) {
                snapshot->pending_blocked_by_rate_limit++;
            } else {
                snapshot->pending_ready++;
            }
            request = request->next;
        }
        snapshot->pending += count;
        if (priority == (uint32_t)DCC_REST_PRIORITY_LOW) {
            snapshot->pending_low = count;
        } else if (priority == (uint32_t)DCC_REST_PRIORITY_NORMAL) {
            snapshot->pending_normal = count;
        } else if (priority == (uint32_t)DCC_REST_PRIORITY_HIGH) {
            snapshot->pending_high = count;
        }
    }
    for (size_t i = 0; i < DCC_REST_BUCKET_CAP; ++i) {
        const dcc_rest_bucket_t *bucket = &client->rest_buckets[i];
        if (!bucket->active) {
            continue;
        }
        snapshot->rate_limit_buckets++;
        if (bucket->remaining == 0 && bucket->reset_at_ms > now) {
            uint64_t wait_ms = bucket->reset_at_ms - now;
            snapshot->rate_limit_blocked_buckets++;
            if (snapshot->next_bucket_wait_ms == 0 || wait_ms < snapshot->next_bucket_wait_ms) {
                snapshot->next_bucket_wait_ms = wait_ms;
            }
            if (wait_ms > snapshot->max_bucket_wait_ms) {
                snapshot->max_bucket_wait_ms = wait_ms;
            }
        }
    }
}
