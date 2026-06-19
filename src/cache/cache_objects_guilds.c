#include "internal/cache/dcc_cache_internal.h"

dcc_status_t dcc_cache_put_guild(dcc_client_t *client, const dcc_guild_t *guild) {
    if (guild == NULL || guild->id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }
    dcc_guild_t *copy = dcc_guild_clone(guild);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_cache_lock(&client->cache);
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

const dcc_guild_t *dcc_cache_get_guild(const dcc_client_t *client, dcc_snowflake_t guild_id) {
    if (client == NULL || !client->enable_cache || guild_id == 0) {
        return NULL;
    }
    dcc_cache_t *cache = (dcc_cache_t *)&client->cache;
    dcc_cache_lock(cache);
    const dcc_guild_t *value = (const dcc_guild_t *)dcc_cache_table_get(&client->cache.guilds, guild_id, 0);
    dcc_cache_unlock(cache);
    return value;
}

uint8_t dcc_cache_remove_guild(dcc_client_t *client, dcc_snowflake_t guild_id) {
    if (client == NULL || !client->enable_cache || guild_id == 0) {
        return 0;
    }
    dcc_cache_lock(&client->cache);
    uint8_t removed = dcc_cache_table_remove(&client->cache.guilds, guild_id, 0, dcc_cache_free_guild_value);
    dcc_cache_unlock(&client->cache);
    return removed;
}
