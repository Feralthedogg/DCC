#include "internal/cache/dcc_cache_internal.h"

dcc_status_t dcc_cache_merge_guild(dcc_client_t *client, const dcc_guild_t *guild, uint64_t fields) {
    if (guild == NULL || guild->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }

    dcc_cache_lock(&client->cache);
    const dcc_guild_t *existing = (const dcc_guild_t *)dcc_cache_table_get(&client->cache.guilds, guild->id, 0);
    dcc_guild_t merged = *guild;
    if (existing != NULL) {
        dcc_cache_merge_guild_view(&merged, existing, fields);
    }
    dcc_guild_t *copy = dcc_guild_clone(&merged);
    if (copy == NULL) {
        dcc_cache_unlock(&client->cache);
        return DCC_ERR_NOMEM;
    }
    status = dcc_cache_table_put(
        &client->cache.guilds,
        guild->id,
        0,
        copy,
        dcc_cache_free_guild_value,
        dcc_cache_next_sequence(&client->cache),
        client->cache_policy.max_guilds
    );
    dcc_cache_unlock(&client->cache);
    if (status != DCC_OK) {
        dcc_guild_free(copy);
    }
    return status;
}
