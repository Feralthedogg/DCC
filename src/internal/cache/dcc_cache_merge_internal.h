#ifndef DCC_CACHE_MERGE_INTERNAL_H
#define DCC_CACHE_MERGE_INTERNAL_H

#include <dcc/dcc.h>

#include <stdint.h>

#include "internal/cache/dcc_cache_merge_guild_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

void dcc_cache_merge_user_view(dcc_user_t *merged, const dcc_user_t *existing, uint64_t fields);
void dcc_cache_merge_message_view(dcc_message_t *merged, const dcc_message_t *existing, uint64_t fields);
void dcc_cache_merge_channel_view(dcc_channel_t *merged, const dcc_channel_t *existing, uint64_t fields);
void dcc_cache_merge_role_view(dcc_role_t *merged, const dcc_role_t *existing, uint64_t fields);
void dcc_cache_merge_member_view(dcc_member_t *merged, const dcc_member_t *existing, uint64_t fields);
void dcc_cache_merge_guild_view(dcc_guild_t *merged, const dcc_guild_t *existing, uint64_t fields);
void dcc_cache_merge_voice_state_view(
    dcc_voice_state_t *merged,
    const dcc_voice_state_t *existing,
    uint64_t fields
);

#ifdef __cplusplus
}
#endif

#endif
