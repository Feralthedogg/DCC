#ifndef DCC_CLIENT_CONFIG_STATE_FIELDS_INTERNAL_H
#define DCC_CLIENT_CONFIG_STATE_FIELDS_INTERNAL_H

#include <dcc/cache.h>
#include <dcc/client.h>

#include <stdatomic.h>
#include <stdint.h>

#define DCC_CLIENT_CONFIG_STATE_FIELDS \
    dcc_intents_t intents; \
    uint32_t shard_id; \
    uint32_t shard_count; \
    uint32_t rest_concurrency; \
    uint32_t enable_cache; \
    uint32_t infer_guild_id_from_channel; \
    atomic_uint_fast64_t inferred_channel_id; \
    atomic_uint_fast64_t inferred_guild_id; \
    dcc_cache_policy_t cache_policy

#endif
