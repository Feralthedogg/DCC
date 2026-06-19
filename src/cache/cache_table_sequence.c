#include "internal/cache/dcc_cache_internal.h"

void dcc_cache_table_resequence(dcc_cache_table_t *table, uint64_t *sequence) {
    if (table == NULL || sequence == NULL || table->buckets == NULL) {
        return;
    }
    for (size_t i = 0; i < table->bucket_count; i++) {
        for (dcc_cache_entry_t *entry = table->buckets[i]; entry != NULL; entry = entry->next) {
            if (*sequence < UINT64_MAX) {
                (*sequence)++;
            }
            entry->sequence = *sequence;
        }
    }
}

uint64_t dcc_cache_next_sequence(dcc_cache_t *cache) {
    if (cache == NULL) {
        return 0;
    }
    if (cache->sequence == UINT64_MAX) {
        uint64_t sequence = 0;
        dcc_cache_table_resequence(&cache->guilds, &sequence);
        dcc_cache_table_resequence(&cache->channels, &sequence);
        dcc_cache_table_resequence(&cache->roles, &sequence);
        dcc_cache_table_resequence(&cache->members, &sequence);
        dcc_cache_table_resequence(&cache->users, &sequence);
        dcc_cache_table_resequence(&cache->messages, &sequence);
        dcc_cache_table_resequence(&cache->voice_states, &sequence);
        cache->sequence = sequence;
    }
    return ++cache->sequence;
}
