#include "internal/cache/dcc_cache_internal.h"

#include <stdlib.h>

#define DCC_CACHE_INITIAL_BUCKETS 256U

uint64_t dcc_cache_hash_pair(dcc_snowflake_t major, dcc_snowflake_t minor) {
    uint64_t x = major + UINT64_C(0x9E3779B97F4A7C15);
    x ^= minor + UINT64_C(0xBF58476D1CE4E5B9) + (x << 6U) + (x >> 2U);
    x ^= x >> 30U;
    x *= UINT64_C(0xBF58476D1CE4E5B9);
    x ^= x >> 27U;
    x *= UINT64_C(0x94D049BB133111EB);
    x ^= x >> 31U;
    return x;
}

static dcc_status_t dcc_cache_table_resize(dcc_cache_table_t *table, size_t bucket_count) {
    if (table == NULL ||
        bucket_count == 0 ||
        bucket_count > SIZE_MAX / sizeof(*table->buckets)) {
        return DCC_ERR_NOMEM;
    }

    dcc_cache_entry_t **next_buckets = (dcc_cache_entry_t **)calloc(bucket_count, sizeof(*next_buckets));
    if (next_buckets == NULL) {
        return DCC_ERR_NOMEM;
    }

    if (table->buckets != NULL) {
        for (size_t i = 0; i < table->bucket_count; i++) {
            dcc_cache_entry_t *entry = table->buckets[i];
            while (entry != NULL) {
                dcc_cache_entry_t *next = entry->next;
                size_t index = (size_t)(dcc_cache_hash_pair(entry->major, entry->minor) % bucket_count);
                entry->next = next_buckets[index];
                next_buckets[index] = entry;
                entry = next;
            }
        }
    }

    free(table->buckets);
    table->buckets = next_buckets;
    table->bucket_count = bucket_count;
    return DCC_OK;
}

dcc_status_t dcc_cache_table_reserve_for_insert(dcc_cache_table_t *table) {
    if (table == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (table->bucket_count == 0) {
        return dcc_cache_table_resize(table, DCC_CACHE_INITIAL_BUCKETS);
    }
    if (table->bucket_count > SIZE_MAX / 2U) {
        return DCC_ERR_NOMEM;
    }
    size_t next_bucket_count = table->bucket_count * 2U;
    if (table->size <= next_bucket_count) {
        return DCC_OK;
    }
    return dcc_cache_table_resize(table, next_bucket_count);
}
