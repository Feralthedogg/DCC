#include "internal/rest/dcc_rest_rate_limit_buckets_internal.h"

#include <stdio.h>
#include <string.h>

dcc_rest_bucket_t *dcc_rest_find_bucket_locked(dcc_client_t *client, const char *route, int create) {
    dcc_rest_bucket_t *free_slot = NULL;
    dcc_rest_bucket_t *oldest = &client->rest_buckets[0];

    for (size_t i = 0; i < DCC_REST_BUCKET_CAP; ++i) {
        dcc_rest_bucket_t *bucket = &client->rest_buckets[i];
        if (bucket->active) {
            if (strncmp(bucket->route, route, sizeof(bucket->route)) == 0) {
                return bucket;
            }
            if (bucket->reset_at_ms < oldest->reset_at_ms) {
                oldest = bucket;
            }
        } else if (free_slot == NULL) {
            free_slot = bucket;
        }
    }

    if (!create) {
        return NULL;
    }

    dcc_rest_bucket_t *bucket = free_slot != NULL ? free_slot : oldest;
    memset(bucket, 0, sizeof(*bucket));
    snprintf(bucket->route, sizeof(bucket->route), "%s", route);
    bucket->remaining = 1;
    bucket->active = 1;
    return bucket;
}

void dcc_rest_bucket_set_hash(dcc_rest_bucket_t *bucket, const char *hash, size_t hash_len) {
    if (bucket == NULL || hash == NULL || hash_len == 0) {
        return;
    }
    if (hash_len >= sizeof(bucket->bucket)) {
        hash_len = sizeof(bucket->bucket) - 1U;
    }
    memcpy(bucket->bucket, hash, hash_len);
    bucket->bucket[hash_len] = '\0';
}

int dcc_rest_bucket_hash_matches(const dcc_rest_bucket_t *bucket, const char *hash) {
    return bucket != NULL && hash != NULL && hash[0] != '\0' && strcmp(bucket->bucket, hash) == 0;
}

void dcc_rest_bucket_collect_wait(
    dcc_rest_bucket_t *bucket,
    uint64_t now,
    uint64_t *wait_ms,
    int reset_expired
) {
    if (bucket == NULL || wait_ms == NULL || bucket->remaining != 0) {
        return;
    }
    if (bucket->reset_at_ms > now) {
        uint64_t bucket_wait = bucket->reset_at_ms - now;
        if (bucket_wait > *wait_ms) {
            *wait_ms = bucket_wait;
        }
    } else if (reset_expired) {
        bucket->remaining = 1;
        bucket->reset_at_ms = 0;
    }
}
