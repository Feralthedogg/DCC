#ifndef DCC_CACHE_TABLE_INTERNAL_H
#define DCC_CACHE_TABLE_INTERNAL_H

#include "internal/cache/dcc_cache_state_internal.h"
#include "internal/cache/dcc_cache_value_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t dcc_cache_next_sequence(dcc_cache_t *cache);
uint64_t dcc_cache_hash_pair(dcc_snowflake_t major, dcc_snowflake_t minor);
dcc_status_t dcc_cache_table_reserve_for_insert(dcc_cache_table_t *table);
void dcc_cache_table_resequence(dcc_cache_table_t *table, uint64_t *sequence);
void dcc_cache_table_clear(dcc_cache_table_t *table, dcc_cache_value_free_fn free_value);
void dcc_cache_table_deinit(dcc_cache_table_t *table, dcc_cache_value_free_fn free_value);
void dcc_cache_table_trim(
    dcc_cache_table_t *table,
    size_t max_size,
    dcc_cache_value_free_fn free_value
);
dcc_status_t dcc_cache_table_put(
    dcc_cache_table_t *table,
    dcc_snowflake_t major,
    dcc_snowflake_t minor,
    void *value,
    dcc_cache_value_free_fn free_value,
    uint64_t sequence,
    size_t max_size
);
void *dcc_cache_table_get(const dcc_cache_table_t *table, dcc_snowflake_t major, dcc_snowflake_t minor);
uint8_t dcc_cache_table_remove(
    dcc_cache_table_t *table,
    dcc_snowflake_t major,
    dcc_snowflake_t minor,
    dcc_cache_value_free_fn free_value
);

#ifdef __cplusplus
}
#endif

#endif
