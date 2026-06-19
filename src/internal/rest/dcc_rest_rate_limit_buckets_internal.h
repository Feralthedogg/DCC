#ifndef DCC_REST_RATE_LIMIT_BUCKETS_INTERNAL_H
#define DCC_REST_RATE_LIMIT_BUCKETS_INTERNAL_H

#include "internal/rest/dcc_rest_rate_limit_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_rest_bucket_t *dcc_rest_find_bucket_locked(dcc_client_t *client, const char *route, int create);
void dcc_rest_bucket_set_hash(dcc_rest_bucket_t *bucket, const char *hash, size_t hash_len);
int dcc_rest_bucket_hash_matches(const dcc_rest_bucket_t *bucket, const char *hash);
void dcc_rest_bucket_collect_wait(
    dcc_rest_bucket_t *bucket,
    uint64_t now,
    uint64_t *wait_ms,
    int reset_expired
);

#ifdef __cplusplus
}
#endif

#endif
