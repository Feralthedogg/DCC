#include "internal/cache/dcc_cache_merge_guild_internal.h"

void dcc_cache_merge_guild_view(dcc_guild_t *merged, const dcc_guild_t *existing, uint64_t fields) {
    if (merged == NULL || existing == NULL) {
        return;
    }
    dcc_cache_merge_guild_identity_fields(merged, existing);
    dcc_cache_merge_guild_string_fields(merged, existing);
    dcc_cache_merge_guild_collection_fields(merged, existing, fields);
    dcc_cache_merge_guild_limit_fields(merged, existing, fields);
    dcc_cache_merge_guild_flag_fields(merged, existing, fields);
}
