#ifndef DCC_CLIENT_DM_CACHE_STATE_FIELDS_INTERNAL_H
#define DCC_CLIENT_DM_CACHE_STATE_FIELDS_INTERNAL_H

#include <dcc/snowflake.h>

#include <stdatomic.h>
#include <stdint.h>

#define DCC_DM_CHANNEL_CACHE_CAP 128U

typedef struct dcc_dm_channel_cache_entry {
    dcc_snowflake_t user_id;
    dcc_snowflake_t channel_id;
    uint64_t sequence;
} dcc_dm_channel_cache_entry_t;

#define DCC_CLIENT_DM_CACHE_STATE_FIELDS \
    atomic_flag dm_channel_cache_lock; \
    uint64_t dm_channel_cache_sequence; \
    uint32_t dm_channel_cache_len; \
    dcc_dm_channel_cache_entry_t dm_channel_cache[DCC_DM_CHANNEL_CACHE_CAP]

#endif
