#include "internal/rest/dcc_rest_rate_limit_buckets_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"

void dcc_rest_update_rate_limit(
    dcc_client_t *client,
    const char *route,
    const dcc_http_response_t *response,
    double retry_after,
    uint8_t is_global
) {
    size_t remaining_len = 0;
    const char *remaining_header = dcc_rest_response_header(response, "X-RateLimit-Remaining", &remaining_len);
    size_t reset_after_len = 0;
    const char *reset_after_header = dcc_rest_response_header(response, "X-RateLimit-Reset-After", &reset_after_len);
    size_t bucket_hash_len = 0;
    const char *bucket_hash = dcc_rest_response_header(response, "X-RateLimit-Bucket", &bucket_hash_len);
    double reset_after = dcc_rest_parse_header_double(reset_after_header, reset_after_len);
    uint32_t remaining = 1;
    int has_remaining = dcc_rest_parse_header_u32(remaining_header, remaining_len, &remaining);

    if (retry_after > reset_after) {
        reset_after = retry_after;
    }

    uint64_t now = dcc_rest_now_ms();
    uint64_t reset_after_ms = dcc_rest_seconds_to_ms(reset_after);
    uint64_t reset_at_ms = reset_after_ms != 0 ? dcc_rest_add_ms(now, reset_after_ms) : 0;

    dcc_rest_lock(client);
    if (is_global && reset_at_ms != 0) {
        client->rest_global_reset_at_ms = reset_at_ms;
    }

    if (response != NULL && (response->status == 429 || has_remaining || reset_at_ms != 0 || bucket_hash != NULL)) {
        dcc_rest_bucket_t *bucket = dcc_rest_find_bucket_locked(client, route, 1);
        if (bucket != NULL) {
            dcc_rest_bucket_set_hash(bucket, bucket_hash, bucket_hash_len);
            if (response->status == 429) {
                bucket->remaining = 0;
                bucket->reset_at_ms = reset_at_ms;
            } else if (has_remaining) {
                bucket->remaining = remaining;
                if (remaining == 0 && reset_at_ms != 0) {
                    bucket->reset_at_ms = reset_at_ms;
                } else if (remaining > 0) {
                    bucket->reset_at_ms = 0;
                }
            }
            if (bucket->bucket[0] != '\0') {
                for (size_t i = 0; i < DCC_REST_BUCKET_CAP; ++i) {
                    dcc_rest_bucket_t *related = &client->rest_buckets[i];
                    if (related != bucket && related->active && dcc_rest_bucket_hash_matches(related, bucket->bucket)) {
                        related->remaining = bucket->remaining;
                        related->reset_at_ms = bucket->reset_at_ms;
                    }
                }
            }
        }
    }
    dcc_rest_unlock(client);
}
