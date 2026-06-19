#include "internal/cache/dcc_cache_internal.h"

#include <stdlib.h>

dcc_status_t dcc_cache_table_put(
    dcc_cache_table_t *table,
    dcc_snowflake_t major,
    dcc_snowflake_t minor,
    void *value,
    dcc_cache_value_free_fn free_value,
    uint64_t sequence,
    size_t max_size
) {
    if (table == NULL || value == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_cache_table_reserve_for_insert(table);
    if (status != DCC_OK) {
        return status;
    }

    size_t index = (size_t)(dcc_cache_hash_pair(major, minor) % table->bucket_count);
    for (dcc_cache_entry_t *entry = table->buckets[index]; entry != NULL; entry = entry->next) {
        if (entry->major == major && entry->minor == minor) {
            free_value(entry->value);
            entry->value = value;
            entry->sequence = sequence;
            dcc_cache_table_trim(table, max_size, free_value);
            return DCC_OK;
        }
    }

    dcc_cache_entry_t *entry = (dcc_cache_entry_t *)calloc(1, sizeof(*entry));
    if (entry == NULL) {
        return DCC_ERR_NOMEM;
    }
    entry->major = major;
    entry->minor = minor;
    entry->sequence = sequence;
    entry->value = value;
    entry->next = table->buckets[index];
    table->buckets[index] = entry;
    table->size++;
    dcc_cache_table_trim(table, max_size, free_value);
    return DCC_OK;
}

void *dcc_cache_table_get(const dcc_cache_table_t *table, dcc_snowflake_t major, dcc_snowflake_t minor) {
    if (table == NULL || table->buckets == NULL || table->bucket_count == 0) {
        return NULL;
    }

    size_t index = (size_t)(dcc_cache_hash_pair(major, minor) % table->bucket_count);
    for (dcc_cache_entry_t *entry = table->buckets[index]; entry != NULL; entry = entry->next) {
        if (entry->major == major && entry->minor == minor) {
            return entry->value;
        }
    }
    return NULL;
}

uint8_t dcc_cache_table_remove(
    dcc_cache_table_t *table,
    dcc_snowflake_t major,
    dcc_snowflake_t minor,
    dcc_cache_value_free_fn free_value
) {
    if (table == NULL || table->buckets == NULL || table->bucket_count == 0) {
        return 0;
    }

    size_t index = (size_t)(dcc_cache_hash_pair(major, minor) % table->bucket_count);
    dcc_cache_entry_t **cursor = &table->buckets[index];
    while (*cursor != NULL) {
        dcc_cache_entry_t *entry = *cursor;
        if (entry->major == major && entry->minor == minor) {
            *cursor = entry->next;
            free_value(entry->value);
            free(entry);
            table->size--;
            return 1;
        }
        cursor = &entry->next;
    }
    return 0;
}
