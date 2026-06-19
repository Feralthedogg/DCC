#ifndef DCC_CACHE_MERGE_GUILD_INTERNAL_H
#define DCC_CACHE_MERGE_GUILD_INTERNAL_H

#include <dcc/dcc.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_cache_merge_guild_identity_fields(dcc_guild_t *merged, const dcc_guild_t *existing);
void dcc_cache_merge_guild_string_fields(dcc_guild_t *merged, const dcc_guild_t *existing);
void dcc_cache_merge_guild_collection_fields(dcc_guild_t *merged, const dcc_guild_t *existing, uint64_t fields);
void dcc_cache_merge_guild_limit_fields(dcc_guild_t *merged, const dcc_guild_t *existing, uint64_t fields);
void dcc_cache_merge_guild_flag_fields(dcc_guild_t *merged, const dcc_guild_t *existing, uint64_t fields);

#ifdef __cplusplus
}
#endif

#endif
