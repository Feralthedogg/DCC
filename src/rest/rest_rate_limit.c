#include "internal/rest/dcc_rest_rate_limit_buckets_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

void dcc_rest_lock(dcc_client_t *client) {
    while (atomic_flag_test_and_set_explicit(&client->rest_lock, memory_order_acquire)) {
    }
}

void dcc_rest_unlock(dcc_client_t *client) {
    atomic_flag_clear_explicit(&client->rest_lock, memory_order_release);
}

uint64_t dcc_rest_route_wait_ms_locked(
    dcc_client_t *client,
    const char *route,
    uint64_t now,
    int reset_expired
) {
    uint64_t wait_ms = 0;
    if (client == NULL) {
        return 0;
    }
    if (client->rest_global_reset_at_ms > now) {
        wait_ms = client->rest_global_reset_at_ms - now;
    }

    dcc_rest_bucket_t *bucket = dcc_rest_find_bucket_locked(client, route, 0);
    if (bucket != NULL) {
        dcc_rest_bucket_collect_wait(bucket, now, &wait_ms, reset_expired);
        if (bucket->bucket[0] != '\0') {
            for (size_t i = 0; i < DCC_REST_BUCKET_CAP; ++i) {
                dcc_rest_bucket_t *related = &client->rest_buckets[i];
                if (related->active && dcc_rest_bucket_hash_matches(related, bucket->bucket)) {
                    dcc_rest_bucket_collect_wait(related, now, &wait_ms, reset_expired);
                }
            }
        }
    }
    return wait_ms;
}

void dcc_rest_wait_for_route(dcc_client_t *client, const char *route) {
    for (;;) {
        uint64_t now = dcc_rest_now_ms();
        dcc_rest_lock(client);
        uint64_t wait_ms = dcc_rest_route_wait_ms_locked(client, route, now, 1);
        dcc_rest_unlock(client);

        if (wait_ms == 0) {
            return;
        }
        dcc_rest_sleep_ms(wait_ms);
    }
}
