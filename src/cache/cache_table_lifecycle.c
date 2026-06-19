#include "internal/cache/dcc_cache_internal.h"

#include <stdlib.h>

void dcc_cache_table_clear(dcc_cache_table_t *table, dcc_cache_value_free_fn free_value) {
    if (table == NULL || table->buckets == NULL) {
        return;
    }

    for (size_t i = 0; i < table->bucket_count; i++) {
        dcc_cache_entry_t *entry = table->buckets[i];
        while (entry != NULL) {
            dcc_cache_entry_t *next = entry->next;
            free_value(entry->value);
            free(entry);
            entry = next;
        }
        table->buckets[i] = NULL;
    }
    table->size = 0;
}

void dcc_cache_table_deinit(dcc_cache_table_t *table, dcc_cache_value_free_fn free_value) {
    if (table == NULL) {
        return;
    }
    dcc_cache_table_clear(table, free_value);
    free(table->buckets);
    table->buckets = NULL;
    table->bucket_count = 0;
}

static uint8_t dcc_cache_table_evict_oldest(
    dcc_cache_table_t *table,
    dcc_cache_value_free_fn free_value
) {
    if (table == NULL || table->buckets == NULL || table->bucket_count == 0 || table->size == 0) {
        return 0;
    }

    dcc_cache_entry_t **oldest_cursor = NULL;
    uint64_t oldest_sequence = UINT64_MAX;
    for (size_t i = 0; i < table->bucket_count; i++) {
        dcc_cache_entry_t **cursor = &table->buckets[i];
        while (*cursor != NULL) {
            dcc_cache_entry_t *entry = *cursor;
            if (oldest_cursor == NULL || entry->sequence < oldest_sequence) {
                oldest_cursor = cursor;
                oldest_sequence = entry->sequence;
            }
            cursor = &entry->next;
        }
    }

    if (oldest_cursor == NULL || *oldest_cursor == NULL) {
        return 0;
    }

    dcc_cache_entry_t *oldest = *oldest_cursor;
    *oldest_cursor = oldest->next;
    free_value(oldest->value);
    free(oldest);
    table->size--;
    return 1;
}

void dcc_cache_table_trim(
    dcc_cache_table_t *table,
    size_t max_size,
    dcc_cache_value_free_fn free_value
) {
    if (max_size == 0) {
        return;
    }
    while (table != NULL && table->size > max_size) {
        if (!dcc_cache_table_evict_oldest(table, free_value)) {
            return;
        }
    }
}
