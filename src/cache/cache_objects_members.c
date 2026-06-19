#include "internal/cache/dcc_cache_internal.h"

dcc_status_t dcc_cache_put_member(dcc_client_t *client, const dcc_member_t *member) {
    if (member == NULL || member->guild_id == 0 || member->user.id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_cache_require_enabled(client);
    if (status != DCC_OK) {
        return status;
    }
    dcc_member_t *copy = dcc_member_clone(member);
    if (copy == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_cache_lock(&client->cache);
    status = dcc_cache_table_put(
        &client->cache.members,
        member->guild_id,
        member->user.id,
        copy,
        dcc_cache_free_member_value,
        dcc_cache_next_sequence(&client->cache),
        client->cache_policy.max_members
    );
    dcc_cache_unlock(&client->cache);
    if (status != DCC_OK) {
        dcc_member_free(copy);
    }
    return status;
}

const dcc_member_t *dcc_cache_get_member(
    const dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id
) {
    if (client == NULL || !client->enable_cache || guild_id == 0 || user_id == 0) {
        return NULL;
    }
    dcc_cache_t *cache = (dcc_cache_t *)&client->cache;
    dcc_cache_lock(cache);
    const dcc_member_t *value = (const dcc_member_t *)dcc_cache_table_get(&client->cache.members, guild_id, user_id);
    dcc_cache_unlock(cache);
    return value;
}

uint8_t dcc_cache_remove_member(dcc_client_t *client, dcc_snowflake_t guild_id, dcc_snowflake_t user_id) {
    if (client == NULL || !client->enable_cache || guild_id == 0 || user_id == 0) {
        return 0;
    }
    dcc_cache_lock(&client->cache);
    uint8_t removed = dcc_cache_table_remove(&client->cache.members, guild_id, user_id, dcc_cache_free_member_value);
    dcc_cache_unlock(&client->cache);
    return removed;
}
